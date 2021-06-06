#include <fstream>
#include <array>
#include <map>
#include <algorithm>
#include <mos/gfx/mesh.hpp>
#include <mos/util.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/io.hpp>

namespace mos::gfx {

Mesh::Mesh(const std::initializer_list<Vertex> &vertices,
           const std::initializer_list<Triangle> &triangles)
    : Mesh(vertices.begin(), vertices.end(), triangles.begin(), triangles.end()) {
  calculate_sphere();
}

Mesh::Mesh() {
  calculate_sphere();
}

auto Mesh::load(const std::string &path) -> Mesh {
  if (path.substr(path.find_last_of('.') + 1) == "mesh") {

    Mesh mesh;

    std::ifstream is(path, std::ios::binary);
    if (!is.good()) {
      throw std::runtime_error(path + " does not exist.");
    }
    int num_vertices{0};
    int num_indices{0};
    is.read(reinterpret_cast<char *>(&num_vertices), sizeof(int));
    is.read(reinterpret_cast<char *>(&num_indices), sizeof(int));

    auto input_vertices = std::vector<Vertex>(num_vertices);
    auto input_indices = std::vector<int>(num_indices);

    if (!input_vertices.empty()) {
      is.read(reinterpret_cast<char *>(&input_vertices[0]), std::streamsize(input_vertices.size() * sizeof(Vertex)));
    }

    if (!input_indices.empty()) {
      is.read(reinterpret_cast<char *>(&input_indices[0]), std::streamsize(input_indices.size() * sizeof(int)));
    }
    mesh.vertices.assign(input_vertices.begin(), input_vertices.end());
    for (size_t i = 0; i < input_indices.size(); i += 3) {
      mesh.triangles.push_back(std::array<int, 3>{input_indices[i], input_indices[i+1], input_indices[i+2]});
    }
    mesh.calculate_tangents();
    mesh.calculate_sphere();
    return mesh;
  } else {
    throw std::runtime_error("File extension not supported.");
  }
}

void Mesh::clear() {
  vertices.clear();
  triangles.clear();
}

auto Mesh::positions() const -> Positions {
  Positions pos;
  std::transform(vertices.begin(), vertices.end(), std::back_inserter(pos), [](const Vertex &vertex) {
    return vertex.position;
  });
  return pos;
}

auto Mesh::mix(const Mesh &mesh1, const Mesh &mesh2, const float amount) -> void {
  auto it = vertices.begin();
  auto it1 = mesh1.vertices.begin();
  auto it2 = mesh2.vertices.begin();

  while (it != vertices.end()) {
    it->position = glm::mix(it1->position, it2->position, amount);
    it->normal = glm::mix(it1->normal, it2->normal, amount);
    it->uv = glm::mix(it1->uv, it2->uv, amount);

    it++;
    it1++;
    it2++;
  }
}

auto Mesh::apply_transform(const glm::mat4 &transform) -> void {
  for (auto &vertex : vertices) {
    vertex.apply_transform(transform);
  }
}

auto Mesh::calculate_tangents(Vertex &v0,
                              Vertex &v1,
                              Vertex &v2) -> void {
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


auto Mesh::calculate_normals() -> void {
  if (triangles.size() == 0) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
      //TODO: Generalize
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  } else {
    //TODO: Slow brute force, improve?
    using P = std::pair<int, std::vector<Face>>;
    std::map<int, std::vector<Face>> triangle_map;
    for (size_t i = 0; i < triangles.size(); i++) {
      std::array<int, 3> tri{triangles[i][0], triangles[i][1], triangles[i][2]};
      Face t{vertices[tri[0]], vertices[tri[1]], vertices[tri[2]]};
      for (auto i0 : tri) {
        if (triangle_map.find(i0) == triangle_map.end()) {
          triangle_map.insert(P(i0, std::vector<Face>{t}));
        } else {
          triangle_map[i0].push_back(t);
        }
      }
    }
    for (const auto & p : triangle_map) {
      auto &v = vertices[p.first];
      auto normal = glm::vec3(0.0f);
      for (auto &neighbour : p.second) {
        normal += neighbour.normal();
      }
      v.normal = glm::normalize(normal);
    }
  }
}

auto Mesh::calculate_tangents() -> void {
  if (triangles.size() == 0) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
      //TODO: Generalize
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      calculate_tangents(v0, v1, v2);
    }
  } else {
    for (size_t i = 0; i < triangles.size(); i++) {
      auto &v0 = vertices[triangles[i][0]];
      auto &v1 = vertices[triangles[i][1]];
      auto &v2 = vertices[triangles[i][2]];

      calculate_tangents(v0, v1, v2);
    }
  }
}

auto Mesh::calculate_flat_normals() -> void {
  if (triangles.size() == 0) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
      auto &v0 = vertices[i];
      auto &v1 = vertices[i + 1];
      auto &v2 = vertices[i + 2];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  } else {
    for (size_t i = 0; i < triangles.size(); i++) {
      auto &v0 = vertices[triangles[i][0]];
      auto &v1 = vertices[triangles[i][1]];
      auto &v2 = vertices[triangles[i][2]];

      auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
      v0.normal = normal;
      v1.normal = normal;
      v2.normal = normal;
    }
  }
}

auto Mesh::calculate_sphere() -> void {
  const auto & all_positions = positions();

  centroid = std::accumulate(all_positions.begin(), all_positions.end(), glm::vec3(0.0));
  centroid.x /= all_positions.size();
  centroid.y /= all_positions.size();
  centroid.z /= all_positions.size();

  radius = 0.0f;

  for (auto & p : all_positions) {
    auto d = glm::distance(centroid, p);
    if (d > radius) {
      radius = d;
    }
  }

}
auto Mesh::Face::normal() const -> glm::vec3 {
  return glm::triangleNormal(v0.position, v1.position, v2.position);
}
}
