#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <complex>
#include <sdsl/k2_tree_comp.hpp>
#include <sdsl/bit_vectors.hpp>
#include <chrono>
#include <sys/times.h>
#include <sdsl/k2_tree_hybrid.hpp>
#include <sdsl/k2_tree_partitioned.hpp>

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

using namespace sdsl;

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr,
                "USAGE: %s <path to serialized k tree (set templates correctly!)> query_file (binary format) [use access shortcut]\n",
                argv[0]);
        return (-1);
    }

    //char *filename = (char *)malloc(sizeof(char)*20);
    //typedef k2_tree_hybrid<k,k,k,k, bit_vector, bit_vector,true> k2_rrr;
    //typedef k2_tree_comp<k, bit_vector, bit_vector> tested_type;
    //const uint8_t k = 4;
    typedef k2_tree_hybrid<4,7,2,8, bit_vector, bit_vector> k2_rrr;
    typedef k2_tree_partitioned<k2_rrr> tested_type;

    //typedef k2_tree_partitioned<4, k2_rrr, true> k2_part;

//    typedef k2_tree_hybrid<4,5,2,8, bit_vector, bit_vector, false> k2_rrr;
//    typedef k2_tree_partitioned<4, k2_rrr, true> k2_part;

    tested_type k2tree;
    std::string fileName = argv[1];
    load_from_file(k2tree, fileName);

    uint hash_size = 20000000;
    std::cout << "hash_size=" << hash_size << std::endl;

    k2tree.compress_leaves(DAC, hash_size);

    std::string output_file_name(argv[2]);
    store_to_file(k2tree, output_file_name);

    std::cout << "Word iteration " << word_iteration << std::endl;
    std::cout << "frequency_encoding " << frequency_encoding << std::endl;
    std::cout << "dac_compression " << dac_compression << std::endl;

    write_structure<HTML_FORMAT>(k2tree, output_file_name + "("+ k2tree.get_type_string()+ ")" + ".html");



    return 0;
}
