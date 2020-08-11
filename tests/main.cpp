#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

mos::gfx::Camera camera{glm::vec3(0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f),
                        glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f)};

TEST_CASE( "Inside camera frustum front", "[Camera]" ) {

  glm::vec3 point{0.0f, 1.0f, 0.0f};
  float radius = 1.0f;

  REQUIRE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Inside camera frustum right", "[Camera]" ) {

  glm::vec3 point{1.0f, 0.0f, 0.0f};
  float radius = 1.0f;

  REQUIRE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Outside camera frustum behind", "[Camera]" ) {

  glm::vec3 point{0.0f, -2.0f, 0.0f};
  float radius = 1.0f;

  REQUIRE_FALSE( camera.in_frustum(point, radius) );
}

TEST_CASE( "Outside camera frustum right", "[Camera]" ) {

  glm::vec3 point{2.0f, 0.0f, 0.0f};
  float radius = 1.0f;

  REQUIRE_FALSE( camera.in_frustum(point, radius) );
}
