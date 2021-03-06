/* sdsl - succinct data structures library
    Copyright (C) 2014 Simon Gog

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope thatk2_ it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/
/*! \file k2_treap_algorithm.hpp
    \brief k2_treap_algorithm.hpp contains k^2-treap algorithms.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_K2_TREE_ALGORITHM
#define INCLUDED_SDSL_K2_TREE_ALGORITHM

#include "vectors.hpp"
#include "k2_tree_comp.hpp"
#include "k2_tree_hybrid.hpp"
#include "k2_tree_partitioned.hpp"
#include <tuple>
#include <algorithm>
#include <iterator>
#include <climits>
#include <vector>
#include <complex>
#include <queue>
#include <array>

//! Namespace for the succinct data structure library.
namespace sdsl {

    namespace k2_tree_ns {

//! Check if point x is contained in the rectangle (p1,p2)
/*! \param p Point.
 *  \param Lower left corner of the rectangle.
 *  \param Upper right corner of the rectangle.
 */
        bool
        contained(const point_type p, const point_type &p1, const point_type &p2) {
            return real(p) >= real(p1) and real(p) <= real(p2) and
                   imag(p) >= imag(p1) and imag(p) <= imag(p2);
        }

//! Check if the rectangle of node v is contained in the rectangle (p1,p2)
        template<uint8_t t_k>
        bool
        contained(const point_type &p1, const point_type &p2, const node_type &v) {
//    uint64_t d = (1ULL << v.t)-1;
//    uint64_t d = (1ULL << v.t)-1;
            uint64_t d = precomp<t_k>::exp(v.t) - 1;
            return real(p1) <= real(v.p) and real(p2) >= real(v.p) + d and
                   imag(p1) <= imag(v.p) and imag(p2) >= imag(v.p) + d;
        }

//! Check if rectangle (p1,p2) and the area of node v overlap
        template<uint8_t t_k>
        bool
        overlap(const point_type &p1, const point_type &p2, const node_type &v) {
//    uint64_t d = (1ULL << v.t)-1;
            uint64_t d = precomp<t_k>::exp(v.t) - 1;
            return real(p1) <= real(v.p) + d and real(p2) >= real(v.p) and
                   imag(p1) <= imag(v.p) + d and imag(p2) >= imag(v.p);
        }
    }

    using namespace k2_tree_ns;
// forward declaration
   template<uint8_t t_k,
            typename t_lev,
            typename t_leaf,
            typename t_rank>
    class k2_tree_comp;

    // forward declaration
    template<uint8_t t_k_l_1,
            uint8_t t_k_l_1_size,
            uint8_t t_k_l_2,
            uint8_t t_k_leaves,
            typename t_lev,
            typename t_leaf,
            typename t_rank>
    class k2_tree_hybrid;

    // forward declaration
    template<typename subk2_tree>
    class k2_tree_partitioned;


    template<typename subk2_tree>
    void
    construct(k2_tree_partitioned<subk2_tree> &idx, std::string file = "", construction_algorithm cons_algo = COUNTING_SORT) {
        int_vector_buffer<> buf_x(file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(file + ".y", std::ios::in);
        k2_tree_partitioned<subk2_tree> tmp(buf_x, buf_y, cons_algo);
        tmp.swap(idx);
    }


    template<uint8_t t_k,
            typename t_lev=bit_vector,
            typename t_leaf=bit_vector,
            typename t_rank=typename t_lev::rank_1_type>
    void
    construct(k2_tree_comp <t_k, t_lev, t_leaf, t_rank> &idx, std::string file = "", construction_algorithm cons_algo = COUNTING_SORT) {
        int_vector_buffer<> buf_x(file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(file + ".y", std::ios::in);
        k2_tree_comp<t_k, t_lev, t_leaf, t_rank> tmp(buf_x, buf_y, cons_algo);
        tmp.swap(idx);
    }

    template<uint8_t t_k_l_1,
            uint8_t t_k_l_1_size,
            uint8_t t_k_l_2,
            uint8_t t_k_leaves,
            typename t_lev=bit_vector,
            typename t_leaf=bit_vector,
            typename t_rank=typename t_lev::rank_1_type>
    void
    construct(k2_tree_hybrid<t_k_l_1, t_k_l_1_size, t_k_l_2, t_k_leaves, t_lev, t_leaf, t_rank> &idx, std::string file = "", construction_algorithm cons_algo = COUNTING_SORT) {
        int_vector_buffer<> buf_x(file + ".x", std::ios::in);
        int_vector_buffer<> buf_y(file + ".y", std::ios::in);
        k2_tree_hybrid<t_k_l_1, t_k_l_1_size, t_k_l_2, t_k_leaves, t_lev, t_leaf, t_rank> tmp(buf_x, buf_y, cons_algo);
        tmp.swap(idx);
    }

    template<uint8_t t_k,
            typename t_lev=bit_vector,
            typename t_leaf=bit_vector,
            typename t_rank=typename t_lev::rank_1_type,
            typename t_vector>
    void
    construct_im(k2_tree_comp<t_k, t_lev, t_leaf, t_rank> &idx, t_vector data, uint64_t max_hint = 0,
                 construction_algorithm cons_algo = COUNTING_SORT) {
        std::string tmp_prefix = ram_file_name("k2_treap_");
        k2_tree_comp<t_k, t_lev, t_leaf, t_rank> tmp(data, cons_algo, max_hint, tmp_prefix);
        tmp.swap(idx);
    }


    template<typename subk2_tree,
            typename t_vector>
    void
    construct_im(k2_tree_partitioned<subk2_tree> &idx, t_vector data, uint64_t max_hint = 0, construction_algorithm cons_algo = COUNTING_SORT){
        std::string tmp_prefix = ram_file_name("k2_treap_");
        k2_tree_partitioned<subk2_tree> tmp(data, cons_algo, max_hint, 0, tmp_prefix);
        tmp.swap(idx);
    }

    template<uint8_t t_k_l_1,
            uint8_t t_k_l_1_size,
            uint8_t t_k_l_2,
            uint8_t t_k_leaves,
            typename t_lev=bit_vector,
            typename t_leaf=bit_vector,
            typename t_rank=typename t_lev::rank_1_type,
            typename t_vector>
    void
    construct_im(k2_tree_hybrid<t_k_l_1, t_k_l_1_size, t_k_l_2, t_k_leaves, t_lev, t_leaf, t_rank> &idx,
                 t_vector data, uint64_t max_hint = 0, construction_algorithm cons_algo = COUNTING_SORT){
        std::string tmp_prefix = ram_file_name("k2_treap_");
        k2_tree_hybrid<t_k_l_1, t_k_l_1_size, t_k_l_2, t_k_leaves, t_lev, t_leaf, t_rank>
                tmp(data, cons_algo, max_hint, tmp_prefix);
        tmp.swap(idx);
    }
}
#endif
