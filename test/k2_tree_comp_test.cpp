#include "sdsl/k2_tree_algorithm.hpp"
#include "gtest/gtest.h"
#include "k2_tree_test_helper.hpp"

namespace {

    using namespace sdsl;
    using namespace std;

    typedef int_vector<>::size_type size_type;

    string test_file;
    string temp_file;
    bool in_memory;

    template<class T>
    class k2_tree_comp_test : public ::testing::Test {
    };

    using testing::Types;

    typedef k2_tree_hybrid<2, 2, 2, 2, bit_vector, rrr_vector<63>> hybrid_k2_2222_b_rrr;
    typedef k2_tree_hybrid<4, 5, 2, 4, bit_vector, rrr_vector<63>> hybrid_k2_4524_b_rrr;
    typedef k2_tree_hybrid<2, 5, 2, 8, bit_vector, rrr_vector<63>> hybrid_k2_2528_b_rrr;
    typedef k2_tree_hybrid<8, 5, 2, 8, bit_vector, rrr_vector<63>> hybrid_k2_8523_b_rrr;
    typedef k2_tree_comp<2, bit_vector> k2;
    typedef k2_tree_comp<4, bit_vector> k4;
    typedef k2_tree_comp<8, bit_vector> k8;

    typedef Types<
            /*k2,
            k2rrr,
            k3,
            k4,
            k6,
            k8,
            k16,*/
            /*43*/
            k2_tree_partitioned<k2>,
            k2_tree_partitioned<k4>,
            k2_tree_partitioned<k8>,/*
            hybrid_k2_2222_b_rrr,
            hybrid_k2_4524_b_rrr,
            hybrid_k2_2528_b_rrr,
            hybrid_k2_165216_b_rrr,
            hybrid_k2_8523_b_rrr,
            hybrid_k2_3524_b_rrr,*/
            k2_tree_partitioned<hybrid_k2_4524_b_rrr>,
            k2_tree_partitioned<hybrid_k2_2528_b_rrr>,
            k2_tree_partitioned<hybrid_k2_8523_b_rrr>
    > Implementations;

    TYPED_TEST_CASE(k2_tree_comp_test, Implementations);

