#include <mos/assets.hpp>

#include <algorithm>
#include <cstring>
#include <exception>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <iterator>
#include <lodepng.h>
#include <memory>
#include <mos/util.hpp>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <stb_vorbis.h>

namespace mos {
using namespace std;
using namespace glm;
using namespace nlohmann;

Assets::Assets(const std::string directory) : directory_(directory) {}

Assets::~Assets() {}

Model Assets::model_value(const json &value) {
  auto name = value.value("name", "");
  auto mesh = value.value("mesh", "");
  auto wrap = value.value("wrap", "repeat");
  auto texture_name = value.value("texture", "");
  auto texture2_name = value.value("texture2", "");
  auto lightmap_name =
      value["lightmap"].is_null() ? "" : value.value("lightmap", "");
  auto normalmap_name = value.value("normalmap", "");
  std::string material_name = value.value("material", "");
  bool recieves_light = value.value("receives_light", true);

  // glm::mat4 transform = glm::mat4(1.0f);

  auto transform = jsonarray_to_mat4(value["transform"]);
  /*
    if (!value["transform"].is_null()) {
      std::vector<float> nums;
      for (auto &num : value["transform"]) {
        nums.push_back(num);
      }
      transform = glm::make_mat4x4(nums.data());
    }*/

  static std::map<std::string, mos::Texture::Wrap> wrap_map{
      {"clamp", mos::Texture::Wrap::CLAMP},
      {"repeat", mos::Texture::Wrap::REPEAT}};

  auto created_model = mos::Model(
      name, mesh_cached(mesh),
      Textures(texture_cached(texture_name, true, wrap_map[wrap]), texture_cached(texture2_name, true, wrap_map[wrap])),
      transform, material_cached(material_name),
      Lightmaps(texture_cached(lightmap_name)), texture_cached(normalmap_name),
      recieves_light);

  for (auto &m : value["models"]) {
    created_model.models.push_back(model_value(m));
  }

  return created_model;
}

Model Assets::model(const std::string &path) {
  auto doc = json::parse(mos::text(directory_ + path));
  return model_value(doc);
}

Animation Assets::animation(const string &path) {
  auto doc = json::parse(mos::text(directory_ + path));
  auto frame_rate = doc["frame_rate"];
  std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes;

  for (auto &keyframe : doc["keyframes"]) {
    auto key = keyframe["key"];
    auto mesh_path = keyframe["mesh"];
    keyframes.insert({key, mesh_cached(mesh_path)});
  }
  Animation animation(keyframes, frame_rate);
  return animation;
}

std::shared_ptr<Mesh> Assets::mesh(const std::string &path) const {
  if (path.empty()) {
    return std::make_shared<Mesh>(Mesh());
  } else {
    if (path.substr(path.find_last_of(".") + 1) == "mesh") {
      std::ifstream is(directory_ + path, std::ios::binary);
      if (!is.good()) {
        throw std::runtime_error(directory_ + path + " does not exist.");
      }
      int num_vertices;
      int num_indices;
      is.read((char *)&num_vertices, sizeof(int));
      is.read((char *)&num_indices, sizeof(int));

      auto vertices = std::vector<mos::Vertex>(num_vertices);
      auto elements = std::vector<int>(num_indices);

      if (vertices.size() > 0) {
        is.read((char *)&vertices[0], vertices.size() * sizeof(Vertex));
      }

      if (elements.size() > 0) {
        is.read((char *)&elements[0], elements.size() * sizeof(int));
      }
      return std::make_shared<Mesh>(Mesh(vertices.begin(), vertices.end(),
                                         elements.begin(), elements.end()));
    } else {
      throw std::runtime_error("File extension not supported.");
    }
  }
}

std::shared_ptr<Mesh> Assets::mesh_cached(const std::string &path) {
  if (meshes_.find(path) == meshes_.end()) {
    meshes_.insert(MeshPair(path, mesh(path)));
  }
  return meshes_.at(path);
}

std::shared_ptr<Texture> Assets::texture(const std::string &path,
                                         const bool mipmaps,
                                         const Texture::Wrap &wrap) const {
  std::vector<unsigned char> texels;
  unsigned int width, height;

  if (path.empty()) {
    return std::shared_ptr<Texture>(nullptr);
  }
  auto error = lodepng::decode(texels, width, height, directory_ + path);
  if (error) {
    std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error)
              << std::endl;
  }

  return std::make_shared<Texture>(texels.begin(), texels.end(), width, height,
                                   mipmaps, wrap);
}

std::shared_ptr<Texture>
Assets::texture_cached(const std::string &path, const bool mipmaps,
                       const Texture::Wrap &wrap) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(TexturePair(path, texture(path, mipmaps, wrap)));
    }
    return textures_.at(path);
  } else {
    return std::shared_ptr<Texture>(nullptr);
  }
}

