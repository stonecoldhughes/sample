#pragma once
#include "data.hpp"
#include <cassert>
#include <array>
#include <iostream>
#include <cmath>

/* Contains class definitions that define operation on data types */
namespace operation
{

template< int S >
class round_robin_wheel
{
public:

  round_robin_wheel( int const start_index = 0 ) : size( S ), current_index( start_index ) {}

  /* the next index returned will be "new_index" */
  void set_index( int const new_index )
  { 
    assert( new_index < size && new_index >= 0 );
    current_index = new_index;
    return; 
  }

  int spin()
  {
    int const n = current_index++;

    if (current_index == size)
      current_index = 0;

    return n;
  }

private:
  int const size;
  int current_index;
};

/*
X = number of wheels
Y = each wheel's capacity
*/
template< int X, int Y >
class round_robin_nd
{
public:

  round_robin_nd( std::array< int, X > const indices = std::array< int, X >{} );

  void set_indices( std::array< int, X > const &new_indices );

  std::array< int, X > get_indices() { return indices; }

  void spin();

  void back_spin();

private:

  std::array< int, X > indices;

  std::array< round_robin_wheel< Y >, X > wheels;
};

template< int X, int Y >
round_robin_nd< X, Y >::round_robin_nd( std::array< int, X > const indices )
{
  assert( X > 0 );

  set_indices( indices );

  return;
}

template< int X, int Y >
void
round_robin_nd< X, Y >::spin()
{
  for( int i = 0; i < indices.size(); ++i )
  {
    int index = wheels[ i ].spin();
    indices[ i ] = index;
    if( index != 0 ) return;
  }

  return;
}

template< int X, int Y >
void
round_robin_nd< X, Y >::back_spin()
{
  for( int i = indices.size() - 1; i >= 0; --i )
  {
    int index = wheels[ i ].spin();
    indices[ i ] = index;
    if( index != 0 ) return;
  }

  return;
}

template< int X, int Y >
void round_robin_nd< X, Y >::set_indices( std::array< int, X > const &new_indices )
{
  indices = new_indices;

  for( int i = 0; i < indices.size(); ++i )
  {
    wheels[ i ].set_index( indices[ i ] );
    wheels[ i ].spin();
  }

  return;
}

/*
T = underlying data type
X = number of matrixes
Y = columns
Z = rows
*/
/* rename to uniform kron? the matrices are all of uniform dimensions */
template< typename T, int X, int Y, int Z >
class kron_operation
{

public:

  /* needs to assert on proper sizes */
  kron_operation( data::tensor< T, X, Y, Z > const &t,
                  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in, 
                  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out );

  void compute_kron();

  void compute_kron_1d( std::array< data::vector_1d< T, Z >, X > const &kron_args,
                        T scale_factor );

private:

  /* Since everything is a const reference, the data class needs no move/copy constructor */
  data::tensor< T, X, Y, Z > const &tensor;
  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in;
  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out;

};

template< typename T, int X, int Y, int Z >
kron_operation< T, X, Y, Z >::kron_operation( data::tensor< T, X, Y, Z > const &t,
                  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in,
                  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out )
  :
  tensor( t ),
  v_in( v_in ),
  v_out( v_out )
{
  /* assert that nothing is zero */
  assert( X > 0 );
  assert( Y > 0 );
  assert( Z > 0 );

  /* assert v_in and v_out are the correct size */
  int const input_size = std::pow( Y, X );
  assert( v_in.size() == input_size );

  int const output_size = std::pow( Z, X );
  assert( v_out.size() == output_size );

  return;
}

template< typename T, int X, int Y, int Z >
void kron_operation< T, X, Y, Z >::compute_kron()
{
  round_robin_nd< X, Y > column_selector;

  for( int i = 0; i < v_in.size(); ++i )
  {
    std::array< int, X > indices = column_selector.get_indices();

    std::array< data::vector_1d< T, Z >, X > kron_args;

    std::cout << "v_in[ " << i << " ] = " << v_in[ i ] << std::endl;

    for( int ii = 0; ii < indices.size(); ++ii )
    {
      data::vector_1d< T, Z > c = tensor.get_vector( ii, indices[ ii ] );
      kron_args[ ii ] = c;
    }

    /* now process the kron arguments */
    compute_kron_1d( kron_args, v_in[ i ] );

    column_selector.back_spin();
  }

  return;
}

template< typename T, int X, int Y, int Z >
void
kron_operation< T, X, Y, Z >::
compute_kron_1d( std::array< data::vector_1d< T, Z >, X > const &kron_args, T scale_factor )
{
  round_robin_nd< X, Z > row_selector;

  std::cout << "v_out.size(): " << v_out.size() << std::endl;
  for( int i = 0; i < v_out.size(); ++i )
  {
    auto indices = row_selector.get_indices();

    std::cout << "i = " << i <<std::endl;
    std::cout << "indices.size(): " << indices.size() << std::endl;

    T v_i = scale_factor;

    for( int ii = 0; ii < indices.size(); ++ii )
    {
      std::cout << "ii = " << ii << std::endl;
      std::cout << "kron_args.size() = " << kron_args.size() << std::endl; 
      v_i *= kron_args[ ii ][ indices[ ii ] ];
    }

    v_out[ i ] += v_i;

    row_selector.back_spin();
  }

  return;
}

}

































