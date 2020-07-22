/*

This component defines objects that carry out a specific math operation using the flexible
primitives defined in the file "data.hpp".

round_robin_wheel:

  A finite-state machine that implements a circular counter. 
  Each state is represented as a number.

round_robin_nd:

  A more complex finite-state machine. 
  Each state is represented as a tuple of numbers.

kron_operation:

  An object that carries out this specific linear algebra routine:

  ( M_0 kron M_1 kron ... kron M_n ) * v_in = v_out

*/
#pragma once
#include "data.hpp"
#include <cassert>
#include <array>
#include <iostream>
#include <cmath>

namespace operation
{

/*
  Simple fsm. The "spin()" routine increments this object to the next state
  S = number of states
*/
template< int S >
class round_robin_wheel
{
public:

  round_robin_wheel( int const start_index = 0 ) : size( S ), current_index( start_index ) {}

/* The next 2 functions could easily have been implemented with modular arithmetic - however,
   this is a particularly inefficient operation. The same result is accomplished with
   conditionals. For large value of "size", branch prediction ensures efficient execution */
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
  /* FSM current state */
  int current_index;
};

/*

This FSM is an ordered tuple of round_robin_wheel FSMs.

X = Number of round_robin_wheel FSMs that generate the state-tuple for this FSM
Y = value of the final state for each round_robin_wheel

Finite states are represented as tuples:  
{ ... , t_(i-1), t_i, t_(i+1), ... }. Where the value of each t_i is determined by the state of
an object of type "round_robin_wheel" described above. t_i progresses to its next state when
t_(i+1) completes a full cycle; when t_i completes a full cycle, t_( i - 1 ) progress one state
further.

*/
template< int X, int Y >
class round_robin_nd
{
public:

  /* default initial state { t_i = 0 for all i } */
  round_robin_nd( std::array< int, X > const indices = std::array< int, X >{} );

  /* arbitrarily change state */
  void set_indices( std::array< int, X > const &new_indices );

  /* obtain current state */
  std::array< int, X > get_indices() { return indices; }

  /* progress the FSM to the next state */
  void spin();

  /* return the FSM to the previous state */
  void back_spin();

private:

  /* current FSM state */
  std::array< int, X > indices;

  /* generates next or previous FSM state */
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
  for( int i = 0; i < static_cast<int>( indices.size() ); ++i )
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

  for( int i = 0; i < static_cast<int>(indices.size()); ++i )
  {
    wheels[ i ].set_index( indices[ i ] );
    wheels[ i ].spin();
  }

  return;
}

/*

kron_operation carries out this specific linear algebra routine:

( M_0 kron M_1 kron ... kron M_n ) * v_in = v_out

Where the "M_i" are "X" number of column-major matrices of "Y" columns and "Z" rows residing
adjacent in memory and encoded in a third-order data::tensor object t< X, Y, Z >.

"kron" indicates the Kronecker Product operation: 
            https://en.wikipedia.org/wiki/Kronecker_product
"v_in" is a 1D vector,
"v_out" is a 1D vector,
"*" indicates matrix-vector multiplication

The sequential steps needed to carry out the routine defined above can be inferred by iterating
through a full cycle of the finite states defined by an object of type round_robin_nd
parameterized properly.

T = underlying data type
X = number of matrices. Each matrix has equal dimensions.
    Values are expected in column-major order. Matrices are adjacent in memory.
Y = number of columns per matrix
Z = number of rows per matrix

*/
template< typename T, int X, int Y, int Z >
class kron_operation
{

public:

  /* initializes all process parameters. Output is stored in v_out */
  kron_operation( data::tensor< T, X, Y, Z > const &t,
                  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in, 
                  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out );

  /* carries out the operation by breaking down the problem into a sequence of Kronecker
     products with 1D operands */
  void compute_kron();

private:

  /* Carries out a scaled, 1D Kronecker product */
  void compute_kron_1d( std::array< data::vector_1d< T, Z >, X > const &kron_args,
                        T scale_factor );

  /* operands involved in the computation */
  data::tensor< T, X, Y, Z > const &tensor;
  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in;
  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out;

};

/* constructor */
template< typename T, int X, int Y, int Z >
kron_operation< T, X, Y, Z >::kron_operation( data::tensor< T, X, Y, Z > const &t,
                  data::vector_1d< T, int( std::pow( Y, X ) ) > const &v_in,
                  data::vector_1d< T, int( std::pow( Z, X ) ) > &v_out )
  :
  tensor( t ),
  v_in( v_in ),
  v_out( v_out )
{
  /* all dimensions must be positive and non-zero */
  assert( X > 0 );
  assert( Y > 0 );
  assert( Z > 0 );

  /* ensure dimensions match for valid matrix operations */
  int const input_size = std::pow( Y, X );
  assert( v_in.size() == input_size );

  int const output_size = std::pow( Z, X );
  assert( v_out.size() == output_size );

  return;
}

/*

The final output vector of the desired computation is a linear combination:

summation{ c_i * v_i } for i = { 0, 1, ... , Y^X }

where each "v_i" is the result of the Kronecker product of "X" 1D vectors of length Z:

v_i = ( v_i_0 kron v_i_1 kron ... kron v_i_X ), length = Z^X

where the { v_i_j } for j = { 0, 1, ... , X } are selected via a round_robin_nd FSM

the "c_i" are simply the elements of the "v_in" 1D input vector

*/
/*

Performance note:

This method requires far too many levels of indirection per floating point operation to
be performant. Each flop requires multiple memory accesses to arrive at the final operands.

Furthermore, this method makes no use of fused multiply-add units or highly optimized matrix
algebra vendor libraries.

*/
template< typename T, int X, int Y, int Z >
void kron_operation< T, X, Y, Z >::compute_kron()
{
  /* each state of "column_selector" represents a set of "X" 1D vectors */
  round_robin_nd< X, Y > column_selector;

  for( int i = 0; i < v_in.size(); ++i )
  {
    std::array< int, X > indices = column_selector.get_indices();

    std::array< data::vector_1d< T, Z >, X > kron_args;

    for( int ii = 0; ii < static_cast<int>(indices.size()); ++ii )
    {
      data::vector_1d< T, Z > c = tensor.get_vector( ii, indices[ ii ] );
      kron_args[ ii ] = c;
    }

    /* the vectors collected for the finite state are now Kroneckered */
    compute_kron_1d( kron_args, v_in[ i ] );

    /* Specify the next set of vectors */
    column_selector.back_spin();
  }

  return;
}

/*

Given a list of 1D vectors in "kron_args", this routine computes:

scale_factor * kron( kron_args[ 0 ], kron_args[ 1 ], ... , kron_args[ X ] )

where each 1D vector is Z elements long

*/
template< typename T, int X, int Y, int Z >
void
kron_operation< T, X, Y, Z >::
compute_kron_1d( std::array< data::vector_1d< T, Z >, X > const &kron_args, T scale_factor )
{
  /* each state of "row_selector" represents a set of scalar factors obtained, one each, from
     the elements of "kron_args" */
  round_robin_nd< X, Z > row_selector;

  for( int i = 0; i < v_out.size(); ++i )
  {
    auto indices = row_selector.get_indices();

    T v_i = scale_factor;

    for( int ii = 0; ii < static_cast<int>(indices.size()); ++ii )
    {
      v_i *= kron_args[ ii ][ indices[ ii ] ];
    }

    v_out[ i ] += v_i;

    /* Specify the next set of factors */
    row_selector.back_spin();
  }

  return;
}

}
