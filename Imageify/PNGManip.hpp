#ifndef _PNGMANIP_H_
#define _PNGMANIP_H_


#include "pngHeaders.h"
#include "ErrorHandling.hpp"


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
	uint16_t width;
	uint16_t height;
	
	png_byte pixelSize;
	png_byte pixelDepth;
	
	std::vector<pixel_t> pixels;
};





/**
* @brief A class for manipulating PNG images, including encoding and decoding functionalities.
*/
class PNGManip
{
private:
	
	const std::string processType, inputFile, outputFile, terminalOutput;

	bitmap_t pngImage;
	pixel_t* pixel;

	// For Timing
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	
	
	/**
	* @brief Function to insert information into the pixel of the image
	*/
	PNGManipErrorCode encodeToImage();

	/**
	* @brief Function to decode image, i.e., extract information from the pixel of the image
	*/
	PNGManipErrorCode decodeImage();

	/**
	* @brief Returns a pointer to the pixel at the specified row and column in the bitmap.
	*/
	inline pixel_t* pixelAt(bitmap_t*, size_t, size_t);

	/**
	* @brief Saves the PNG image to the specified output file.
	*/
	PNGManipErrorCode savePNGToFile();

	/**
	* @brief Function to calculate the ideal dimension for the PNG Image
	*/
	std::pair<size_t, size_t> getDimensions(size_t);

	/**
	* @brief Returns the size of the file at the given path.
	*/
	size_t getFileSize(const char*);

	/**
	* @brief Returns the file extension of the given filename
	*/
	std::string getFileExtension(const std::string&);

	/**
	* @brief Reads and retrieves details from the PNG image file.
	*/
	PNGManipErrorCode saveDecodedPNGInfo();

	/**
	* @brief Encodes the input file into a PNG image and saves it to the output file.
	*/
	void encode();

	/**
	* @brief Decodes the PNG image from the input file and extracts the pixel data.
	*/
	void decode();

	/**
	* @brief Validates the input file for existence and readability.
	*/
	PNGManipErrorCode validateInputFile() const;
	PNGManipErrorCode validateOutputFile() const;

public:
	/**
	* @brief Constructor for PNGManip class.
	*/
	PNGManip(const std::string&, const std::string&, const std::string&, const std::string&);
	~PNGManip() = default;

	void startProcess();
};

#endif // !_PNGMANIP_H_
