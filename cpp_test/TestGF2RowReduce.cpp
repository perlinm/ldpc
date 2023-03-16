#include <gtest/gtest.h>
#include "gf2sparse.hpp"
#include "gf2sparse_linalg.hpp"
#include "sparse_matrix_util.hpp"
#include "rapidcsv.h"
#include <iostream>
#include <set>
#include "io.hpp"
#include <string>

using namespace gf2sparse;
using namespace gf2sparse_linalg;

TEST(RowReduce, init1){

    
    auto matrix = GF2Sparse<>::New(3,3);
    for(int i =0; i<3; i++) matrix->insert_entry(i,i);
    matrix->insert_entry(1,0);
    auto rr = RowReduce(matrix);


}

TEST(RowReduce, initiliase_LU){

    
    auto matrix = GF2Sparse<>::New(5,5);
    for(int i =0; i<5; i++) matrix->insert_entry(i,i);
    matrix->insert_entry(1,0);

    auto rr = RowReduce(matrix);
    rr.initiliase_LU();
    auto U = rr.U;
    auto p1 = print_sparse_matrix(*U,true).str();
    auto p2 = print_sparse_matrix(*matrix, true).str();
    ASSERT_EQ(p1,p2);

}

TEST(RowReduce, set_column_row_orderings){

    
    auto matrix = GF2Sparse<>::New(4,4);
    for(int i =0; i<4; i++) matrix->insert_entry(i,i);
    matrix->insert_entry(1,0);

    auto rr = RowReduce(matrix);
    rr.initiliase_LU();
    auto U = rr.U;
    auto p1 = print_sparse_matrix(*U,true).str();
    auto p2 = print_sparse_matrix(*matrix, true).str();
    ASSERT_EQ(p1,p2);

    vector<int> rows = {0,1,2,3};
    vector<int> cols = {0,1,2,3};

    rr.set_column_row_orderings();

    for(int i = 0; i<4; i++){
        ASSERT_EQ(rr.rows[i],rows[i]);
        ASSERT_EQ(rr.cols[i], cols[i]);
    }

}

TEST(RowReduce, set_column_row_orderings2){

    
    auto matrix = GF2Sparse<>::New(4,4);
    for(int i =0; i<4; i++) matrix->insert_entry(i,i);
    matrix->insert_entry(1,0);

    auto rr = RowReduce(matrix);
    rr.initiliase_LU();
    auto U = rr.U;
    auto p1 = print_sparse_matrix(*U,true).str();
    auto p2 = print_sparse_matrix(*matrix, true).str();
    ASSERT_EQ(p1,p2);

    vector<int> rows = {0,1,3,2};
    vector<int> cols = {1,0,3,2};

    rr.set_column_row_orderings(cols,rows);

    for(int i = 0; i<4; i++){
        ASSERT_EQ(rr.rows[i],rows[i]);
        ASSERT_EQ(rr.cols[i], cols[i]);
    }

}


TEST(RowReduce, rref1){

    
    auto matrix = GF2Sparse<>::New(3,3);
    for(int i =0; i<3; i++) matrix->insert_entry(i,i);
    matrix->insert_entry(1,0);
    matrix->insert_entry(2,0);

    auto rr = RowReduce(matrix);
    auto U = rr.rref();
    auto B = rr.L->matmul(matrix);
    auto I = gf2_identity(3);
    ASSERT_EQ(B->gf2_equal(I),true);
    // print_sparse_matrix(*B);

}

TEST(RowReduce, rref2){

    auto matrix = gf2_identity(3);
    matrix->insert_entry(0,1);
    auto rr = RowReduce(matrix);
    auto U = rr.rref(true);
    auto B = rr.L->matmul(matrix);
    auto I = gf2_identity(3);
    ASSERT_EQ(B->gf2_equal(I),true);

}

TEST(GF2Sparse, rref_batch){

    auto csv_path = io::getFullPath("cpp_test/test_inputs/gf2_invert_test.csv");
    rapidcsv::Document doc(csv_path, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(';'));


    int row_count = doc.GetColumn<string>(0).size();

    for(int i = 0; i<row_count; i++){

        std::vector<string> row = doc.GetRow<string>(i);

        int m = stoi(row[0]);
        int n = stoi(row[1]);
        auto input_csr_vector = io::string_to_csr_vector(row[2]);

        auto matrix = GF2Sparse<>::New(m,n);
        matrix->csr_insert(input_csr_vector);

        auto I = gf2_identity(n);

        auto rr = new RowReduce(matrix);
        rr->rref(true);
        auto B = rr->L->matmul(matrix);

        ASSERT_EQ(B->gf2_equal(I),true);


        }

    }

TEST(GF2Sparse, reverse_cols){

    auto mat1 = GF2Sparse<>::New(3,3);
    auto mat2 = GF2Sparse<>::New(3,3);

    for(int i = 0; i<3; i++){
        mat1->insert_entry(i,3-1-i);
    }

    auto rr = RowReduce(mat1);
    vector<int> cols = {2,1,0};
    rr.rref(false,false,cols);

    ASSERT_EQ(mat1->gf2_equal(rr.U),true);

}

