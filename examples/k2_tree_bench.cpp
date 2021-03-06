//
// Created by d056848 on 6/7/16.
//

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <sys/times.h>
#include <sdsl/k2_tree_utility.hpp>
#include "sdsl/k2_tree_hybrid.hpp"
#include "sdsl/k2_tree_partitioned.hpp"
#include "sdsl/k2_tree_comp.hpp"

using std::ifstream;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using ::std::ofstream;
using namespace sdsl;
using namespace std;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void print(std::vector<uint32_t> &to_print, uint32_t source_node) {
    std::cout << "Reachable nodes from " << source_node << std::endl;
    for (auto &link: to_print) {
        std::cout << link << "\t";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {




/* end Time meassuring */

    /*Graph Format:
    <NumNodes>
    Source  Target
    Source  Target


    First lines of eu2005
     862664
     0       240
     0       420
     0       620
     0       630
    */
    if (argc < 4) {
        fprintf(stderr,
                "USAGE: %s <GRAPH> <QUERY-FILE> <OUTPUT-FILE> [Hash-Size]\n <GRAPH> has to be in the .ladrabin format",
                argv[0]);
        return (-1);
    }

    std::string file_name(argv[1]);
    std::string query_file_name(argv[2]);
    std::string output_file_name(argv[3]);
    uint64_t hash_size = 0;
    if (argc > 4)
        hash_size = stoull(argv[4]);
    
    const uint8_t k = 4;
//    typedef k2_tree_comp<k, bit_vector, bit_vector> tested_type;
    //typedef k2_tree_hybrid<4, 5, 2, 4, bit_vector, bit_vector> tested_type;
    typedef k2_tree_hybrid<4, 6, 2, 8, bit_vector, bit_vector> tested_type;
    //typedef k2_tree_partitioned<k2_rrr> tested_type;
    //construct_im(k2treap, coordinates, numberOfNodes - 1);

    tested_type k2tree;
    construction_algorithm construction = ZORDER_SORT; //should be determined by type automatically
    bool use_shortcut = false;
    uint64_t peak_RSS;
    uint64_t peak_VMEM;
    uint64_t construction_time;
    {
        mem_monitor mem_monitor1("bench_script_mem");
        memory_monitor::start();
        auto start = timer::now();
        k2tree.load_from_ladrabin(file_name, construction);
        auto stop = timer::now();
        memory_monitor::stop();
        auto status = mem_monitor1.get_current_stats();
        std::cout << "peak usage = " << memory_monitor::peak() / (1024*1024) << " MB" << std::endl;
        std::ofstream cstofs("cst-construction.html");
        cout << "writing memory usage visualization to cst-construction.html\n";
        memory_monitor::write_memory_log<HTML_FORMAT>(cstofs);
        cstofs.close();
        peak_RSS = mem_monitor1.max_seen_rss;
        peak_VMEM = mem_monitor1.max_seen_vmem;
        construction_time = duration_cast<milliseconds>(stop - start).count();
    }

	cout << "# construct_morton_duration = " << morton_number_duration << endl;
        cout << "# construct_bv_complete_duration = " << construct_bv_complete_duration  << endl;
        cout << "# construct_sort_duration = " << sort_duration << endl;
        cout << "# construct_duration = " << construct_duration << endl;
        cout << "# buildvec_duration = " << build_vec_duration << endl;
        cout << "# subtree_constructor_duration = " << constructor_duration << endl;
        cout << "# constructor_call_duration = " << constructor_call_duration << endl;
    cout << "# k2_size = " << size_in_bytes(k2tree) << endl;
    std::cout << "Speed test without compression" << std::endl;
    access_times times_uncomp = perform_speed_test(query_file_name, k2tree, query_file_name+".single", use_shortcut);
    std::cout << "Direct recovered: " << times_uncomp.direct_recovered << std::endl;
    std::cout << "Inverse recovered: " << times_uncomp.inverse_recovered << std::endl;
    std::cout << "Links recovered: " << times_uncomp.check_present << std::endl;
    std::cout << "Hereyougo:" << file_name << "\t" << k2tree.get_type_string() <<  "\t" << construction_time << "\t" << peak_RSS << "\t" << peak_VMEM;
    if (use_shortcut){
        //Construction Time	Compressed Size (Byte)	Bpe	Direct Short (ns)	Direct (ns)	Inverse Short (ns)	Inverse (ns)	Check S (ns)	Check (ns)
        std::cout << "\t" <<  times_uncomp.direct_short_time << "\t" << times_uncomp.direct_time << "\t" << times_uncomp.inverse_short_time << "\t" << times_uncomp.inverse_time << "\t" << times_uncomp.check_short_time << "\t" << times_uncomp.check_time << std::endl;
	std::cout << "Direct Short recovered: " << times_uncomp.direct_short_recovered << std::endl;
	std::cout << "Inverse Short recovered: " << times_uncomp.inverse_short_recovered << std::endl;
    } else {
        //Construction Time	Compressed Size (Byte)	Bpe	Direct (ns)	Inverse (ns)	Check (ns)
        std::cout << "\t" << times_uncomp.direct_time << "\t" << times_uncomp.inverse_time << "\t" << times_uncomp.check_time << std::endl;
    }

    store_to_file(output_file_name, k2tree);



    std::cout << "Compressing" << std::endl;

    uint64_t peak_RSS_comp;
    uint64_t peak_VMEM_comp;
    uint64_t construction_time_comp;
    {
        mem_monitor mem_monitor1("comp_mem");
        auto start = timer::now();
        k2tree.compress_leaves(DAC, hash_size);
        auto stop = timer::now();
        auto status = mem_monitor1.get_current_stats();
        peak_RSS_comp = mem_monitor1.max_seen_rss;
        peak_VMEM_comp = mem_monitor1.max_seen_vmem;
        construction_time_comp = duration_cast<milliseconds>(stop - start).count();
    }


    cout << "# k2_size = " << size_in_bytes(k2tree) << endl;
    std::cout << "Speed test with compression" << std::endl;
    access_times times_comp = perform_speed_test(query_file_name, k2tree, query_file_name+".single", use_shortcut);

    std::cout << "Direct recovered: " << times_comp.direct_recovered << std::endl;
    std::cout << "Inverse recovered: " << times_comp.inverse_recovered << std::endl;
    std::cout << "Links recovered: " << times_comp.check_present << std::endl;
    std::cout << "Hereyougo:" << file_name << "\t" << k2tree.get_type_string() <<  "\t" << construction_time_comp << "\t" << peak_RSS_comp << "\t" << peak_VMEM_comp;
    if (use_shortcut){
        //Construction Time	Compressed Size (Byte)	Bpe	Direct Short (ns)	Direct (ns)	Inverse Short (ns)	Inverse (ns)	Check S (ns)	Check (ns)
        std::cout << "\t" << times_comp.direct_short_time <<"\t"<< times_comp.direct_time <<"\t"<< times_comp.inverse_short_time <<"\t"<< times_comp.inverse_time <<"\t"<< times_comp.check_short_time <<"\t"<< times_comp.check_time << std::endl;
	std::cout << "Direct Short recovered: " << times_comp.direct_short_recovered << std::endl;
	std::cout << "Inverse Short recovered: " << times_comp.inverse_short_recovered << std::endl;
    } else {
        //Construction Time	Compressed Size (Byte)	Bpe	Direct (ns)	Inverse (ns)	Check (ns)
        std::cout << "\t" << times_comp.direct_time << "\t" << times_comp.inverse_time << "\t" << times_comp.check_time << std::endl;
    }

    store_to_file(output_file_name+"compressed", k2tree);

}

