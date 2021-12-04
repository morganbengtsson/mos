#include <catch2/catch.hpp>
#include <mos/util.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE( "Hex color conversion", "[Util]" ) {

  constexpr auto white_hex = 0xFFFFFF;
  constexpr auto black_hex = 0x000000;
  constexpr auto red_hex = 0xFF0000;

  const auto white = mos::hex_color(white_hex);
  const auto black = mos::hex_color(black_hex);
  const auto red = mos::hex_color(red_hex);

  REQUIRE(white == glm::vec3(1.0F));
  REQUIRE(black == glm::vec3(0.0F));
  REQUIRE(red == glm::vec3(1.0F, 0.0F, 0.0F));
}
