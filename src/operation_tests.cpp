#include "operation.hpp"
#include "tests_general.hpp"

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

    for( int i = 0; i < iterations; ++i ) rr2.back_spin();

    REQUIRE( gold == rr2.get_indices() );
  }

  /* Test regular, irregular, and edge cases */
  SECTION("simple kronecker product")
  {
    int const num_matrices = 3;
    int const rows = 2;
    int const cols = 2;

    int const flat_size = num_matrices * rows * cols;
    int const input_size = std::pow( cols, num_matrices );
    int const output_size = std::pow( rows, num_matrices );

    /* matrices */
    std::array< TestType, flat_size > flat_tensor =
    { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

    /* input vector */
    std::array< TestType, input_size > input =
    {8, 7, 6, 5, 4, 3, 2, 1 };
    
    /* output space - should be set to zero */
    std::array< TestType, output_size > output{};
    for( auto i : output ) assert( i == 0 );

    std::array< TestType, output_size > correct_answer =
    { 6376, 7356, 7736, 8448, 9392, 10256, 10784, 11776 };

    data::tensor< TestType, num_matrices, cols, rows > t( flat_tensor.data() );
    
    data::vector_1d< TestType, input.size() > v_in( input.data() );
    data::vector_1d< TestType , output.size() > v_out( output.data() );

    operation::kron_operation< TestType, num_matrices, cols, rows > kron_op( t, v_in, v_out );

    kron_op.compute_kron();

    std::cout << "v_out:" << std::endl;
    for( int i = 0; i < v_out.size(); ++i )
    {
      std::cout << " " << v_out[ i ];
    }
    std::cout << std::endl;

    /* Captain! Put this in a namespace too */
    trivial_truth();
  }

  SECTION( "irregular kronecker product" )
  {
    int const num_matrices = 4;
    int const rows = 2;
    int const cols = 3;

    int const flat_size = num_matrices * rows * cols;
    int const input_size = std::pow( cols, num_matrices );
    int const output_size = std::pow( rows, num_matrices );

    /* matrices */
    std::array< TestType, flat_size > flat_tensor =
    { 6, 7, 4, 5, 2, 3, 2, 3, 4, 5, 6, 7, 8, 13, 10, 11, 12, 9, 2, 2, 2, 2, 2, 2 };

    /* input vector contains all 1's */
    std::array< TestType, input_size > input;
    for( auto &i : input ) i = 1;
    
    /* output space - should be set to zero */
    std::array< TestType, output_size > output{};
    for( auto i : output ) assert( i == 0 );

    std::array< TestType, output_size > correct_answer =
    {25920, 25920, 28512, 28512, 32400, 32400, 35640, 35640, 
     32400, 32400, 35640, 35640, 40500, 40500, 44550, 44550};

    data::tensor< TestType, num_matrices, cols, rows > t( flat_tensor.data() );
    
    data::vector_1d< TestType, input.size() > v_in( input.data() );
    data::vector_1d< TestType , output.size() > v_out( output.data() );

    operation::kron_operation< TestType, num_matrices, cols, rows > kron_op( t, v_in, v_out );

    kron_op.compute_kron();

    std::cout << "v_out:" << std::endl;
    for( int i = 0; i < v_out.size(); ++i )
    {
      std::cout << " " << v_out[ i ];
    }
    std::cout << std::endl;

    /* Captain! Put this in a namespace too */
    trivial_truth();
  }
}























