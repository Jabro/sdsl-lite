#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <complex>
#include <sdsl/bit_vectors.hpp>
#include <c++/4.9.3/bitset>

using namespace sdsl;
using namespace std;

static const unsigned short MortonTable_2bitwise_256_[256] =
        {
                0x0000, 0x0004, 0x0008, 0x000c, 0x0040, 0x0044, 0x0048, 0x004c, 0x0080, 0x0084, 0x0088, 0x008c, 0x00c0, 0x00c4, 0x00c8, 0x00cc,
                0x0400, 0x0404, 0x0408, 0x040c, 0x0440, 0x0444, 0x0448, 0x044c, 0x0480, 0x0484, 0x0488, 0x048c, 0x04c0, 0x04c4, 0x04c8, 0x04cc,
                0x0800, 0x0804, 0x0808, 0x080c, 0x0840, 0x0844, 0x0848, 0x084c, 0x0880, 0x0884, 0x0888, 0x088c, 0x08c0, 0x08c4, 0x08c8, 0x08cc,
                0x0c00, 0x0c04, 0x0c08, 0x0c0c, 0x0c40, 0x0c44, 0x0c48, 0x0c4c, 0x0c80, 0x0c84, 0x0c88, 0x0c8c, 0x0cc0, 0x0cc4, 0x0cc8, 0x0ccc,
                0x4000, 0x4004, 0x4008, 0x400c, 0x4040, 0x4044, 0x4048, 0x404c, 0x4080, 0x4084, 0x4088, 0x408c, 0x40c0, 0x40c4, 0x40c8, 0x40cc,
                0x4400, 0x4404, 0x4408, 0x440c, 0x4440, 0x4444, 0x4448, 0x444c, 0x4480, 0x4484, 0x4488, 0x448c, 0x44c0, 0x44c4, 0x44c8, 0x44cc,
                0x4800, 0x4804, 0x4808, 0x480c, 0x4840, 0x4844, 0x4848, 0x484c, 0x4880, 0x4884, 0x4888, 0x488c, 0x48c0, 0x48c4, 0x48c8, 0x48cc,
                0x4c00, 0x4c04, 0x4c08, 0x4c0c, 0x4c40, 0x4c44, 0x4c48, 0x4c4c, 0x4c80, 0x4c84, 0x4c88, 0x4c8c, 0x4cc0, 0x4cc4, 0x4cc8, 0x4ccc,
                0x8000, 0x8004, 0x8008, 0x800c, 0x8040, 0x8044, 0x8048, 0x804c, 0x8080, 0x8084, 0x8088, 0x808c, 0x80c0, 0x80c4, 0x80c8, 0x80cc,
                0x8400, 0x8404, 0x8408, 0x840c, 0x8440, 0x8444, 0x8448, 0x844c, 0x8480, 0x8484, 0x8488, 0x848c, 0x84c0, 0x84c4, 0x84c8, 0x84cc,
                0x8800, 0x8804, 0x8808, 0x880c, 0x8840, 0x8844, 0x8848, 0x884c, 0x8880, 0x8884, 0x8888, 0x888c, 0x88c0, 0x88c4, 0x88c8, 0x88cc,
                0x8c00, 0x8c04, 0x8c08, 0x8c0c, 0x8c40, 0x8c44, 0x8c48, 0x8c4c, 0x8c80, 0x8c84, 0x8c88, 0x8c8c, 0x8cc0, 0x8cc4, 0x8cc8, 0x8ccc,
                0xc000, 0xc004, 0xc008, 0xc00c, 0xc040, 0xc044, 0xc048, 0xc04c, 0xc080, 0xc084, 0xc088, 0xc08c, 0xc0c0, 0xc0c4, 0xc0c8, 0xc0cc,
                0xc400, 0xc404, 0xc408, 0xc40c, 0xc440, 0xc444, 0xc448, 0xc44c, 0xc480, 0xc484, 0xc488, 0xc48c, 0xc4c0, 0xc4c4, 0xc4c8, 0xc4cc,
                0xc800, 0xc804, 0xc808, 0xc80c, 0xc840, 0xc844, 0xc848, 0xc84c, 0xc880, 0xc884, 0xc888, 0xc88c, 0xc8c0, 0xc8c4, 0xc8c8, 0xc8cc,
                0xcc00, 0xcc04, 0xcc08, 0xcc0c, 0xcc40, 0xcc44, 0xcc48, 0xcc4c, 0xcc80, 0xcc84, 0xcc88, 0xcc8c, 0xccc0, 0xccc4, 0xccc8, 0xcccc
        };

