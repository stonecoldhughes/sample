/*
Make a "what you should look at list - what each file shows proficiency in, instructions
how to see it in action, etc. Include notes about what could be improved in the future, like
using exceptions instead of asserts.
*/
/*
data component

Defines 2 types of custom data structures that will be used to hold the inputs and outputs of
the operations defined in the "operations" component

The "data" component in this code sample illustrates:

1. An understanding of some basic C++ template metaprogramming concepts.
2. Implementation and understanding of STL features like iterators
3. Using 1 and 2 to create something that is convenient to use

*/
/* Captain! Make a short note like: sample feature = function overloading */
#pragma once
#include <iostream>
#include <initializer_list>
#include <cassert>
#include <array>
#include <functional>
#include <iterator>
#include <vector>

/* Captain! Use size_t instead of "int" for dimension sizes */

/* namespace for input/output containers for this simple matrix library */
namespace data
{

/* Simple class to hold one dimensional data. This class does NOT manage any memory */
template< typename T >
class vector_1d
{
  public:

    /* initialize empty with length "len" */
    vector_1d( int const len, T *data = nullptr );

    T operator []( int const i ) const { assert( i < len && i >= 0 ); return data[ i ]; }
    T& operator []( int const i ) { assert( i < len && i >= 0 ); return data[ i ]; }

    /* initialize with values - length is the length of initializer list */
    /* Captain! Uncomment after you get the variadic template thing set up */
    //vector_1d( std::initializer_list< T > values );

    /* Captain! Check if vector.data() from this is the same memory address as T *data member */
    std::vector< T > get_std_vector(); 

    int const len;

  private:

    /* "len" T data-type elements */
    T *data;
};

template< typename T, int ... dimlist >
class tensor
{
  public:

    tensor( T *data );

    template< typename ... pack >
    std::vector< T > get_vector( pack ... ) const;

  private:

    template< typename ... indices_remaining >
    int get_vector_recurse( int dim, int index, indices_remaining ... ) const;

    /* base case routine - rename */
    template< typename ... indices_remaining >
    int get_vector_recurse( int dim, int index ) const;

    
    /* Captain! Add better public/private specifiers and label functions better */
    std::array< int, sizeof...( dimlist ) > const &get_dimensions() { return dims_array; };
    T *get_data_array() { return data; };
    std::array< int, sizeof...( dimlist ) - 1 > const &get_offsets() { return offset_factors; };

  private:

    std::array< int, sizeof...( dimlist ) > dims_array;
    T *data;
    std::array< int, sizeof...( dimlist ) - 1 > offset_factors;
};

/* Captain! Explain why you did a base case function instead of a programmatic base case*/
template< typename T, int ... dimlist > // Captain! Label each declaration line
template< typename ... indices_remaining >
int
tensor< T, dimlist ... >::
get_vector_recurse( int dim, int index ) const
{
  return index * offset_factors[ dim ];
}

template< typename T, int ... dimlist >
template< typename ... indices_remaining >
int
tensor< T, dimlist ... >::
get_vector_recurse( int dim, int index, indices_remaining ... indices ) const
{
  return index * offset_factors[ dim ] + get_vector_recurse( dim + 1, indices ... );
}

template< typename T, int ... dimlist >
template< typename ... pack >
std::vector< T >
tensor< T, dimlist ... >::
get_vector( pack ... p ) const
{
  assert( sizeof...( p ) == dims_array.size() - 1 );

  assert( ( std::is_same_v< int, pack > && ... ) );

  int offset = get_vector_recurse( 0, p ... );

  return std::vector< T >( data + offset, data + offset + dims_array.back() );
}

template< typename T, int ... dimlist >
tensor< T, dimlist ... >::tensor( T *data_arg )
  :
  dims_array{ dimlist ... },
  data( data_arg )
{
  int factor = 1;

  for( int i = dims_array.size() - 1; i > 0; --i )
  {
    factor *= dims_array[ i ];

    offset_factors[ i - 1 ] = factor;
  }

  return;
}

/* Captain! Experimental class for implementing a 1D operator */
template< typename T, int ... dimlist >
class operator_1d : tensor< T, dimlist ... >
{
  public:

    operator_1d( T *data );
    
};

template< typename T, int ... dimlist >
operator_1d< T, dimlist ... >::
operator_1d( T *data )
  :
  tensor< T, dimlist ... >( data )
{
  std::cout << "Ahoy, Captain!" << std::endl;
  return;
}

} // end namespace data



































