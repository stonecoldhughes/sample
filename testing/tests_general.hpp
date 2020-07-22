#pragma once
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "src/data.hpp"

namespace testing
{

/* floating point values cannot ever be expected to be exactly equal: equality within a tolerance
   is implemented by this function */
template< typename T, int N >
void rmse_comparison( std::array< T, N > const &output, std::array< T, N > const &correct,
                      T tol_factor = 0.001 )
{ 
  for( int i = 0; i < static_cast<int>(output.size()); ++i )
  {
    REQUIRE( std::abs( output[ i ] - correct[ i ] ) < tol_factor );
  }

  return;
}

} // end namespace testing
