#pragma once



#include <vector>
#include <bitset>



std::vector<std::bitset<8>> encode_file_2_binary( std::string fileName ); 

std::pair<int, int> getMatrixDimensions(int size);

void encode_string_2_image( std::vector<std::bitset<8>> byteArray );

void createPNG(int width, int height, std::vector<std::bitset<8>> byteArray);
