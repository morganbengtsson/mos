#include <fstream>
#include <mos/render/mesh.hpp>
#include <mos/util.hpp>
#include <algorithm>

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

    auto vertices = std::vector<mos::Vertex>(num_vertices);
    auto elements = std::vector<int>(num_indices);

    if (vertices.size() > 0) {
      is.read((char *)&vertices[0], vertices.size() * sizeof(Vertex));
    }

    if (elements.size() > 0) {
      is.read((char *)&elements[0], elements.size() * sizeof(int));
    }
    vertices_.assign(vertices.begin(), vertices.end());
    elements_.assign(elements.begin(), elements.end());


    if (elements_.size() == 0) {
      for (int i = 0; i < vertices_.size(); i += 3) {
        //TODO: Generalize
        auto &v0 = vertices_[i];
        auto &v1 = vertices_[i + 1];
        auto &v2 = vertices_[i + 2];
        calculate_tangents(v0, v1, v2);
      }
    }
    else{
      for (int i = 0; i < elements_.size(); i += 3) {
        auto &v0 = vertices_[elements_[i]];
        auto &v1 = vertices_[elements_[i + 1]];
        auto &v2 = vertices_[elements_[i + 2]];

        calculate_tangents(v0, v1, v2);
      }
    }

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
  return vertices_.begin();
}

Mesh::Vertices::iterator Mesh::end() {
  return vertices_.end();
}

Mesh::Vertices::const_iterator Mesh::vertices_begin() const {
  return vertices_.begin();
}

Mesh::Vertices::const_iterator Mesh::vertices_end() const {
  return vertices_.end();
}

Mesh::Elements::const_iterator Mesh::elements_begin() const {
  return elements_.begin();
}

Mesh::Elements::const_iterator Mesh::elements_end() const {
  return elements_.end();
}

Mesh::Vertices::iterator Mesh::vertices_begin() { return vertices_.begin(); }

Mesh::Vertices::iterator Mesh::vertices_end() { return vertices_.end(); }

Mesh::Elements::iterator Mesh::elements_begin() { return elements_.begin(); }

Mesh::Elements::iterator Mesh::elements_end() { return elements_.end(); }

unsigned int Mesh::id() const { return id_; }

bool Mesh::valid() const { return valid_; }

void Mesh::invalidate() { valid_ = false; }

void Mesh::clear() {
  vertices_.clear();
  elements_.clear();
}

void Mesh::add(const Vertex& vertex) {
  vertices_.push_back(vertex);
  valid_ = false;
}

void Mesh::add(const int element) {
  elements_.push_back(element);
  valid_ = false;
}

const Mesh::Vertices &Mesh::vertices() { return vertices_; }

const Mesh::Elements &Mesh::elements() { return elements_; }

const Vertex *Mesh::vertices_data() const { return vertices_.data(); }

Mesh::Vertices::size_type Mesh::vertices_size() const {
  return vertices_.size();
}

const int *Mesh::elements_data() const { return elements_.data(); }

Mesh::Vertices::size_type Mesh::elements_size() const {
  return elements_.size();
}

Mesh::Positions Mesh::positions() const {
  Positions pos;
  std::transform(vertices_.begin(), vertices_.end(), std::back_inserter(pos), [](const Vertex &vertex){
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
  for (auto &vertex : vertices_) {
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
}
