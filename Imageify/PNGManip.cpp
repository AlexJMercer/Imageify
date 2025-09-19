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



void PNGManip::decodeImage()
{
	FILE* filePtr = fopen(inputFile.c_str(), "rb");

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	
	if (!png_ptr)
	{
		fclose(filePtr);
		std::cout << "\nERROR: Cannot read PNG image.\n";
		return;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(filePtr);
		std::cout << "\nERROR: Cannot read PNG image.\n";
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(filePtr);
		std::cout << "\nERROR: Cannot read PNG image.\n";
		return;
	}

	png_init_io(png_ptr, filePtr);
	png_read_info(png_ptr, info_ptr);

	pngImage.width		= static_cast<uint16_t>( png_get_image_width(png_ptr, info_ptr) );
	pngImage.height		= static_cast<uint16_t>( png_get_image_height(png_ptr, info_ptr) );
	pngImage.pixelDepth = static_cast<uint8_t>( png_get_bit_depth(png_ptr, info_ptr) );
	pngImage.pixelSize	= 4;
	pngImage.pixels = static_cast<pixel_t*>(
		calloc(sizeof(pixel_t), 
			static_cast<size_t>(pngImage.width * pngImage.height)
		)
	);

	std::cout << "\n[INFO] Image Dimensions:\n"
		<< "\nWidth:\t" << pngImage.width
		<< "\nHeight:\t" << pngImage.height 
		<< "\nDepth:\t" << pngImage.pixelDepth 
		<< "\nPixel Size:\t" << pngImage.pixelSize 
		<< std::endl;

	png_byte** row_pointers = static_cast<png_byte**>(
		png_malloc(png_ptr, pngImage.height * sizeof(png_byte*))
	);
	for (size_t y = 0; y < pngImage.height; ++y)
	{
		row_pointers[y] = static_cast<png_byte*>(
			png_malloc(png_ptr, sizeof(uint8_t) * pngImage.width * pngImage.pixelSize)
		);
	}

	png_read_image(png_ptr, row_pointers);

	for (size_t x = 0; x < pngImage.height; ++x)
	{
		png_bytep row = row_pointers[ x ];
		for (size_t y = 0; y < pngImage.width; ++y)
		{
			pixel = pixelAt(&pngImage, y, x);
			pixel->red		= *row++;
			pixel->green	= *row++;
			pixel->blue		= *row++;
			pixel->alpha	= *row++;
		}
	}

	for (size_t y = 0; y < pngImage.height; y++)
		png_free(png_ptr, row_pointers[y]);
	png_free(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	fclose(filePtr);
}



int PNGManip::saveDecodedPNGInfo()
{
    FILE* outputFilePtr = fopen(outputFile.c_str(), "wb");
    if (!outputFilePtr)
    {	
        std::cout << "\nERROR: Cannot open output file.\n";
        return -1;
    }

    std::string outputStr;

    for (size_t row = 0; row < pngImage.height; ++row)
    {
        for (size_t col = 0; col < pngImage.width; ++col)
        {
            pixel = pixelAt(&pngImage, row, col);
            if (pixel->red != 0x1D)
                outputStr.push_back(static_cast<char>(pixel->red));
            else
                break;
            
			if (pixel->green != 0x1D)
                outputStr.push_back(static_cast<char>(pixel->green));
            else
                break;
            
			if (pixel->blue != 0x1D)
                outputStr.push_back(static_cast<char>(pixel->blue));
            else
                break;
            
			if (pixel->alpha != 0x1D)
                outputStr.push_back(static_cast<char>(pixel->alpha));
            else
                break;
        }
    }

    // Write to output file
    fwrite(outputStr.data(), 1, outputStr.size(), outputFilePtr);

    // Display in terminal
    std::cout << "\nDecoded Output:\n" << outputStr << std::endl;

    fclose(outputFilePtr);
    return 0;
}



inline pixel_t* PNGManip::pixelAt(bitmap_t* bitmap, size_t row, size_t col)
{
	return bitmap->pixels + (row * bitmap->width + col);
}


int PNGManip::savePNGToFile()
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
		pngImage.width,
		pngImage.height,
		pngImage.pixelDepth,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	row_pointers = static_cast<png_byte**>(png_malloc(png_ptr, pngImage.height * sizeof(png_byte*)));

	for (size_t y = 0; y < pngImage.height; ++y)
	{
		png_byte* row = static_cast<png_byte*>(png_malloc(png_ptr, sizeof(uint8_t) * pngImage.width * pngImage.pixelSize));

		row_pointers[ y ] = row;

		for (size_t x = 0; x < pngImage.width; ++x)
		{
			pixel = pixelAt(&pngImage, x, y);

			*row++ = pixel->red;
			*row++ = pixel->green;
			*row++ = pixel->blue;
			*row++ = pixel->alpha;
		}
	}


	png_init_io(png_ptr, filePtr);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	for (size_t y = 0; y < pngImage.height; y++)
		png_free(png_ptr, row_pointers[ y ]);

	png_free(png_ptr, row_pointers);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(filePtr);

	return 0;
}



void PNGManip::encode()
{
	start = std::chrono::high_resolution_clock::now();

	encodeToImage();

	end = std::chrono::high_resolution_clock::now();

	int error = savePNGToFile();
	if (error)
		std::cout << "\nERROR: Cannot encode image.\n";
	else
		std::cout << "\nImage encoded successfully !\n";

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "Encoding process took: " << duration.count() << " microseconds\n";
}



void PNGManip::decode()
{
	start = std::chrono::high_resolution_clock::now();

	decodeImage();

	end = std::chrono::high_resolution_clock::now();

	int error = saveDecodedPNGInfo();
	if (error)
		std::cout << "\nERROR: Unable to decode image.\n";
	else
		std::cout << "\nImage decoded successfully !\n";

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "Decoding process took: " << duration.count() << " microseconds\n";
}







/**
* Public Functions -----------------------------------
*/


/**
* @brief Constructor for the PNGManip class.
*/
PNGManip::PNGManip(const std::string& type, const std::string& input, const std::string& output) : 
	processType(type),
	inputFile(input), 
	outputFile(output),
	pixel(),
	pngImage()
{
	if (processType == "ENCODE")
	{
		// Input file should be a text file

		// Get dimensions
		auto dimensions = getDimensions( getFileSize(inputFile.c_str()) );
		pngImage.width = static_cast<uint16_t>(dimensions.first);
		pngImage.height = static_cast<uint16_t>(dimensions.second);

		std::cout << "Resultant Image Dimensions: " << pngImage.width << " x " << pngImage.height << std::endl;

		pngImage.pixelSize = 4;
		pngImage.pixelDepth = 8;
		pngImage.pixels = static_cast<pixel_t*>(
			calloc(sizeof(pixel_t), 
				static_cast<size_t>(pngImage.width * pngImage.height)
			)
		);

	}
	else if (processType == "DECODE")
	{
		// Do nothing here
	}
	else
	{
		std::cout << "\nERROR: Invalid process type. Use 'ENCODE' or 'DECODE'.\n";
	}
}



PNGManip::~PNGManip()
{
	if (pngImage.pixels)
		free(pngImage.pixels);
}



void PNGManip::startProcess()
{
	if (processType == "ENCODE")
		encode();
	
	else if (processType == "DECODE")
		decode();
	
	else
		std::cout << "\nERROR: Invalid process type. Use 'ENCODE' or 'DECODE'.\n";
}
