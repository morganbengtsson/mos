#include <fstream>
#include <mos/render/mesh.hpp>
#include <mos/util.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/io.hpp>
#include <array>
#include <map>
#include <algorithm>
#include <glm/gtx/io.hpp>

namespace mos {

std::atomic_uint Mesh::current_id;

Mesh::Mesh(std::initializer_list<Vertex> vertices,
           std::initializer_list<int> elements)
  : Mesh(vertices.begin(), vertices.end(), elements.begin(), elements.end()) {
}

Mesh::Mesh(const std::string &path) : id_(current_id++){
  if (path.substr(path.find_last_of(".") + 1) == "mesh") {
    std::ifstream is(path, std::ios::binary);
    if (!is.good()) {
      throw std::runtime_error(path + " does not exist.");
    }
    int num_vertices;
    int num_indices;
    is.read((char *)&num_vertices, sizeof(int));
    is.read((char *)&num_indices, sizeof(int));

    auto input_vertices = std::vector<mos::Vertex>(num_vertices);
    auto input_indices = std::vector<int>(num_indices);

    if (input_vertices.size() > 0) {
      is.read((char *)&input_vertices[0], input_vertices.size() * sizeof(Vertex));
    }

    if (input_indices.size() > 0) {
      is.read((char *)&input_indices[0], input_indices.size() * sizeof(int));
    }
    vertices.assign(input_vertices.begin(), input_vertices.end());
    indices.assign(input_indices.begin(), input_indices.end());

    calculate_tangents();
  } else {
    throw std::runtime_error("File extension not supported.");
  }
}

Mesh::Mesh() : valid_(false), id_(current_id++) {}

Mesh::Mesh(const Mesh &mesh)
    : Mesh(mesh.vertices_begin(), mesh.vertices_end(), mesh.elements_begin(),
           mesh.elements_end()) {}

Mesh::~Mesh() {}

SharedMesh Mesh::load(const std::string &path) {
  if (path.empty() || (path.back() == '/')) {
    return std::make_shared<Mesh>(Mesh());
  } else {
    return std::make_shared<Mesh>(path);
  }
}

Mesh::Vertices::iterator Mesh::begin() {
  return vertices.begin();
}

Mesh::Vertices::iterator Mesh::end() {
  return vertices.end();
}

Mesh::Vertices::const_iterator Mesh::vertices_begin() const {
  return vertices.begin();
}

Mesh::Vertices::const_iterator Mesh::vertices_end() const {
  return vertices.end();
}

Mesh::Indices::const_iterator Mesh::elements_begin() const {
  return indices.begin();
}

Mesh::Indices::const_iterator Mesh::elements_end() const {
  return indices.end();
}

Mesh::Vertices::iterator Mesh::vertices_begin() { return vertices.begin(); }

Mesh::Vertices::iterator Mesh::vertices_end() { return vertices.end(); }

Mesh::Indices::iterator Mesh::elements_begin() { return indices.begin(); }

Mesh::Indices::iterator Mesh::elements_end() { return indices.end(); }

unsigned int Mesh::id() const { return id_; }

bool Mesh::valid() const { return valid_; }

void Mesh::invalidate() { valid_ = false; }

void Mesh::clear() {
  vertices.clear();
  indices.clear();
}

void Mesh::add(const Vertex& vertex) {
  vertices.push_back(vertex);
  valid_ = false;
}

void Mesh::add(const int element) {
  indices.push_back(element);
  valid_ = false;
}

Mesh::Positions Mesh::positions() const {
  Positions pos;
  std::transform(vertices.begin(), vertices.end(), std::back_inserter(pos), [](const Vertex &vertex){
    return vertex.position;
  });
  return pos;
}

void Mesh::mix(const Mesh &mesh1, const Mesh &mesh2, const float amount) {
  auto it = vertices_begin();
  auto it1 = mesh1.vertices_begin();
  auto it2 = mesh2.vertices_begin();

  while (it != vertices_end()) {
    it->position = glm::mix(it1->position, it2->position, amount);
    it->normal = glm::mix(it1->normal, it2->normal, amount);
    it->uv = glm::mix(it1->uv, it2->uv, amount);
    it->uv_lightmap = glm::mix(it1->uv_lightmap, it2->uv_lightmap, amount);

    it++;
    it1++;
    it2++;
  }
  invalidate();
}

void Mesh::apply_transform(const glm::mat4 &transform) {
  for (auto &vertex : vertices) {
    vertex.position = glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
  }
}

void Mesh::calculate_tangents(mos::Vertex &v0,
                              mos::Vertex &v1,
                              mos::Vertex &v2) {
  auto pos1 = v0.position;
  auto pos2 = v1.position;
  auto pos3 = v2.position;

  auto uv1 = v0.uv;
  auto uv2 = v1.uv;
  auto uv3 = v2.uv;

  glm::vec3 edge1 = pos2 - pos1;
  glm::vec3 edge2 = pos3 - pos1;
  glm::vec2 delta_uv1 = uv2 - uv1;
  glm::vec2 delta_uv2 = uv3 - uv1;

  float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);