    TYPED_TEST(k2_tree_comp_test, CreateAndStoreTest) {
        int_vector_buffer<> buf_x(test_file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(test_file + ".y", std::ios::in);
        TypeParam k2treap(buf_x, buf_y, PARTITIONBASED);
        //construct(k2treap, test_file);
        ASSERT_TRUE(store_to_file(k2treap, temp_file));
        TypeParam k2treap2;
        ASSERT_TRUE(load_from_file(k2treap2, temp_file));
        ASSERT_EQ(k2treap, k2treap2);
    }

    TYPED_TEST(k2_tree_comp_test, ConstructCompareTest) {
        int_vector_buffer<> buf_x(test_file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(test_file + ".y", std::ios::in);
        TypeParam k2treap;
        TypeParam k2treap2;
        TypeParam k2treap3;
        construct(k2treap, test_file, PARTITIONBASED);
        construct(k2treap2, test_file, COUNTING_SORT);
        construct(k2treap3, test_file, ZORDER_SORT);
        std::cout << "Comparing Results" << std::endl;

        if (!( k2treap == k2treap2)){
            std::cout << "Results differ" << std::endl;
        }


        if (!( k2treap == k2treap3)){
            std::cout << "Results of zorder sort differ" << std::endl;
        }

        ASSERT_EQ(k2treap, k2treap2);
        ASSERT_EQ(k2treap2, k2treap3);
    }

    TYPED_TEST(k2_tree_comp_test, size) {
        TypeParam k2treap;
        ASSERT_TRUE(load_from_file(k2treap, temp_file));
        int_vector<> x, y;
        ASSERT_TRUE(load_from_file(x, test_file + ".x"));
        ASSERT_TRUE(load_from_file(y, test_file + ".y"));
        ASSERT_EQ(x.size(), y.size());
        ASSERT_EQ(x.size(), k2treap.size());
    }

    TYPED_TEST(k2_tree_comp_test, direct_links_2) {
        TypeParam k2treap;
        perform_direct_links_test(k2treap, temp_file, test_file, [&k2treap](uint64_t source_id, std::vector<uint64_t> &result) {
            k2treap.direct_links2(source_id, result);
        });
    }

    TYPED_TEST(k2_tree_comp_test, inverse_links2) {
        TypeParam k2treap;
        perform_inverse_links_test(k2treap, temp_file, test_file, [&k2treap](uint64_t source_id, std::vector<uint64_t> &result) {
            k2treap.inverse_links2(source_id, result);
        });
    }

    TYPED_TEST(k2_tree_comp_test, check_link) {
        TypeParam k2treap;
        perform_check_link_test(k2treap, temp_file, test_file, [&k2treap](std::pair<uint64_t, uint64_t> asd) {
            return k2treap.check_link(asd);
        });
    }


    /*
template<class t_k2treap>
void count_test(
    const t_k2treap& k2treap,
    complex<uint64_t> min_xy,
    complex<uint64_t> max_xy,
    const int_vector<>& x,
    const int_vector<>& y)
{
    uint64_t cnt = 0;
    for (uint64_t i = 0; i < x.size(); ++i) {
        if (x[i] >= real(min_xy) and x[i] <= real(max_xy)
            and y[i] >= imag(min_xy) and y[i] <= imag(max_xy)) {
            ++cnt;
        }
    }
    ASSERT_EQ(cnt, count(k2treap, {real(min_xy),imag(min_xy)}, {real(max_xy),imag(max_xy)}));
}

TYPED_TEST(k2_tree_comp_test, count)
{
    TypeParam k2treap;
    ASSERT_TRUE(load_from_file(k2treap, temp_file));
    int_vector<> x,y;
    ASSERT_TRUE(load_from_file(x, test_file+".x"));
    ASSERT_TRUE(load_from_file(y, test_file+".y"));
    ASSERT_EQ(x.size(), y.size());
    ASSERT_EQ(x.size(), k2treap.size());
    if (x.size() > 0) {
        std::mt19937_64 rng;
        std::uniform_int_distribution<uint64_t> distribution(0, x.size()-1);
        auto dice = bind(distribution, rng);
        for (size_t i=0; i<3; ++i) {
            auto idx1 = dice();
            auto idx2 = dice();
            uint64_t x1 = x[idx1];
            uint64_t y1 = y[idx1];
            uint64_t x2 = x[idx2];
            uint64_t y2 = y[idx2];
            count_test(k2treap, {std::min(x1,x2), std::min(y1,y2)}, {std::max(x1,x2),std::max(y1,y2)}, x, y);
        }
    }
}
*/

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    if (argc < 3) {
        // LCOV_EXCL_START
        cout << "Usage: " << argv[0] << " file temp_file [in-memory]" << endl;
        cout << " (1) Generates a k2-treap out of file.x, file.y, and file.w." << endl;
        cout << "     Result is stored in temp_file." << endl;
        cout << "     If `in-memory` is specified, the in-memory construction is tested." << endl;
        cout << " (2) Performs tests." << endl;
        cout << " (3) Deletes temp_file." << endl;
        return 1;
        // LCOV_EXCL_STOP
    }
    test_file = argv[1];
    temp_file = argv[2];
    in_memory = argc > 3;
    if (in_memory) {
        auto load_and_store_in_mem = [&](string suf) {
            int_vector<> data;
            string file = temp_file + suf;
            load_vector_from_file(data, file);
            string ram_file = ram_file_name(file);
            store_to_file(data, ram_file);
        };
        load_and_store_in_mem("x");
        load_and_store_in_mem("y");
        temp_file = ram_file_name(temp_file);
    }
    return RUN_ALL_TESTS();
}
