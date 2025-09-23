#include "PNGManip.hpp"
#include "ErrorHandling.hpp"



// Validate input file
PNGManipErrorCode PNGManip::validateInputFile() const 
{
    std::ifstream file(inputFile, std::ios::binary);
    
    if (!file.is_open()) 
    {
        logError("Input file not found: " + inputFile);
        return PNGManipErrorCode::FileNotFound;
    }
    
    if (!file.good()) {
        logError("Input file not readable: " + inputFile);
        return PNGManipErrorCode::FileNotReadable;
    }
    
    return PNGManipErrorCode::Success;
}



// Validate output file
PNGManipErrorCode PNGManip::validateOutputFile() const 
{
    std::ofstream file(outputFile, std::ios::binary | std::ios::app);

    if (!file.is_open()) {
        logError("Output file not writable: " + outputFile);
        return PNGManipErrorCode::FileNotWritable;
    }

    return PNGManipErrorCode::Success;
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
    fclose(file);
    return size;
}



std::string PNGManip::getFileExtension(const std::string& fileName)
{
    size_t dotPos = fileName.find_last_of('.');

    if (dotPos == std::string::npos)
        return "";

	return fileName.substr(dotPos + 1);
}




PNGManipErrorCode PNGManip::encodeToImage() 
{
    FILE* inputFilePtr = fopen(inputFile.c_str(), "rb");
    if (!inputFilePtr) {
        logError("Cannot open input file: " + inputFile);
        return PNGManipErrorCode::FileNotFound;
    }

    size_t fileSize = getFileSize(inputFile.c_str());
    size_t totalBytes = fileSize * pngImage.pixelSize;

    std::vector<uint8_t> buffer(totalBytes); // Allocate and size the buffer

    size_t bytesRead = fread(buffer.data(), 1, totalBytes, inputFilePtr);

    fclose(inputFilePtr);

    size_t idx = 0;
    for (size_t row = 0; row < pngImage.height; ++row) 
    {
        for (size_t col = 0; col < pngImage.width; ++col) 
        {
            pixel = pixelAt(&pngImage, row, col);

            pixel->red      = (idx < bytesRead) ? buffer[idx++] : 0x81;
            pixel->green    = (idx < bytesRead) ? buffer[idx++] : 0x81;
            pixel->blue     = (idx < bytesRead) ? buffer[idx++] : 0x81;
            pixel->alpha    = (idx < bytesRead) ? buffer[idx++] : 0x81;
        }
    }

    return PNGManipErrorCode::Success;
}




PNGManipErrorCode PNGManip::decodeImage() 
{
    FILE* filePtr = fopen(inputFile.c_str(), "rb");
    if (!filePtr) {
        logError("Cannot open input file: " + inputFile);
        return PNGManipErrorCode::FileNotFound;
    }

    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) 
    {
        fclose(filePtr);
        logError("Cannot read PNG image (struct creation failed).");
        return PNGManipErrorCode::DecodingError;
    }

    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(filePtr);
        logError("Cannot read PNG image (info struct failed).");
        return PNGManipErrorCode::DecodingError;
    }

    
    if (setjmp(png_jmpbuf(png_ptr))) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(filePtr);
        logError("PNG read error (setjmp).");
        return PNGManipErrorCode::DecodingError;
    }
    
    
    png_init_io(png_ptr, filePtr);
    png_read_info(png_ptr, info_ptr);
    
    
    pngImage.width      = static_cast<uint16_t>(png_get_image_width(png_ptr, info_ptr));
    pngImage.height     = static_cast<uint16_t>(png_get_image_height(png_ptr, info_ptr));
    pngImage.pixelDepth = static_cast<uint8_t>(png_get_bit_depth(png_ptr, info_ptr));
    pngImage.pixelSize  = 4;
    pngImage.pixels = static_cast<pixel_t*>(
        calloc(sizeof(pixel_t), static_cast<size_t>(pngImage.width * pngImage.height))
    );
    
    if (!pngImage.pixels) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(filePtr);
        logError("Memory allocation error for PNG pixels.");
        return PNGManipErrorCode::MemoryAllocationError;
    }
    
    std::cout << "\033[36m[INFO] Image Dimensions:"
        << "\nWidth:\t" << pngImage.width
        << "\nHeight:\t" << pngImage.height
        << "\nDepth:\t" << pngImage.pixelDepth
        << "\nPixel Size:\t" << pngImage.pixelSize
        << "\033[0m\n";
    
    
    png_byte** row_pointers = static_cast<png_byte**>(
        png_malloc(png_ptr, pngImage.height * sizeof(png_byte*))
    );
    for (size_t y = 0; y < pngImage.height; ++y)
        row_pointers[y] = static_cast<png_byte*>(png_malloc(png_ptr, sizeof(uint8_t) * pngImage.width * pngImage.pixelSize));
    
    
    png_read_image(png_ptr, row_pointers);
    
    for (size_t x = 0; x < pngImage.height; ++x) 
    {
        png_bytep row = row_pointers[x];
        for (size_t y = 0; y < pngImage.width; ++y) 
        {
            pixel = pixelAt(&pngImage, y, x);
            pixel->red   = *row++;
            pixel->green = *row++;
            pixel->blue  = *row++;
            pixel->alpha = *row++;
        }
    }


    for (size_t y = 0; y < pngImage.height; y++)
        png_free(png_ptr, row_pointers[y]);
    png_free(png_ptr, row_pointers);
    
    
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(filePtr);
    
    
    return PNGManipErrorCode::Success;
}




