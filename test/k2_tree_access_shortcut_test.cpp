#include <sdsl/rrr_vector.hpp>
#include <sdsl/k2_tree_hybrid.hpp>
#include <sdsl/k2_tree_comp.hpp>
#include <sdsl/k2_tree_partitioned.hpp>
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
    class k2_tree_access_shortcut_test : public ::testing::Test {
    };

    using testing::Types;

    typedef k2_tree_hybrid<2, 2, 2, 2, bit_vector, rrr_vector<63>> hybrid_k2_2222_b_rrr;
    typedef k2_tree_hybrid<4, 5, 2, 4, bit_vector, rrr_vector<63>> hybrid_k2_4524_b_rrr;
    typedef k2_tree_hybrid<2, 5, 2, 8, bit_vector, rrr_vector<63>> hybrid_k2_2528_b_rrr;
    typedef k2_tree_hybrid<8, 5, 2, 8, bit_vector, rrr_vector<63>> hybrid_k2_8528_b_rrr;
    typedef k2_tree_comp<2, bit_vector, bit_vector> k2comp;
    typedef k2_tree_comp<2, bit_vector, bit_vector> k2;
    typedef k2_tree_comp<2, bit_vector, rrr_vector<63>> k2rrr;

    typedef Types<
            k2comp,
            k2rrr,
            k2,
            k2_tree_partitioned<k2>,
            k2_tree_partitioned<k2>,
            k2_tree_partitioned<k2rrr>,
            hybrid_k2_2222_b_rrr,
            hybrid_k2_4524_b_rrr,
            hybrid_k2_2528_b_rrr,
            hybrid_k2_8528_b_rrr,
            k2_tree_partitioned<hybrid_k2_4524_b_rrr>,
            k2_tree_partitioned<hybrid_k2_2528_b_rrr>,
            k2_tree_partitioned<hybrid_k2_8528_b_rrr>
    > Implementations;

    TYPED_TEST_CASE(k2_tree_access_shortcut_test, Implementations);

    TYPED_TEST(k2_tree_access_shortcut_test, CreateAndStoreTest) {
        int_vector_buffer<> buf_x(test_file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(test_file + ".y", std::ios::in);
        try {
            TypeParam k2treap(buf_x, buf_y, COUNTING_SORT);
            //construct(k2treap,k test_file);
            ASSERT_TRUE(store_to_file(k2treap, temp_file));
            TypeParam k2treap2;
            ASSERT_TRUE(load_from_file(k2treap2, temp_file));
            ASSERT_EQ(k2treap, k2treap2);

            std::vector<uint> shortcut_sizes_to_test = {2,4};//,8,16};

            for (auto shortcut_size : shortcut_sizes_to_test) {
                k2treap.construct_access_shortcut(shortcut_size);
                // a bit hacky because construction can fail during runtime, when access_shortcut_size > t_k_l_1 leading to temp_file not being present
                perform_check_link_test(k2treap, temp_file, test_file, [&k2treap](std::pair<uint64_t, uint64_t> asd) {
                    return k2treap.check_link(asd);
                });

                perform_check_link_test(k2treap, temp_file, test_file, [&k2treap](std::pair<uint64_t, uint64_t> asd) {
                    return k2treap.check_link_shortcut(asd);
                });

                perform_direct_links_test(k2treap, temp_file, test_file, [&k2treap](uint64_t source_id, std::vector<uint64_t> &result) {
                    k2treap.direct_links_shortcut_2(source_id, result);
                });

                perform_direct_links_test(k2treap, temp_file, test_file, [&k2treap](uint64_t source_id, std::vector<uint64_t> &result) {
                    k2treap.direct_links_shortcut(source_id, result);
                });


                perform_inverse_links_test(k2treap, temp_file, test_file, [&k2treap](uint64_t source_id, std::vector<uint64_t> &result) {
                    k2treap.inverse_links_shortcut(source_id, result);
                });
            }
        } catch (std::runtime_error const &e) {
            std::cerr << "Exception occured " << e.what() << std::endl;
            //quite hacky comparing strings
            if (strcmp(e.what(), "Cannot use check_link_shortcut if m_access_shortcut_size == 0") != 0) {
                FAIL();
            }
        }

    }
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
