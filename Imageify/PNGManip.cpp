#include "PNGManip.hpp"

/**
* Private Functions ----------------------------------
*/


std::pair<size_t, size_t> PNGManip::getDimensions(size_t size)
{
	// Find the next closest perfect square
	size_t row = static_cast<size_t>(std::sqrt(size));

	while (size % row)
		--row;

	return std::make_pair(row , size / row);
}


inline pixel_t* PNGManip::pixelAt(bitmap_t* bitmap, size_t row, size_t col)
{
	return bitmap->pixels + (row * bitmap->width + col);
}


int PNGManip::savePNGToFile(bitmap_t* bitmap, const char* output)
{
	// Initialize File Pointer to write to output
	FILE* filePtr = fopen(output, "wb");
	
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
	pixel(),
	pngImage()
{
	pngImage.width = 50;
	pngImage.height = 50;
	pngImage.pixelSize = 4;
	pngImage.pixelDepth = 8;
	pngImage.pixels = static_cast<pixel_t*>(calloc(sizeof(pixel_t), pngImage.width * pngImage.height));
}




void PNGManip::encode()
{
	// Encoding logic goes here
	for (size_t row = 0; row < pngImage.height; ++row)
	{
		for (size_t col = 0; col < pngImage.width; ++col)
		{
			pixel = pixelAt(&pngImage, row, col);
			
			pixel->red = static_cast<uint8_t>((col + 80) % 170);
			pixel->green = static_cast<uint8_t>((col + 11) % 120);
			pixel->blue = static_cast<uint8_t>((row + 30) % 255);
			pixel->alpha = static_cast<uint8_t>((row * 4) % 255);
		}
	}

	int error = savePNGToFile(&pngImage, outputFile.c_str());
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