static const unsigned short MortonTable256[256] =
        {
                0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015,
                0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055,
                0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115,
                0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155,
                0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415,
                0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455,
                0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515,
                0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555,
                0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015,
                0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
                0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115,
                0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155,
                0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415,
                0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455,
                0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515,
                0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555,
                0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015,
                0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055,
                0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115,
                0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
                0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415,
                0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455,
                0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515,
                0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555,
                0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015,
                0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055,
                0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115,
                0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155,
                0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415,
                0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
                0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515,
                0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555
        };


int main()
{
/*
    uint x = 65535;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x << 16)) & 0xc0003FFF;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x << 14)) & 0xfc000FFF;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  12)) & 0xffc003FF;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  10)) & 0xfffc00FF;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  8)) & 0xffffc03F;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  6)) & 0xcccccc0f;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  4)) & 0xccccccc3;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  2)) & 0xcccccccc;
    std::cout <<  std::bitset<32>(x) << std::endl;

*/
    uint x =  65535;
    uint y =  0;

    std::cout <<  std::bitset<32>(MortonTable_2bitwise_256_[y >> 8] << 16) << std::endl;
    std::cout <<  std::bitset<32>(MortonTable_2bitwise_256_[x >> 8] << 14) << std::endl;
    std::cout <<  std::bitset<32>(MortonTable_2bitwise_256_[y & 0xFF]) << std::endl;
    std::cout <<  std::bitset<32>(MortonTable_2bitwise_256_[x & 0xFF] >> 2) << std::endl;

    uint z = MortonTable_2bitwise_256_[y >> 8] << 16 |
             MortonTable_2bitwise_256_[x >> 8] << 14 |
             MortonTable_2bitwise_256_[y & 0xFF] |
             MortonTable_2bitwise_256_[x & 0xFF] >> 2;

    std::cout <<  std::bitset<32>(z) << std::endl;


    uint z2 = MortonTable256[y >> 8] << 17 |
             MortonTable256[x >> 8] << 16 |
             MortonTable256[y & 0xFF] << 1 |
             MortonTable256[x & 0xFF];

    std::cout <<  std::bitset<32>(z2) << std::endl;




    uint z3 = (y << 16) & 0xF0000000 |
              (x << 12) & 0x0F000000 |
              (y << 12)  & 0x00F00000 |
              (x << 8) & 0x000F0000 |
              (y << 8)  & 0x0000F000 |
              (x << 4)  & 0x00000F00 |
              (y << 4)  & 0x000000F0 |
              (x)  & 0x0000000F;

    std::cout <<  std::bitset<32>(z3) << std::endl;
/*
 *
    x = (x | (x <<  4)) & 0x030C30C3;
    std::cout <<  std::bitset<32>(x) << std::endl;

    x = (x | (x <<  2)) & 0x09249249;
    std::cout <<  std::bitset<32>(x) << std::endl;
*/
    //std::vector<pair<uint64_t, uint64_t>> points = {{770,775},{39683,55811}};

    /*std::vector<pair<uint, uint>> points;
    int size = 8;
    const int k = 2;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++) {
            points.push_back(std::make_pair(i,j));
        }
    }

    k2_treap<k, bit_vector> tree;
    tree.set_height(log(size)/log(k));

    std::sort(points.begin(), points.end(), [&](const std::pair<uint32_t, uint32_t>& lhs, const std::pair<uint32_t, uint32_t>& rhs){
        return tree.sort_by_z_order(lhs, rhs);
    });

    vector<pair<uint32_t, uint32_t>> correct_order = {{0,0},{0,1},{1,0},{1,1},{0,2},{0,3},{1,2},{1,3},
                                                      {2,0},{2,1},{3,0},{3,1},{2,2},{2,3},{3,2},{3,3},
                                                      {0,4},{0,5},{1,4},{1,5},{0,6},{0,7},{1,6},{1,7},
                                                      {2,4},{2,5},{3,4},{3,5},{2,6},{2,7},{3,6},{3,7},
                                                      {4,0},{4,1},{5,0},{5,1},{4,2},{4,3},{5,2},{5,3},
                                                      {6,0},{6,1},{7,0},{7,1},{6,2},{6,3},{7,2},{7,3},
                                                      {4,4},{4,5},{5,4},{5,5},{4,6},{4,7},{5,6},{5,7},
                                                      {6,4},{6,5},{7,4},{7,5},{6,6},{6,7},{7,6},{7,7},
    };
    */
    /*int size = 8;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++) {
            points.push_back(std::make_pair(i,j));
        }
    }*/

    //std::sort(points.begin(), points.end(), sort_by_z_order());

    /*std::cout << "Sorted points" << std::endl;
    for (uint i = 0; i < points.size(); ++i) {
        std::cout << points[i].first << "," << points[i].second << "\t";
    }

    std::cout << std::endl;*/
}