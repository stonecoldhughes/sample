#include "data.hpp"
#include "tests_general.hpp"

TEMPLATE_TEST_CASE("empty 0", "empty 1", double, float )
{
  SECTION( "trivial test" )
  {
    data::vector_1d< TestType > v( 420 ); 

    trivial_truth();
  }

  /* Explain the basic idea behind how the test works */
  SECTION("5 dimensional tensor get_vector()")
  {
    int const dim_0 = 2;
    int const dim_1 = 3;
    int const dim_2 = 4;
    int const dim_3 = 5;
    int const dim_4 = 3;

    std::vector< TestType > numbers( dim_0 * dim_1 * dim_2 * dim_3 * dim_4 );

    /* Captain! Change this to rand()? */
    for( int i = 0; i < static_cast<int>(numbers.size()); ++i ) numbers[ i ] = i;

    data::tensor< TestType, dim_0, dim_1, dim_2, dim_3, dim_4 > t( numbers.data() );

    std::vector< TestType > v_gold;

    for( int i = 0; i < dim_0; ++i )
    {
      for( int ii = 0; ii < dim_1; ++ii )
      {
        for( int iii = 0; iii < dim_2; ++iii )
        {
          for( int iiii = 0; iiii < dim_3; ++iiii )
          {
            std::vector< TestType > v = t.get_vector( i, ii, iii, iiii );
            v_gold.insert( v_gold.end(), v.begin(), v.end() );
          }
        }
      }
    }

    /* Captain! Replace this with an rmse check? */
    if( v_gold == numbers ) std::cout << "Captain! Success" << std::endl;
    else std::cout << "Captain... abject failure" << std::endl;

    trivial_truth();
  }
}
