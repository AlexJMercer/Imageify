
#include "headers.hpp"


void encode_string_2_image(std::vector<std::bitset<8>> byteArray) {
	int array_size = static_cast<int>(byteArray.size());

	std::pair<int, int> dimensions = getMatrixDimensions(array_size);

	std::cout << std::endl << "Dimensions: " << dimensions.first << " x " << dimensions.second << std::endl;
	
	createPNG(dimensions.first, dimensions.second, byteArray);

	// Modify here
}




std::pair<int, int> getMatrixDimensions(int size) {
	
	int column_count = static_cast<int>(std::sqrt(size));
	
	int rowCount = column_count + 1;
	int leftover = column_count * rowCount - size;

	return std::make_pair(rowCount, column_count);
}






void createPNG(int width, int height, std::vector<std::bitset<8>> byteArray) {

	//std::ofstream imageFile;
	//imageFile.open("output.ppm");


	//if (!imageFile.is_open()) {
	//	std::cout << "Error opening image file" << std::endl;
	//	return;
	//}


	//imageFile << "P3" << std::endl;
	//imageFile << width << " " << height << std::endl;
	//imageFile << "255" << std::endl;
	//

	//int counter = 0;
	//for (auto var : byteArray) {

	//	imageFile << var.to_ulong() << " ";
	//	std::cout << var.to_ulong() << " ";

	//		
	//	if ((++counter % 3 == 0) && (counter != 0)) {
	//		imageFile << std::endl;
	//		std::cout << std::endl;
	//	}

	//}
	//
	//std::cout << "PPM file has been successfully written!" << std::endl;

	 
}