#include "headers.hpp"


int main()
{
	// Input File
	std::string fileName;
	char output;
	// Resultant array of bytes after encoding
	std::vector<std::bitset<8>> bin_array;
	

	std::cout << "Please enter the relative path of the file you want to read: \n";
	std::cin >> fileName;
	
	std::cout << "\nDo you want output to be displayed (y/n): ";
	std::cin >> output;

	bin_array = encode_file_2_binary(fileName);

	if ( bin_array.empty() )
		std::cerr << "Error while opening selected file! Please check again.";

	if (output == 'y') {
		for (auto var : bin_array)
		{
			// For printing in Binary
			std::cout << var << " ";
		}
		std::cout << std::endl;
	}

	encode_string_2_image(bin_array);
	

	return 0;
}