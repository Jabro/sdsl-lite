#include "sdsl/k2_tree_comp.hpp"
#include "sdsl/bit_vectors.hpp"
#include "gtest/gtest.h"
#include <vector>
#include <tuple>
#include <string>
#include <algorithm> // for std::min. std::sort
#include <random>

namespace sdsl {

    using namespace sdsl;
    using namespace std;

    typedef int_vector<>::size_type size_type;

        template<class T>
        class K2TreeInternalTest : public ::testing::Test {
        };

    template<uint8_t t_k,
            typename t_lev=bit_vector,
            typename t_leaf=bit_vector,
            typename t_rank=typename t_lev::rank_1_type>
    void get_paper_k2_tree(k2_tree_comp<t_k,t_lev,t_leaf,t_rank> &idx, uint access_shortcut_size = 0){
        //This Matrix is the same as the 16x16 matrix in the paper, the rows containing only 0s are omitted
        /*
        * 0 1 0 0 | 0 0 0 0 | 0 0 0
        * 0 0 1 1 | 1 0 0 0 | 0 0 0
        * 0 0 0 0 | 0 0 0 0 | 0 0 0
        * 0 0 0 0 | 0 0 0 0 | 0 0 0
        * -------------------------
        * 0 0 0 0 | 0 0 0 0 | 0 0 0
        * 0 0 0 0 | 0 0 0 0 | 0 0 0
        * 0 0 0 0 | 0 0 0 0 | 0 0 0
        * 0 0 0 0 | 0 0 1 0 | 0 0 0
        * -------------------------
        * 0 0 0 0 | 0 0 1 0 | 0 1 0
        * 0 0 0 0 | 0 0 1 0 | 1 0 1
        * 0 0 0 0 | 0 0 1 0 | 0 1 0
        */
        std::string tmp_prefix = ram_file_name("k2_tree_comp_test");
        std::vector<std::pair<uint32_t, uint32_t>> coordinates = {{0,1},{1,2},{1,3},{1,4},{7,6},{8,6},{8,9},{9,6},{9,8},{9,10},{10,6},{10,9}};
        k2_tree_comp<t_k,t_lev,t_leaf,t_rank> tmp(coordinates, COUNTING_SORT);
        tmp.swap(idx);
    }

    TEST(K2TreeInternalTest, testZOrderSort) {

        std::vector<pair<uint, uint>> points;
        int size = 8;
        const int k = 2;
        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++) {
                points.push_back(std::make_pair(i,j));
            }
        }

        k2_tree_comp<2> tree;
        auto morton_numbers = tree.calculate_morton_numbers(points[0].first, points);

        std::sort(morton_numbers.begin(), morton_numbers.end(), [&](const uint64_t lhs, const uint64_t rhs){
            return lhs < rhs;
        });

        std::vector<uint64_t> expected(size*size);
        for (int l = 0; l < size*size; ++l) {
            expected[l] = l;
        }

        ASSERT_EQ(morton_numbers.size(),expected.size());
        std::cout << std::endl;
        for (uint k = 0; k < expected.size(); ++k) {
            ASSERT_EQ(expected[k], morton_numbers[k]);
        }
    }

    TEST(K2TreeInternalTest, test_morton_number_calculation) {
        std::vector<pair<uint, uint>> points;
        int size = 1000;
        const int k = 2;
        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++) {
                points.push_back(std::make_pair(i,j));
            }
        }

        k2_tree_hybrid<4,1,2,8> tree(points, COUNTING_SORT); //build tree to initialize everything

        std::vector<uint64_t> morton_numbers(size*size);
        tree.calculate_morton_numbers_internal(points, morton_numbers);

        std::vector<uint64_t> morton_numbers2(size*size);
        tree.calculate_morton_numbers_internal_pdep(points, morton_numbers2);

        ASSERT_EQ(morton_numbers, morton_numbers2);
    }

    void makeZOrderK(uint& xinit, uint& yinit, uint& ctr, uint k, std::vector<pair<uint, uint>>& points){
        for (uint x = xinit; x < xinit+k; ++x) {
            for (uint y = yinit; y < yinit+k; ++y) {
                ASSERT_EQ(std::make_pair(x, y), points[ctr]);
                ctr++;
            }
        }
    }

    void recursivelyCreateMatrix(uint size, uint xinit, uint yinit, uint& ctr, std::vector<pair<uint, uint>>& points, uint k){
        if (size == k) {
            //std::cout << "Rec Base" << std::cout << std::endl;
            makeZOrderK(xinit, yinit, ctr, k, points);
        } else if (size < k) {
            throw runtime_error("shouldn't happen, please choose a size that is an exponential of chosen k");
        } else {
            uint initialY = yinit;
            for (uint i = 0; i < k; ++i) {
                //first row
                yinit = initialY;
                for (uint j = 0; j < k; ++j) {
                    //std::cout << "Rec Call" << std::cout << std::endl;
                    recursivelyCreateMatrix(size / k, xinit, yinit, ctr, points, k);
                    yinit += size/k;
                }
                xinit += size/k;

            }
        }
    }
/*
    TEST(K2TreeInternalTest, testZOrderSort2) {

        const int k = 3;
        std::vector<pair<uint, uint>> points;
        int size = 81;
        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++) {
                points.push_back(std::make_pair(i,j));
            }
        }

        std::random_shuffle (points.begin(), points.end());

        k2_tree_comp<k, bit_vector> tree;
        tree.set_height(log(size)/log(k));

        std::sort(points.begin(), points.end(), [&](const std::pair<uint32_t, uint32_t>& lhs, const std::pair<uint32_t, uint32_t>& rhs){
            return tree.sort_by_z_order(lhs, rhs);
        });

        uint ctr = 0;
        vector<pair<uint32_t, uint32_t>> correct_order;
        recursivelyCreateMatrix(size, 0, 0, ctr, points, k);
    }

    TEST(K2TreeInternalTest, test_calculate_subtree_number_and_new_relative_coordinates) {
        k2_tree_comp<2> tree;
        get_paper_k2_tree(tree);
        //crappy test with magic numbers ;-)
        std::pair<uint, uint> test_link = std::make_pair(8,9);
        uint subtree_number = tree.calculate_subtree_number_and_new_relative_coordinates(test_link,0);
        ASSERT_EQ(subtree_number, (uint) 3);
        ASSERT_EQ(test_link.first, (uint) 0);
        ASSERT_EQ(test_link.second, (uint) 1);
        uint subtree_number_2 = tree.calculate_subtree_number_and_new_relative_coordinates(test_link,1);
        ASSERT_EQ(subtree_number_2, (uint) 0);
    }
/*
    TEST(K2TreeInternalTest, test_access_shortcut) {
        using namespace k2_treap_ns;
        uint access_shortcut_size = 3;
        k2_tree_comp<2> tree;
        get_paper_k2_tree(tree, access_shortcut_size);

        //crappy test with magic numbers ;-)
        node_type* node = tree.check_link_shortcut((uint32_t)9, (uint32_t) 6);

        ASSERT_EQ(node->p.real() , (uint) 8);
        ASSERT_EQ(node->p.imag(), (uint) 6);
        ASSERT_EQ(node->idx, (uint) 52);
    }*/
}
// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
