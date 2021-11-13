#include <catch2/catch.hpp>
#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

mos::gfx::Camera camera{glm::vec3(0.0F),
                        glm::vec3(0.0F, 1.0F, 0.0F),
                        glm::perspective(glm::half_pi<float>(), 1.0F, 0.1F, 10.0F)};

TEST_CASE( "Inside camera frustum front", "[Camera]" ) {

  glm::vec3 point{0.0F, 1.0F, 0.0F};
  float radius = 1.0F;

  REQUIRE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Inside camera frustum right", "[Camera]" ) {

  glm::vec3 point{1.0F, 0.0F, 0.0F};
  float radius = 1.0F;

  REQUIRE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Outside camera frustum behind", "[Camera]" ) {

  glm::vec3 point{0.0F, -2.0F, 0.0F};
  float radius = 1.0F;

  REQUIRE_FALSE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Outside camera frustum right", "[Camera]" ) {

  glm::vec3 point{2.0F, 0.0F, 0.0F};
  float radius = 1.0F;

  REQUIRE_FALSE( camera.in_frustum(point, radius) );
}
