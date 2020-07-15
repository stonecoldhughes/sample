#include "operation.hpp"
#include "tests_general.hpp"

#include <cmath>

/* illustrates modulo repetition */
TEMPLATE_TEST_CASE( "name", "string", double )
{
  SECTION( "another test" )
  {
    /* contains 3 round robin wheels of capacity 5 each */
    int const num_wheels = 3;
    int const wheel_size = 5;
    std::array< int, num_wheels > gold = { 0, 2, 3 };
    std::array< int, num_wheels > indices;

    operation::round_robin_nd< num_wheels, wheel_size > rr2( gold );

    int iterations = std::pow( wheel_size, num_wheels );

    for( int i = 0; i < iterations; ++i ) rr2.spin();

    REQUIRE( gold == rr2.get_indices() );
  }
}
