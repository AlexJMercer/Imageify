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



static inline void printHelp()
{
    std::cout << "Convert any Text file into a PNG image and back !\n"
        << "Usage: Imageify.exe [OPTIONS]\n"
        << "Options:\n"
        << "\t-h\t\t--help  \t\t<Show Help Menu>\n"
        << "\t-e\t\t--encode\t\t<Path to Text File>\n"
        << "\t-d\t\t--decode\t\t<Path to PNG image>\n"
        << "\t-o\t\t--output\t\t<Name of Output File>\n"
        << "\t-s\t\t--show  \t\t<Show Decoded Text in Terminal>\n";
}



static std::vector<std::string> parseArguments(int argc, char* argv[])
{
    std::string type, inputFile, outputFile, showDecoded = "FALSE";


    for (int i = 1; i < argc; ++i)
    {
        if ((std::strcmp(argv[i], "-e") == 0 || std::strcmp(argv[i], "--encode") == 0) && i + 1 < argc)
        {
            inputFile = argv[++i];
			type = "ENCODE";

        }
        else if ((std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--decode") == 0) && i + 1 < argc)
        {
            inputFile = argv[++i];
			type = "DECODE";
        }
        else if ((std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--show") == 0) && i + 1 < argc)
        {
			showDecoded = "TRUE";
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
    if (inputFile.empty() && type == "ENCODE")
		inputFile = "testFile.txt";
    
	// Default to outputImage.png if no input file is provided while decoding process
    else if (inputFile.empty() && type == "DECODE")
		inputFile = "outputImage.png";

    
	// Default to outputImage.png if no output file is provided while encoding process
    if (outputFile.empty() && type == "ENCODE")
		outputFile = "outputImage.png";

    // Default to outputText.txt if no output file is provided while decoding process
	else if (outputFile.empty() && type == "DECODE")
		outputFile = "outputText.txt";

    // Display chosen options
    std::cout << "\n[INFO] Chosen options:\n" 
        << "Process Type        :\t" << type << "\n"
        << "Path to Input File  :\t" << inputFile << "\n"
        << "Path to Output File :\t" << outputFile << "\n"
        << "Show decoded text?  :\t" << showDecoded << "\n"
        << std::endl;


    return { type, inputFile, outputFile, showDecoded };
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printHelp();
        return EXIT_SUCCESS;
	}

	std::vector<std::string> args = parseArguments(argc, argv);

    if (args.size() == 0)
		return EXIT_FAILURE;

    PNGManip pngProcessor(args[0], args[1], args[2], args[3]);
    pngProcessor.startProcess();


    return EXIT_SUCCESS;
}



