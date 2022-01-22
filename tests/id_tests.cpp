#include <catch2/catch.hpp>
#include <mos/core/id.hpp>

struct Type0 {};
struct Type1 {};

TEST_CASE( "IDs for multiple types", "[Id]" ) {
  mos::Id<Type0> id0;
  mos::Id<Type0> id1;

  mos::Id<Type1> id2;
  mos::Id<Type1> id3;

  REQUIRE( id0 == 1 );
  REQUIRE( id1 == 2 );

  REQUIRE( id2 == 1 );
  REQUIRE( id3 == 2 );
}