PNGManipErrorCode PNGManip::savePNGToFile() 
{
    FILE* filePtr = fopen(outputFile.c_str(), "wb");
    if (!filePtr) 
    {
        logError("Cannot open output file: " + outputFile);
        return PNGManipErrorCode::FileNotWritable;
    }
    
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) 
    {
        fclose(filePtr);
        logError("Cannot create PNG write struct.");
        return PNGManipErrorCode::EncodingError;
    }
    
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(filePtr);
        logError("Cannot create PNG info struct.");
        return PNGManipErrorCode::EncodingError;
    }
    
    
    if (setjmp(png_jmpbuf(png_ptr))) 
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(filePtr);
        logError("PNG write error (setjmp).");
        return PNGManipErrorCode::EncodingError;
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
    
    
    png_byte** row_pointers = static_cast<png_byte**>(
        png_malloc(png_ptr, pngImage.height * sizeof(png_byte*))
    );
    
    
    for (size_t y = 0; y < pngImage.height; ++y) 
    {
        png_byte* row = static_cast<png_byte*>(png_malloc(png_ptr, sizeof(uint8_t) * pngImage.width * pngImage.pixelSize));
        row_pointers[y] = row;
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
        png_free(png_ptr, row_pointers[y]);
    png_free(png_ptr, row_pointers);
    
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(filePtr);
    
    
    return PNGManipErrorCode::Success;
}



PNGManipErrorCode PNGManip::saveDecodedPNGInfo()
{
    FILE* outputFilePtr = fopen(outputFile.c_str(), "wb");
    if (!outputFilePtr) 
    {
        logError("Cannot open output file: " + outputFile);
        return PNGManipErrorCode::FileNotWritable;
    }

    std::string outputStr;
	
    if (getFileExtension(outputFile) == "txt")
    {
        for (size_t row = 0; row < pngImage.height; ++row)
        {
            for (size_t col = 0; col < pngImage.width; ++col)
            {
                pixel = pixelAt(&pngImage, row, col);

                if (pixel->red != 0x81)
                    outputStr.push_back(static_cast<char>(pixel->red));
                else
                    break;

                if (pixel->green != 0x81)
                    outputStr.push_back(static_cast<char>(pixel->green));
                else
                    break;

                if (pixel->blue != 0x81)
                    outputStr.push_back(static_cast<char>(pixel->blue));
                else
                    break;

                if (pixel->alpha != 0x81)
                    outputStr.push_back(static_cast<char>(pixel->alpha));
                else
                    break;
            }
        }

        fwrite(outputStr.data(), 1, outputStr.size(), outputFilePtr);
		fclose(outputFilePtr);

    }
    else if (getFileExtension(outputFile) == "pdf" )
    {
        for (size_t row = 0; row < pngImage.height; ++row)
        {
            for (size_t col = 0; col < pngImage.width; ++col)
            {
                pixel = pixelAt(&pngImage, row, col);

                outputStr.push_back(static_cast<char>(pixel->red));
                outputStr.push_back(static_cast<char>(pixel->green));
                outputStr.push_back(static_cast<char>(pixel->blue));
                outputStr.push_back(static_cast<char>(pixel->alpha));
            }
        }

        fwrite(outputStr.data(), 1, outputStr.size(), outputFilePtr);
		fclose(outputFilePtr);

    }/*
    else if ( getFileExtension(outputFile) == "docx" )
    {
        std::vector<char> docxBuffer;

        for (size_t row = 0; row < pngImage.height; ++row)
        {
            for (size_t col = 0; col < pngImage.width; ++col)
            {
                pixel = pixelAt(&pngImage, row, col);

                docxBuffer.push_back(static_cast<char>(pixel->red));
                docxBuffer.push_back(static_cast<char>(pixel->green));
                docxBuffer.push_back(static_cast<char>(pixel->blue));
                docxBuffer.push_back(static_cast<char>(pixel->alpha));
            }
        }


        auto findEOCD = [&](const std::vector<char>& buffer) -> size_t {
            std::vector<char> eocdSignature = { 0x50, 0x4b, 0x05, 0x06 };

            for (size_t i = buffer.size() > 22 ? buffer.size() - 22 : 0; i < buffer.size() - 3; ++i)
            {
                if (std::equal(eocdSignature.begin(), eocdSignature.end(), buffer.begin() + i)) {
                    return i + 22;
                }
            }
            return 0;
        };


		// Print last 50 bytes from docxBuffer for debugging
		if (docxBuffer.size() >= 50) {
			std::cout << "\n\033[33m[DEBUG] Last 50 bytes of decoded DOCX data:\n";
            for (size_t i = docxBuffer.size() - 50; i < docxBuffer.size(); ++i) {
                printf("%02X ", static_cast<unsigned char>(docxBuffer[i]));
			}
			std::cout << "\n\033[0m";
        }
		//fwrite(docxBuffer.data(), 1, docxBuffer.size(), outputFilePtr);


   //     size_t endPos = findEOCD(docxBuffer);

   //     if (!endPos)
			//endPos = docxBuffer.size();

   //     fwrite(docxBuffer.data(), 1, endPos, outputFilePtr);
		fclose(outputFilePtr);
    }*/
    else
    {
        logError("Unsupported output file format. Please use .txt or .pdf extension.");
        fclose(outputFilePtr);
        return PNGManipErrorCode::FileNotWritable;
	}
    
    
    
    if (terminalOutput == "TRUE")
        std::cout << "\nDecoded Output:\n" << outputStr << std::endl;
    
    
    return PNGManipErrorCode::Success;
}



