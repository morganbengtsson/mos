#include <catch2/catch.hpp>
#include <mos/gfx/mesh.hpp>
#include <glm/gtx/io.hpp>

TEST_CASE( "Flat normals", "[Mesh]" ) {
  using namespace mos::gfx;

  auto v0 = Vertex{glm::vec3(0.0F)};
  auto v1 = Vertex{glm::vec3(1.0F, 0.0F, 0.0F)};
  auto v2 = Vertex{glm::vec3(0.0F, 1.0F, 0.0F)};

  Mesh mesh({v0, v1, v2}, {});
  mesh.calculate_flat_normals();

  const auto expected_normal = glm::vec3(0.0F, 0.0F, 1.0F);

  REQUIRE(mesh.vertices[0].normal == expected_normal);
  REQUIRE(mesh.vertices[1].normal == expected_normal);
  REQUIRE(mesh.vertices[2].normal == expected_normal);
}


TEST_CASE( "Bounding sphere", "[Mesh]" ) {
  using namespace mos::gfx;

  constexpr auto width = 2.0F;
  constexpr auto half_width = width / 2.0F;

  auto v0 = Vertex{glm::vec3(0.0F)};
  auto v1 = Vertex{glm::vec3(width, 0.0F, 0.0F)};
  auto v2 = Vertex{glm::vec3(0.0F, width, 0.0F)};

  auto v3 = Vertex{glm::vec3(width, width, 0.0F)};


  Mesh mesh({v0, v1, v2, v1, v3, v2}, {});
  mesh.calculate_sphere();

  const auto expected_radius = glm::sqrt(glm::pow(half_width, 2.0F) + glm::pow(half_width, 2.0F));
  const auto expected_centroid = glm::vec3(half_width, half_width, 0.0F);

  REQUIRE(mesh.radius() == expected_radius);
  REQUIRE(mesh.centroid() == expected_centroid);
}
