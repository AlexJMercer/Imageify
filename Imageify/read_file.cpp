#include "headers.hpp"



std::vector<std::bitset<8>> encode_file_2_binary( std::string fileName ) {

	// Input Filestream for File Handling
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	
	// 1-byte string essentially, and an array to store all bytes.
	uint8_t byte = 0;
	std::vector<std::bitset<8>> bin_buf;

	if (file.is_open()) {

		// Read through entire file
		while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
			// Output binary string
			//std::cout << std::bitset<8>(byte) << " ";

			// Pushing all bytes into an array
			bin_buf.push_back(byte);
		}
		// End of File
		file.close();



		// To print entire array (for debugging) 
		//if (!bin_buf.empty()) {
		//	for (auto var : bin_buf)
		//	{
		//		// For printing in Binary
		//		//std::cout << var << " ";

		//		// For printing in Hexadecimal
		//		//
		//		// NOTE: While decoding, use UTF-8
		//		//
		//		std::cout << std::hex << std::uppercase << var.to_ulong() << " ";
		//	}
		//}
	}
	else
	{
		std::cout << "Error opening file." << std::endl;
	}

	return bin_buf;
}