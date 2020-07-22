/*

This test file illustrates the library's workflor and verifies correct results on
test problems.

*/
#include "operation.hpp"
#include "tests_general.hpp"

TEMPLATE_TEST_CASE( "operation component", "operation tests", double, float )
{
  /* the round_robin_nd class cycles sequentially through "n" finite states. If "n" is the
     number of states, then cycling it "n" times forwards or "n" times backwards should return
     it to the original state */
  SECTION( "round_robin_nd finite state machine test" )
  {
    /* contains 3 round robin wheels of capacity 5 each */
    int const num_wheels = 3;
    int const wheel_size = 5;
    std::array< int, num_wheels > gold = { 0, 2, 3 };

    operation::round_robin_nd< num_wheels, wheel_size > rr2( gold );

    int iterations = std::pow( wheel_size, num_wheels );

    for( int i = 0; i < iterations; ++i ) rr2.spin();

    REQUIRE( gold == rr2.get_indices() );

    for( int i = 0; i < iterations; ++i ) rr2.back_spin();

    REQUIRE( gold == rr2.get_indices() );
  }

  SECTION("simple Kronecker product with square matrices")
  {
    int const num_matrices = 3;
    int const rows = 2;
    int const cols = 2;

    int const flat_size = num_matrices * rows * cols;
    int const input_size = std::pow( cols, num_matrices );
    int const output_size = std::pow( rows, num_matrices );

    /* 3 matrices of size 2x2 as flat data */
    std::array< TestType, flat_size > flat_tensor =
    { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

    /* input vector */
    std::array< TestType, input_size > input =
    {8, 7, 6, 5, 4, 3, 2, 1 };
    
    /* output space - should be set to zero */
    std::array< TestType, output_size > output{};
    for( auto i : output ) assert( i == 0 );

    std::array< TestType, output_size > correct_answer =
    { 6736, 7356, 7736, 8448, 9392, 10256, 10784, 11776 };

    /* tensor-encode the matrices */
    data::tensor< TestType, num_matrices, cols, rows > t( flat_tensor.data() );
    
    data::vector_1d< TestType, input.size() > v_in( input.data() );
    data::vector_1d< TestType , output.size() > v_out( output.data() );

    /* pass parameters to a kron_operation objet */
    operation::kron_operation< TestType, num_matrices, cols, rows > kron_op( t, v_in, v_out );

    /* carry out the computation and write the result to "v_out" */
    kron_op.compute_kron();

    /* verify correct results */
    testing::rmse_comparison< TestType, output_size >( output, correct_answer );
  }

  SECTION( "simple Kronecker product with irregular matrices" )
  {
    int const num_matrices = 4;
    int const rows = 2;
    int const cols = 3;

    int const flat_size = num_matrices * rows * cols;
    int const input_size = std::pow( cols, num_matrices );
    int const output_size = std::pow( rows, num_matrices );

    /* matrix data is specified in column major order */
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

    /* create a tensor from the flat array of data */
    data::tensor< TestType, num_matrices, cols, rows > t( flat_tensor.data() );
    
    /* create 1D vectors */
    data::vector_1d< TestType, input.size() > v_in( input.data() );
    data::vector_1d< TestType , output.size() > v_out( output.data() );

    /* create a kron_operation object with the parameters created above */
    operation::kron_operation< TestType, num_matrices, cols, rows > kron_op( t, v_in, v_out );

    /* write the result of the computation into "v_out" */
    kron_op.compute_kron();

    /* verify correct results */
    testing::rmse_comparison< TestType, output_size >( output, correct_answer );
  }
}
