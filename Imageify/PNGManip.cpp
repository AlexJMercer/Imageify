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
    std::unique_ptr<FILE, decltype(&fclose)> inputFilePtr( fopen(inputFile.c_str(), "rb"), &fclose);
    if (!inputFilePtr) {
        logError("Cannot open input file: " + inputFile);
        return PNGManipErrorCode::FileNotFound;
    }

    uint32_t fileSize = static_cast<uint32_t>(getFileSize(inputFile.c_str()));

    std::vector<uint8_t> buffer(fileSize + sizeof(uint32_t)); // Allocate and size the buffer

	// Put the size of the file in the first 4 bytes
    memcpy(buffer.data(), &fileSize, sizeof(uint32_t));

    size_t bytesRead = fread(buffer.data() + sizeof(uint32_t), 1, fileSize, inputFilePtr.get());

    if (bytesRead != fileSize) 
    {
        logError("Error reading input file: " + inputFile);
        return PNGManipErrorCode::FileNotReadable;
	}


    size_t idx = 0;
    for (size_t row = 0; row < pngImage.height; ++row) 
    {
        for (size_t col = 0; col < pngImage.width; ++col) 
        {
            pixel           = pixelAt(&pngImage, row, col);

            pixel->red      = (idx < bytesRead) ? buffer[ idx++ ] : 0x00;
            pixel->green    = (idx < bytesRead) ? buffer[ idx++ ] : 0x00;
            pixel->blue     = (idx < bytesRead) ? buffer[ idx++ ] : 0x00;
            pixel->alpha    = (idx < bytesRead) ? buffer[ idx++ ] : 0x00;
        }
    }

    return PNGManipErrorCode::Success;
}




PNGManipErrorCode PNGManip::decodeImage() 
{
    std::unique_ptr<FILE, decltype(&fclose)> filePtr( fopen(inputFile.c_str(), "rb" ), &fclose );

    if (!filePtr) {
        logError("Cannot open input file: " + inputFile);
        return PNGManipErrorCode::FileNotFound;
    }

    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) 
    {
        logError("Cannot read PNG image (struct creation failed).");
        return PNGManipErrorCode::DecodingError;
    }

    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        logError("Cannot read PNG image (info struct failed).");
        return PNGManipErrorCode::DecodingError;
    }

    
    if (setjmp(png_jmpbuf(png_ptr))) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        logError("PNG read error (setjmp).");
        return PNGManipErrorCode::DecodingError;
    }
    
    
    png_init_io(png_ptr, filePtr.get());
    png_read_info(png_ptr, info_ptr);
    
    
    pngImage.width      = static_cast<uint16_t>(png_get_image_width(png_ptr, info_ptr));
    pngImage.height     = static_cast<uint16_t>(png_get_image_height(png_ptr, info_ptr));
    pngImage.pixelDepth = png_get_bit_depth(png_ptr, info_ptr);
    pngImage.pixelSize  = 4;
	pngImage.pixels.resize(static_cast<size_t>(pngImage.width * pngImage.height));
    
    if (pngImage.pixels.empty()) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        logError("Memory allocation error for PNG pixels.");
        return PNGManipErrorCode::MemoryAllocationError;
    }
    
    std::cout << "[INFO] Image Dimensions:\033[36m"
        << "\nWidth:\t\t"     << pngImage.width
        << "\nHeight:\t\t"    << pngImage.height
        << "\nDepth:\t\t"     << (int)pngImage.pixelDepth
        << "\nPixel Size:\t"  << (int)pngImage.pixelSize
        << "\033[0m\n";
    
    
    std::vector<std::vector<uint8_t>> rowStorage(
        pngImage.height,
        std::vector<uint8_t>(pngImage.width * pngImage.pixelSize)
    );

    std::vector<png_bytep> row_pointers;
	row_pointers.reserve(pngImage.height);

    for (auto& row : rowStorage)
        row_pointers.push_back(row.data());
 
    
    png_read_image(png_ptr, row_pointers.data());
 

    for (size_t row = 0; row < pngImage.height; ++row)
    {
        auto* rowData = rowStorage[row].data();
        for (size_t col = 0; col < pngImage.width; ++col)
        {
            pixel_t* px = pixelAt(&pngImage, row, col);
            px->red     = *rowData++;
            px->green   = *rowData++;
            px->blue    = *rowData++;
            px->alpha   = *rowData++;
        }
    }

        
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    
    
    return PNGManipErrorCode::Success;
}




