#ifndef _PNGMANIP_H_
#define _PNGMANIP_H_


#include "pngHeaders.h"


// Define structs for pixel and bitmap

/**
* @brief A structure representing a pixel with RGBA color components.
*/
struct alignas(4) pixel_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

/**
* @brief A structure representing a bitmap image with pixel data and dimensions.
*/
struct alignas(16) bitmap_t
{
	pixel_t* pixels;
	
	uint16_t width;
	uint16_t height;
	
	uint8_t pixelSize;
	uint8_t pixelDepth;
};





/**
* @brief A class for manipulating PNG images, including encoding and decoding functionalities.
*/
class PNGManip
{
private:
	
	size_t inputFileSize;
	const std::string inputFile, outputFile;

	bitmap_t pngImage;
	pixel_t* pixel;
	
	
	/**
	* @brief Function to insert information into the pixel of the image
	*/
	void encodeToImage();

	/**
	* @brief Returns a pointer to the pixel at the specified row and column in the bitmap.
	*/
	inline pixel_t* pixelAt(bitmap_t*, size_t, size_t);

	/**
	* @brief Saves the PNG image to the specified output file.
	*/
	int savePNGToFile(bitmap_t*);

	/**
	* @brief Function to calculate the ideal dimension for the PNG Image
	*/
	std::pair<size_t, size_t> getDimensions(size_t);

	/**
	* @brief Returns the size of the file at the given path.
	*/
	size_t getFileSize(const char*);

public:
	/**
	* @brief Constructor for PNGManip class.
	*/
	PNGManip(const std::string&, const std::string&);

	~PNGManip() = default;


	/**
	* @brief Encodes the input file into a PNG image and saves it to the output file.
	*/
	void encode();

	/**
	* @brief Decodes the PNG image from the input file and extracts the pixel data.
	*/
	void decode();
};


#endif // !_PNGMANIP_H_