std::shared_ptr<AudioBuffer>
Assets::audio_buffer(const std::string &path) const {
  int channels;
  int length;
  int sample_rate;
  short *decoded;

  std::ifstream file(directory_ + path, std::ios::binary);
  if (!file.good()) {
    throw std::runtime_error(directory_ + path + " does not exist.");
  }
  std::vector<unsigned char> data;

  unsigned char c;
  while (file.read(reinterpret_cast<char *>(&c), sizeof(c))) {
    data.push_back(c);
  }
  length = stb_vorbis_decode_memory(data.data(), data.size(), &channels,
                                    &sample_rate, &decoded);
  return std::make_shared<AudioBuffer>(
      AudioBuffer(decoded, decoded + length, channels, sample_rate));
}

std::shared_ptr<AudioStream> Assets::audio_stream(const string &path) const {
  return std::make_shared<AudioStream>(directory_ + path);
}

Font Assets::font(const string &path) {
  std::map<char, Character> characters;
  rapidxml::xml_document<> doc;
  auto xml_string = text(directory_ + path);
  doc.parse<0>(&xml_string[0]);

  auto *chars_node = doc.first_node("font")->first_node("chars");
  auto *description_node = doc.first_node("font")->first_node("description");
  std::string name = description_node->first_attribute("family")->value();
  float size = atof(description_node->first_attribute("family")->value());

  auto *metrics_node = doc.first_node("font")->first_node("metrics");
  float height = atof(metrics_node->first_attribute("height")->value());
  float ascender = atof(metrics_node->first_attribute("ascender")->value());
  float descender = atof(metrics_node->first_attribute("descender")->value());

  auto *texture_node = doc.first_node("font")->first_node("texture");
  std::string file = texture_node->first_attribute("file")->value();
  // std::transform(file.begin(), file.end(), file.begin(), ::tolower);

  for (auto *char_node = chars_node->first_node("char"); char_node;
       char_node = char_node->next_sibling()) {

    Character character;

    character.offset_x = atof(char_node->first_attribute("offset_x")->value());
    character.offset_y = atof(char_node->first_attribute("offset_y")->value());
    character.advance = atof(char_node->first_attribute("advance")->value());
    character.rect_w = atof(char_node->first_attribute("rect_w")->value());
    character.id = *char_node->first_attribute("id")->value();
    character.rect_x = atof(char_node->first_attribute("rect_x")->value());
    character.rect_y = atof(char_node->first_attribute("rect_y")->value());
    character.rect_h = atof(char_node->first_attribute("rect_h")->value());
    characters.insert(std::pair<char, Character>(character.id, character));
  }

  auto char_map = characters;
  auto texture = texture_cached(file);
  return Font(char_map, texture, height, ascender, descender);
}

std::shared_ptr<AudioBuffer>
Assets::audio_buffer_cached(const std::string &path) {
  if (sounds_.find(path) == sounds_.end()) {
    sounds_.insert(AudioBufferPair(path, audio_buffer(path)));
    return sounds_.at(path);
  } else {
    return sounds_.at(path);
  }
}

std::shared_ptr<Material> Assets::material(const std::string &path) const {
  glm::vec3 ambient;
  glm::vec3 diffuse(1.0f, 1.0f, 0.0f);
  glm::vec3 specular;
  float opacity, specular_exponent;

  if (path.empty()) {
    return std::make_shared<Material>(ambient, diffuse, specular, opacity,
                                      specular_exponent);
  }

  if (path.substr(path.find_last_of(".") + 1) == "material") {
    std::ifstream is(directory_ + path, std::ios::binary);

    is.read((char *)&ambient, sizeof(glm::vec3));
    is.read((char *)&diffuse, sizeof(glm::vec3));
    is.read((char *)&specular, sizeof(glm::vec3));
    is.read((char *)&opacity, sizeof(float));
    is.read((char *)&specular_exponent, sizeof(float));
  } else {
    throw std::runtime_error(path.substr(path.find_last_of(".")) +
                             " file format is not supported.");
  }
  return std::make_shared<Material>(ambient, diffuse, specular, opacity,
                                    specular_exponent);
}

std::shared_ptr<Material> Assets::material_cached(const std::string &path) {
  if (materials_.find(path) == materials_.end()) {

    materials_.insert(MaterialPair(path, material(path)));
    return materials_.at(path);
  } else {
    return materials_.at(path);
  }
}

void Assets::clear_unused() {
  for (auto it = textures_.begin(); it != textures_.end();) {
    if (it->second.use_count() <= 1) {
      textures_.erase(it++);
    } else {
      ++it;
    }
  }
  for (auto it = meshes_.begin(); it != meshes_.end();) {
    if (it->second.use_count() <= 1) {
      meshes_.erase(it++);
    } else {
      ++it;
    }
  }
  for (auto it = sounds_.begin(); it != sounds_.end();) {
    if (it->second.use_count() <= 1) {
      sounds_.erase(it++);
    } else {
      ++it;
    }
  }
  for (auto it = materials_.begin(); it != materials_.end();) {
    if (it->second.use_count() <= 1) {
      materials_.erase(it++);
    } else {
      ++it;
    }
  }
}
}
