#include <mos/assets.hpp>

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <lodepng.h>
#include <exception>
#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stb_vorbis.h>
#include <glm/gtx/io.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mos/util.hpp>
#include <algorithm>

namespace mos {
using namespace std;
using namespace glm;

Assets::Assets(const std::string directory) : directory_(directory) {}

Assets::~Assets() {}

Model Assets::model(rapidjson::Value &value) {
  auto name = !value.HasMember("name") || value["name"].IsNull()
                  ? ""
                  : value["name"].GetString();

  std::string mesh = !value.HasMember("mesh") || value["mesh"].IsNull()
                         ? ""
                         : value["mesh"].GetString();

  std::string texture_name =
      !value.HasMember("texture") || value["texture"].IsNull()
          ? ""
          : value["texture"].GetString();
  std::string texture2_name =
      !value.HasMember("texture2") || value["texture2"].IsNull()
          ? ""
          : value["texture2"].GetString();
  std::string lightmap_name =
      !value.HasMember("lightmap") || value["lightmap"].IsNull()
          ? ""
          : value["lightmap"].GetString();
  std::string material_name =
      !value.HasMember("material") || value["material"].IsNull()
          ? ""
          : value["material"].GetString();
  bool recieves_light = !value.HasMember("receives_light") || value["receives_light"].IsNull()
      ? true
      : value["receives_light"].GetBool();

  glm::mat4 transform;

  if (value.HasMember("transform")) {
    std::vector<float> nums;
    for (auto it = value["transform"].Begin(); it != value["transform"].End();
         it++) {
      nums.push_back(it->GetDouble());
    }
    transform = glm::make_mat4x4(nums.data());
  }

  auto created_model = mos::Model(
      name, mesh_cached(mesh), texture_cached(texture_name),
      texture_cached(texture2_name), transform, mos::Model::Draw::TRIANGLES,
      material_cached(material_name), Lightmaps(texture_cached(lightmap_name)), nullptr,
      recieves_light);

  for (auto it = value["models"].Begin(); it != value["models"].End(); it++) {
    created_model.models.push_back(model(*it));
  }

  return created_model;
}

Model Assets::model(const std::string &path) {
  auto doc = document(directory_ + path);
  return model(doc);
}

Animation Assets::animation(const string &path) {
  auto doc = document(directory_ + path);
  auto frame_rate = doc["frame_rate"].GetInt();
  std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes;

  for (auto it = doc["keyframes"].Begin(); it != doc["keyframes"].End(); it++) {
    auto key = (*it)["key"].GetInt();
    auto mesh_path = (*it)["mesh"].GetString();
    keyframes.insert({key, mesh_cached(mesh_path)});
  }
  Animation animation(keyframes, frame_rate);
  return animation;
}

std::shared_ptr<Mesh> Assets::mesh(const std::string &path) const {
  return std::make_shared<Mesh>(Mesh(directory_ + path));
}

std::shared_ptr<Mesh> Assets::mesh_cached(const std::string &path) {
  if (meshes_.find(path) == meshes_.end()) {
    meshes_.insert(MeshPair(path, mesh(path)));
  }
  return meshes_.at(path);
}

std::shared_ptr<Texture2D> Assets::texture(const std::string &path,
                                           const bool mipmaps) const {
  if (path.empty()) {
    return std::shared_ptr<Texture2D>(nullptr);
  }
  return std::make_shared<Texture2D>(directory_ + path, mipmaps);
}

std::shared_ptr<Texture2D> Assets::texture_cached(const std::string &path,
                                                  const bool mipmaps) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(TexturePair(path, texture(path, mipmaps)));
    }
    return textures_.at(path);
  } else {
    return std::shared_ptr<Texture2D>(nullptr);
  }
}

std::shared_ptr<Sound> Assets::sound(const std::string &path) const {
  return std::make_shared<Sound>(Sound(directory_ + path));
}

std::shared_ptr<Stream> Assets::stream(const string &path) const {
  return std::make_shared<mos::Stream>(directory_ + path);
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
  std::string file =
      texture_node->first_attribute("file")->value();
  //std::transform(file.begin(), file.end(), file.begin(), ::tolower);

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

std::shared_ptr<Sound> Assets::sound_cached(const std::string &path) {
  if (sounds_.find(path) == sounds_.end()) {

    sounds_.insert(SoundPair(path, sound(path)));
    return sounds_.at(path);
  } else {
    return sounds_.at(path);
  }
}

std::shared_ptr<Material> Assets::material(const std::string &path) const {
  return std::make_shared<Material>(directory_ + path);
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