PNGManipErrorCode PNGManip::savePNGToFile() 
{
    std::unique_ptr<FILE, decltype(&fclose)> filePtr( fopen(outputFile.c_str(), "wb"), &fclose );
    if (!filePtr) 
    {
        logError("Cannot open output file: " + outputFile);
        return PNGManipErrorCode::FileNotWritable;
    }
    
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) 
    {
        logError("Cannot create PNG write struct.");
        return PNGManipErrorCode::EncodingError;
    }
    
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        logError("Cannot create PNG info struct.");
        return PNGManipErrorCode::EncodingError;
    }
    
    
    if (setjmp(png_jmpbuf(png_ptr))) 
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
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
    
    
    std::vector<std::vector<uint8_t>> rowStorage(
        pngImage.height,
        std::vector<uint8_t>(pngImage.width * pngImage.pixelSize)
	);
    
    
    for (size_t row = 0; row < pngImage.height; ++row)
    {
        auto& rowVec = rowStorage[row];

        for (size_t col = 0; col < pngImage.width; ++col)
        {
            pixel_t* pixel = pixelAt(&pngImage, row, col);
            size_t offset = col * pngImage.pixelSize;

            rowVec[offset] = pixel->red;
            rowVec[offset + 1] = pixel->green;
            rowVec[offset + 2] = pixel->blue;
            rowVec[offset + 3] = pixel->alpha;
        }
    }
    

    std::vector<png_bytep> row_pointers;
    row_pointers.reserve(pngImage.height);

    for (auto& row : rowStorage)
        row_pointers.push_back(row.data());

    
    png_init_io(png_ptr, filePtr.get());
    png_set_rows(png_ptr, info_ptr, row_pointers.data());
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    
    return PNGManipErrorCode::Success;
}


/*
PNGManipErrorCode PNGManip::saveDecodedPNGInfo()
{
    std::unique_ptr<FILE, decltype(&fclose)> outputFilePtr( fopen(outputFile.c_str(), "wb"), &fclose );
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

        fwrite(outputStr.data(), 1, outputStr.size(), outputFilePtr.get());

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

        fwrite(outputStr.data(), 1, outputStr.size(), outputFilePtr.get());

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
    }
    else
    {
        logError("Unsupported output file format. Please use .txt or .pdf extension.");
        return PNGManipErrorCode::FileNotWritable;
	}
    
    
    
    if (terminalOutput == "TRUE")
        std::cout << "\nDecoded Output:\n" << outputStr << std::endl;
    
    
    return PNGManipErrorCode::Success;
}
*/

PNGManipErrorCode PNGManip::saveDecodedPNGInfo()
{
    std::unique_ptr<FILE, decltype(&fclose)> outputFilePtr(fopen(outputFile.c_str(), "wb"), &fclose);
    if (!outputFilePtr) {
        logError("Cannot open output file: " + outputFile);
        return PNGManipErrorCode::FileNotWritable;
    }

    // Flatten pixels
    std::vector<uint8_t> buffer;
    buffer.reserve(static_cast<size_t>(pngImage.width * pngImage.height * 4));
    
    for (size_t row = 0; row < pngImage.height; ++row) 
    {
        for (size_t col = 0; col < pngImage.width; ++col) 
        {
            pixel_t* px = pixelAt(&pngImage, row, col);

            buffer.push_back(px->red);
            buffer.push_back(px->green);
            buffer.push_back(px->blue);
            buffer.push_back(px->alpha);
        }
    }

    // Get file size from first 4 bytes
    if (buffer.size() < sizeof(uint32_t)) 
    {
        logError("Corrupted image: missing header.");
        return PNGManipErrorCode::DecodingError;
    }


    uint32_t fileSize;
    memcpy(&fileSize, buffer.data(), sizeof(uint32_t));

    if (fileSize > buffer.size() - sizeof(uint32_t)) 
    {
        logError("Invalid file size in header.");
        return PNGManipErrorCode::DecodingError;
    }

    // Write the actual file content
    fwrite(buffer.data() + sizeof(uint32_t), 1, fileSize, outputFilePtr.get());

    std::cout << "\n[INFO] Decoded file written: \033[36m"
        << static_cast<float>(fileSize / 1024.0) << " KB\033[0m" << std::endl;

    if (terminalOutput == "TRUE") 
    {
        std::cout << "\nDecoded Output:\n";
        std::cout.write(reinterpret_cast<const char*>(buffer.data() + sizeof(uint32_t)), fileSize);
        std::cout << std::endl;
	}

    return PNGManipErrorCode::Success;
}





inline pixel_t* PNGManip::pixelAt(bitmap_t* bitmap, size_t row, size_t col)
{
    return &bitmap->pixels.at(row * bitmap->width + col);
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
	pixel(nullptr),
    pngImage{},
	terminalOutput(terminalDisp)
{
	if (processType == "ENCODE")
	{
		// Get dimensions
		const size_t headerSize = 8;
		const size_t fileSize = getFileSize(inputFile.c_str());

		auto dimensions = getDimensions( fileSize + headerSize );
		pngImage.width = static_cast<uint16_t>(dimensions.first);
		pngImage.height = static_cast<uint16_t>(dimensions.second);

		std::cout << "[INFO] Resultant Image Dimensions: \033[36m" << pngImage.width << " x " << pngImage.height << "\033[0m" << std::endl;

		pngImage.pixelSize = static_cast<png_byte>(4);
		pngImage.pixelDepth = static_cast<png_byte>(8);
		pngImage.pixels.resize(static_cast<size_t>(pngImage.width * pngImage.height));

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



void PNGManip::startProcess()
{
	if (processType == "ENCODE")
		encode();
	
	else if (processType == "DECODE")
		decode();
	
	else
        logError("Invalid process type. Use 'ENCODE' or 'DECODE'.\n");
}