inline pixel_t* PNGManip::pixelAt(bitmap_t* bitmap, size_t row, size_t col)
{
    return bitmap->pixels + (row * bitmap->width + col);
}



void PNGManip::encode() 
{
    if (validateInputFile() != PNGManipErrorCode::Success) 
        return;

    start = std::chrono::high_resolution_clock::now();
    
    if (encodeToImage() != PNGManipErrorCode::Success) 
        return;
    
    if (savePNGToFile() != PNGManipErrorCode::Success) 
        return;
    
    end = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n\033[32m" << "Image encoded successfully!" << "\033[0m\n";
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	if (duration.count() > 1'000'000)
        std::cout << "\nEncoding process took: " << "\033[36m" << static_cast<float>(duration.count() / 1'000'000) << " seconds\033[0m\n";
	else if (duration.count() > 1'000)
        std::cout << "\nEncoding process took: " << "\033[36m" << static_cast<float>(duration.count() / 1'000) << " milliseconds\033[0m\n";
    else
        std::cout << "\nEncoding process took: " << "\033[36m" << duration.count() << " microseconds\033[0m\n";
}




void PNGManip::decode()
{
    if (validateInputFile() != PNGManipErrorCode::Success)
        return;

    start = std::chrono::high_resolution_clock::now();

    if (decodeImage() != PNGManipErrorCode::Success)
        return;

    if (saveDecodedPNGInfo() != PNGManipErrorCode::Success)
        return;

    end = std::chrono::high_resolution_clock::now();

    std::cout << "\n\033[32m" << "Image decoded successfully!" << "\033[0m\n";

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    if (duration.count() > 1'000'000)
        std::cout << "\nDecoding process took: " << "\033[36m" << static_cast<float>(duration.count() / 1'000'000) << " seconds\033[0m\n";
    else if (duration.count() > 1'000)
        std::cout << "\nDecoding process took: " << "\033[36m" << static_cast<float>(duration.count() / 1'000) << " milliseconds\033[0m\n";
    else
        std::cout << "\nDecoding process took: " << "\033[36m" << duration.count() << " microseconds\033[0m\n";
}






/**
* Public Functions -----------------------------------
*/

PNGManip::PNGManip(const std::string& type, const std::string& input, const std::string& output, const std::string& terminalDisp) : 
	processType(type),
	inputFile(input), 
	outputFile(output),
	pixel(),
	pngImage(),
	terminalOutput(terminalDisp)
{
	if (processType == "ENCODE")
	{
		// Input file should be a text file

		// Get dimensions
		auto dimensions = getDimensions( getFileSize(inputFile.c_str()) );
		pngImage.width = static_cast<uint16_t>(dimensions.first);
		pngImage.height = static_cast<uint16_t>(dimensions.second);

		std::cout << "\033[36m[INFO] Resultant Image Dimensions: " << pngImage.width << " x " << pngImage.height << "\033[0m" << std::endl;

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
		logError("Invalid process type. Use 'ENCODE' or 'DECODE'.\n");
	}
}



//PNGManip::~PNGManip()
//{
//	if (pngImage.pixels)
//		free(pngImage.pixels);
//}



void PNGManip::startProcess()
{
	if (processType == "ENCODE")
		encode();
	
	else if (processType == "DECODE")
		decode();
	
	else
        logError("Invalid process type. Use 'ENCODE' or 'DECODE'.\n");
}
