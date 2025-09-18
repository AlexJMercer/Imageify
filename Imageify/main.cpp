/*
* The aim here is to convert:
*   Text --> PNG Image and vice versa
* 
* While at the same time making sure that we are encoding the pixel color values
* with as much information as we can, to minimize the space required.
* 
* This will also be a good test to see if we are able to actually reduce the size
* of the text information by simply converting it into a PNG.
* 
*/


/*
* This project uses libpng
* 
* The precompiled libpng headers can be found in the main directory of the project.
* 
* This video below provides an excellent walkthrough to compile and assemble libpng
* https://www.youtube.com/watch?v=IP5oEBCW3J8&ab_channel=Murmelex
* 
* And the following GitHub link provides a good template to build your own PNGs
* I have used the same as boilerplate code.
*
* https://github.com/murmelex/libpng-start-project/
*
*/

#include <iostream>
#include <vector>

#include "PNGManip.hpp"


/*

void encode()
{
    std::string filepath;

    std::pair<size_t, size_t> dimensions;
    std::vector<int> rawPixelArray;


    // First, we take input of a file.
    std::cout << "\nEnter a file or provide a path to the file:\n";
    std::cin >> filepath;// First, we take input of a file.


    // TO-DO
    // Validate filepath

    // Now we covert the file into an int vector
    rawPixelArray = convertFileToArray(filepath);


    // Get Dimensions of the Image we are going to create
    dimensions = getDimensions(rawPixelArray.size());


    // Now convert it to PNG and save it
    convertArrayToPNG(&rawPixelArray, dimensions.first, dimensions.second);

    std::cout << "\nFile saved successfully !";
}



void decode()
{
    std::string imagePath;

    std::cout << "\nEnter a file or provide a path to the file:\n";
    std::cin >> imagePath;

    std::vector<int> pixelArray = convertPNGToArray( imagePath.data() );

    if ( pixelArray.size() == 0 )
    {
        std::cout << "\nERROR: Cannot decode image.\n";
        return;
	}

    for ( int val : pixelArray )
        std::cout << (char)val;

	std::cout << "\n\nImage decoded successfully !\n";

}
*/


static inline void printHelp()
{
    std::cout << "Usage: Imageify.exe [OPTIONS]\n"
        << "Options:\n"
        << "\t-h\t\t--help\t\t<Show Help Menu>\n"
        << "\t-e\t\t--encode\t\t<Path to Text File>\n"
        << "\t-d\t\t--decode\t\t<Path to PNG image>\n"
        << "\t-o\t\t--output\t\t<Name of Output File>\n";
}



static std::vector<std::string> parseArguments(int argc, char* argv[])
{
    std::string type, inputFile, outputFile;


    for (int i = 1; i < argc; ++i)
    {
        if ((std::strcmp(argv[i], "-e") == 0 || std::strcmp(argv[i], "--encode") == 0) && i + 1 < argc)
        {
            inputFile = argv[++i];
			type = "encode";

        }
        else if ((std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--decode") == 0) && i + 1 < argc)
        {
            inputFile = argv[++i];
			type = "decode";
        }

        else if ((std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0) && i + 1 < argc)
            outputFile = argv[++i];

        else
        {
            printHelp();
            return {};
        }
    }

	// Default values if not provided

	// Default to testFile.txt if no input file is provided while encoding process
    if (inputFile.empty() && type == "encode")
		inputFile = "testFile.txt";
    
	// Default to outputImage.png if no input file is provided while decoding process
    else if (inputFile.empty() && type == "decode")
		inputFile = "outputImage.png";

    
	// Default to outputImage.png if no output file is provided while encoding process
    if (outputFile.empty() && type == "encode")
		outputFile = "outputImage.png";

    // Default to outputText.txt if no output file is provided while decoding process
	else if (outputFile.empty() && type == "decode")
		outputFile = "outputText.txt";

    return {type, inputFile, outputFile};
}



int main(int argc, char* argv[])
{
	std::vector<std::string> args = parseArguments(argc, argv);

    if (args.size() == 0)
		return EXIT_FAILURE;

    PNGManip pngProcessor(args[1], args[2]);

    if (args[0] == "encode")
        pngProcessor.encode();
	
    else if (args[0] == "decode")
        pngProcessor.decode();
    


    return EXIT_SUCCESS;
}



