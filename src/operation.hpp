#pragma once
#include <cassert>
#include <array>
#include <iostream>
/* Contains class definitions that define operation on data types */
namespace operation
{

/* Captain! Next thing: a class that takes in a tensor and iterates over the rows and columns
   properly */

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

template< int X, int Y >
class round_robin_nd
{
public:

  round_robin_nd( std::array< int, X > const &indices );

  void set_indices( std::array< int, X > const &new_indices );

  std::array< int, X > get_indices() { return indices; }

  void spin();

private:

  std::array< int, X > indices;

  std::array< round_robin_wheel< Y >, X > wheels;
};

template< int X, int Y >
round_robin_nd< X, Y >::round_robin_nd( std::array< int, X > const &indices )
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

/* Captain! Make note of why the wheel uses an array - size of ints etc */
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

}
