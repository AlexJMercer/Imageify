#include "PNGManip.hpp"

/**
* Private Functions ----------------------------------
*/

void PNGManip::encodeToImage()
{
	// Encoding logic goes here
	FILE* inputFilePtr = fopen(inputFile.c_str(), "rb");
	if (!inputFilePtr)
	{
		std::cout << "\nERROR: Cannot open input file.\n";
		return;
	}

	auto readByte = [&]() -> uint8_t {
		int byte = fgetc(inputFilePtr);
		return (byte == EOF) ? 0x1D : static_cast<uint8_t>(byte);
	};

	for (size_t row = 0; row < pngImage.height; ++row)
	{
		for (size_t col = 0; col < pngImage.width; ++col)
		{
			pixel			=	pixelAt(&pngImage, row, col);

			pixel->red		=	readByte();
			pixel->green	=	readByte();
			pixel->blue		=	readByte();
			pixel->alpha	=	readByte();

			//std::cout << pixel->red << " " << pixel->green << " " << pixel->blue << " " << pixel->alpha << "\n";
		}
	}

	//std::cout << "Closing file\n";
	fclose(inputFilePtr);
}



std::pair<size_t, size_t> PNGManip::getDimensions(size_t size)
{
	// Find the next closest perfect square
	size_t dimension = size / 4;

	if (size % 4)
		dimension += 1;

	// Make it even
	if (dimension % 2)
		dimension++;

	size_t row = static_cast<size_t>(std::sqrt(dimension));

	if (row * row < dimension)
		row++;

	size_t column = (dimension + row - 1) / row;

	return std::make_pair(row, column);
}



size_t PNGManip::getFileSize(const char* filePath)
{
	FILE* file = fopen(filePath, "rb");
	
	if (!file)
		return 0;
	
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);

	//std::cout << "Size: " << size << std::endl;
	
	fclose(file);
	
	return size;
}



inline pixel_t* PNGManip::pixelAt(bitmap_t* bitmap, size_t row, size_t col)
{
	return bitmap->pixels + (row * bitmap->width + col);
}


int PNGManip::savePNGToFile(bitmap_t* bitmap)
{
	// Initialize File Pointer to write to output
	FILE* filePtr = fopen(outputFile.c_str(), "wb");
	
	if (!filePtr)
		return -1;


	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	png_byte** row_pointers = nullptr;


	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr)
	{
		fclose(filePtr);
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(filePtr);
		return -1;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(filePtr);
		return -1;
	}


	png_set_IHDR(
		png_ptr,
		info_ptr,
		bitmap->width,
		bitmap->height,
		bitmap->pixelDepth,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);


	row_pointers = static_cast<png_byte**>(png_malloc(png_ptr, bitmap->height * sizeof(png_byte*)));

	for (size_t y = 0; y < bitmap->height; ++y)
	{
		png_byte* row = static_cast<png_byte*>(png_malloc(png_ptr, sizeof(uint8_t) * bitmap->width * bitmap->pixelSize));

		row_pointers[ y ] = row;

		for (size_t x = 0; x < bitmap->width; ++x)
		{
			pixel = pixelAt(bitmap, x, y);

			*row++ = pixel->red;
			*row++ = pixel->green;
			*row++ = pixel->blue;
			*row++ = pixel->alpha;
		}
	}


	png_init_io(png_ptr, filePtr);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	for (size_t y = 0; y < bitmap->height; y++)
		png_free(png_ptr, row_pointers[ y ]);

	png_free(png_ptr, row_pointers);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(filePtr);

	return 0;
}





/**
* Public Functions -----------------------------------
*/


/**
* @brief Constructor for the PNGManip class.
*/
PNGManip::PNGManip(const std::string& input, const std::string& output) : 
	inputFile(input), 
	outputFile(output),
	inputFileSize( getFileSize(input.c_str()) ),
	pixel(),
	pngImage()
{
	auto dimensions = getDimensions( inputFileSize );
	pngImage.width = static_cast<uint16_t>(dimensions.first);
	pngImage.height = static_cast<uint16_t>(dimensions.second);

	std::cout << "Dimensions: " << pngImage.width << "*" << pngImage.height << std::endl;

	pngImage.pixelSize = 4;
	pngImage.pixelDepth = 8;
	pngImage.pixels = static_cast<pixel_t*>(
		calloc(sizeof(pixel_t), 
		static_cast<size_t>(pngImage.width * pngImage.height)
		)
	);
}




void PNGManip::encode()
{
	encodeToImage();

	int error = savePNGToFile(&pngImage);
	if (error)
		std::cout << "\nERROR: Cannot encode image.\n";
	else
		std::cout << "\nImage encoded successfully !\n";

	free(pngImage.pixels);
}



void PNGManip::decode()
{
	std::cout << "\nDecoding image...\n";
}
