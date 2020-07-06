#include "data.hpp"

namespace data
{
/* constructor */
template< typename T >
vector_1d< T >::vector_1d( int const len, T *data )
  :
  len( len ),
  data( data )
{
  return;
}

template< typename T >
std::vector< T > vector_1d< T >::get_std_vector()
{
  return std::vector< T >( data, data + len );
}

/* explicit instantiations of vector_1d */
template class vector_1d< double >;
template class vector_1d< float >;

}