TEST(GF2Sparse, lu1){

    auto mat1 = GF2Sparse<>::New(3,3);

    for(int i = 0; i<3; i++){
        mat1->insert_entry(i,3-1-i);
    }

    mat1->insert_entry(1,0);
    mat1->insert_entry(0,1);
    auto rr = RowReduce(mat1);
    rr.rref(false,true);
    auto LU = rr.L->matmul(rr.U);
    mat1->reorder_rows(rr.rows);
    ASSERT_EQ(mat1->gf2_equal(LU),true);

}

TEST(RowReduce, lu2){

    auto mat1 = GF2Sparse<>::New(5,6);
    for(int i = 5; i>=3; i--){
        mat1->insert_entry(abs(i-5)+1,i);
    }
    mat1->insert_entry(4,5);
    mat1->insert_entry(4,3);

    auto rr = RowReduce(mat1);
    rr.rref(false, true);
    auto LU = rr.L->matmul(rr.U);    
    mat1->reorder_rows(rr.rows);
    ASSERT_EQ(mat1==LU,true);

}



TEST(GF2Sparse, lu_solve1){


    auto mat1 = GF2Sparse<>::New(5,8);
    for(int i = 5; i>=3; i--){
        mat1->insert_entry(abs(i-5)+1,i);
    }

    // print_sparse_matrix(*mat1);

    mat1->insert_entry(4,5);
    mat1->insert_entry(4,3);

    auto x = vector<uint8_t>{0,0,0,1,1,1,0,0};
    auto y = vector<uint8_t>(mat1->m,0);
    mat1->mulvec(x,y);

    auto rr = RowReduce(mat1);
    rr.rref(false,true);

    auto incorrect_length = vector<uint8_t>(mat1->m+1,0);
    EXPECT_THROW(rr.lu_solve(incorrect_length),std::invalid_argument);

    auto xe = rr.lu_solve(y);

    auto y2 = vector<uint8_t>(mat1->m,0);

    mat1->mulvec(xe,y2);

    for(int i = 0; i<mat1->m; i++){
        ASSERT_EQ(y2[i] == y[i],true);
    }

}


TEST(GF2Sparse, lu_solve2){


    auto mat1 = GF2Sparse<>::New(5,5);
 
    auto csr_vector = io::string_to_csr_vector("[[0,2,3],[1,2,3],[2,3,4],[3,4],[0]]");

    mat1->csr_insert(csr_vector);

    // print_sparse_matrix(*mat1);

    auto ys = io::binaryStringToVector("00011");

    auto rr = RowReduce(mat1);

    rr.rref(false,true);
    vector<uint8_t> x ={1,1,1,0,1};
    auto y = vector<uint8_t>(mat1->m,0);
    mat1->mulvec(x,y); 

    auto xe = rr.lu_solve(y);
    vector<uint8_t> ye = vector<uint8_t>(mat1->m,0);
    mat1->mulvec(xe,ye);
    for(int i=0; i<mat1->m; i++){
        ASSERT_EQ(ye[i]==y[i],true);
    }

    auto LU = rr.L->matmul(rr.U);
    mat1->reorder_rows(rr.rows);
    ASSERT_EQ(mat1==LU,true);

}


TEST(GF2Sparse, lu_solve3){


    auto mat1 = GF2Sparse<>::New(6,7);
 
    auto csr_vector = io::string_to_csr_vector("[[3],[2,3,6],[],[2,3,6],[2,3,6],[]]");

    mat1->csr_insert(csr_vector);

    print_sparse_matrix(*mat1);

    auto y = io::binaryStringToVector("110110");

    auto rr = RowReduce(mat1);

    rr.rref(false,true);

    cout<<endl;
    print_sparse_matrix(*rr.L);
    cout<<endl;
    print_sparse_matrix(*rr.U);

    cout<<rr.rank<<endl;

    auto xe = rr.lu_solve(y);
    vector<uint8_t> ye = vector<uint8_t>(mat1->m,0);
    mat1->mulvec(xe,ye);
    for(int i=0; i<mat1->m; i++){
        ASSERT_EQ(ye[i]==y[i],true);
    }




    auto LU = rr.L->matmul(rr.U);
    mat1->reorder_rows(rr.rows);
    ASSERT_EQ(mat1==LU,true);

}



TEST(DISABLED_GF2Sparse, lu_solve_batch){

    auto csv_path = io::getFullPath("cpp_test/test_inputs/gf2_lu_solve_test.csv");
    rapidcsv::Document doc(csv_path, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(';'));


    int row_count = doc.GetColumn<string>(0).size();

    for(int i = 0; i<row_count; i++){

        cout<<"row: "<<i+1<<endl;

        std::vector<string> row = doc.GetRow<string>(i);

        int m = stoi(row[0]);
        int n = stoi(row[1]);
        auto input_csr_vector = io::string_to_csr_vector(row[2]);
        auto input_vector = io::binaryStringToVector(row[3]);

        ASSERT_EQ(input_vector.size(),m);

        auto matrix = GF2Sparse<>::New(m,n);
        matrix->csr_insert(input_csr_vector);

        auto rr = RowReduce(matrix);
        rr.rref(false,true);
        auto x = rr.lu_solve(input_vector);

        vector<uint8_t> output_vector(m,0);

        matrix->mulvec(x,output_vector);

        print_sparse_matrix(*matrix);

        for(auto i = 0; i<m; i++){
            ASSERT_EQ(output_vector[i] == input_vector[i],true);
        }

    }

}




int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}