
#include "headers.hpp"





std::pair<int, int> getMatrixDimensions(int size) {
	
	int column_count = static_cast<int>(std::sqrt(size));
	
	int rowCount = column_count + 1;
	int leftover = column_count * rowCount - size;

	return std::make_pair(rowCount, column_count);
}


void encode_string_2_image(std::vector<std::bitset<8>> byteArray) {
	int array_size = static_cast<int>(byteArray.size());

	std::pair<int, int> dimensions = getMatrixDimensions(array_size);

	std::cout << std::endl << "Dimensions: " << dimensions.first << " x " << dimensions.second << std::endl;
	
	createPNG(dimensions.first, dimensions.second, byteArray);
}



void createPNG(int width, int height, std::vector<std::bitset<8>> byteArray) {

	cv::Mat imageData;

	imageData = cv::Mat(width, height, CV_8UC3, cv::Scalar(255, 0, 255));

	std::string window = "Image Display";
	cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
	cv::imshow(window, imageData);

	cv::waitKey(0);

	cv::destroyAllWindows();
}