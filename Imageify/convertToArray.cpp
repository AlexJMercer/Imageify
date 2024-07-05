
#include "headers.h"



std::pair<size_t, size_t> getDimensions(size_t size)
{
	size_t column_count = static_cast<size_t>(std::sqrt(size));

	size_t rowCount = column_count + 1;
	size_t leftover = column_count * rowCount - size;

	return std::make_pair(rowCount, column_count);
}



std::vector<short> convertFileToArray(std::string filepath)
{
	std::vector<short> pixelArray;

	// First validate and separate filename from filepath if required
	std::string fileName;
	short byte = 0;

	fileName = filepath;	// Temporary


	std::ifstream File(filepath, std::ios::in | std::ios::binary);
	
	// Output to console first
	if (File.is_open())
	{
		while (File.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
		{
			/*
			 * The Integer values will always be a minimum of 32 (which is SPACE)
			 * And so, we can apply an OFFSET of 30.
			 * This acts as a simple Caeser Cipher to not immediately spit out
			 * the correct text information.
			*/

			//std::cout << (short)byte << std::endl;

			// Push to array
			pixelArray.push_back( static_cast<short>(byte) - OFFSET_CIPHER);
		}
	}
	else
	{
		std::cout << "Error opening file. \n";
		return {};
	}


	return pixelArray;
}