  glm::vec3 tangent;
  tangent.x = f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
  tangent.y = f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
  tangent.z = f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);
  tangent = glm::normalize(tangent);

  v0.tangent = tangent;
  v1.tangent = tangent;
  v2.tangent = tangent;
}

struct Triangle{
  Vertex &v0;
  Vertex &v1;
  Vertex &v2;
  glm::vec3 normal() const {
    return glm::triangleNormal(v0.position, v1.position, v2.position);
  }
};

void Mesh::calculate_normals() {
  if (indices.size() == 0) {
    for (int i = 0; i < vertices.size(); i += 3) {
      //TODO: Generalize
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  }
  else {
    //TODO: Slow brute force, improve?
    using P = std::pair<int, std::vector<Triangle>>;
    std::map<int, std::vector<Triangle>> triangle_map;
    for (int i = 0; i < indices.size(); i += 3) {
      std::array<int, 3> tri{indices[i], indices[i + 1], indices[i + 2]};
      Triangle t{vertices[tri[0]], vertices[tri[1]], vertices[tri[2]]};
      for (auto i0 : tri) {
        if (triangle_map.find(i0) == triangle_map.end()) {
          triangle_map.insert(P(i0, std::vector<Triangle>{t}));
        } else {
          triangle_map[i0].push_back(t);
        }
      }
    }
    for (auto p : triangle_map){
      auto & v = vertices[p.first];
      glm::vec3 normal = glm::vec3(0.0f);
      for (auto & neighbour : p.second){
        normal += neighbour.normal();
      }
      v.normal = glm::normalize(normal);
    }

    /*
    for (std::vector<int>::const_iterator i = indices.begin(); i != indices.end(); std::advance(i, 3))
    {
      glm::vec3 v[3] = { vertices[*i].position, vertices[*(i+1)].position, vertices[*(i+2)].position };
      glm::vec3 normal = glm::cross(v[1] - v[0], v[2] - v[0]);

      for (int j = 0; j < 3; ++j)
      {
        glm::vec3 a = v[(j+1) % 3] - v[j];
        glm::vec3 b = v[(j+2) % 3] - v[j];
        float weight =  glm::acos(glm::dot(a, b) / (a.length() * b.length()));
        vertices[*(i+j)].normal += weight * normal;
      }
    }
    for (auto & vertex : vertices){
      vertex.normal = glm::normalize(vertex.normal);
    }*/
  }
}

void Mesh::calculate_tangents() {
  if (indices.size() == 0) {
    for (int i = 0; i < vertices.size(); i += 3) {
      //TODO: Generalize
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      calculate_tangents(v0, v1, v2);
    }
  }
  else{
    for (int i = 0; i < indices.size(); i += 3) {
      auto &v0 = vertices[indices[i]];
      auto &v1 = vertices[indices[i + 1]];
      auto &v2 = vertices[indices[i + 2]];

      calculate_tangents(v0, v1, v2);
    }
  }
}
void Mesh::calculate_flat_normals() {
  if (indices.size() == 0) {
    for (int i = 0; i < vertices.size(); i += 3) {
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  }
  else {
    for (int i = 0; i < indices.size(); i += 3) {
      auto &v0 = vertices[indices[i]];
      auto &v1 = vertices[indices[i + 1]];
      auto &v2 = vertices[indices[i + 2]];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  }
}
}
