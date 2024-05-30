
#include "headers.hpp"

/*
std::vector<BYTE> ReadPPMImage(const std::string_view filename, int& width, int& height)
{
    std::ifstream fd(filename.data());
    if (!fd) {
        std::cerr << "Failed to read " << filename << std::endl;
        return {};
    }

    // validate file header
    constexpr auto FileHeader = "P3";
    std::string line;
    fd >> line;
    if (line != FileHeader) {
        std::cerr << "Failed to read " << filename << ", bad header!\n";
        return {};
    }

    //int width = 0;
    //int height = 0;
    fd >> width >> height;
    if (width <= 0 || height <= 0) {
        std::cerr << "Failed to read " << filename << ", bad dimensions!\n";
        return {};
    }

    int bpp = 0;
    fd >> bpp;
    if (bpp < 0 || bpp >= 256) {
        std::cerr << "Failed to read " << filename << ", bad color size!\n";
        return {};
    }

    // prepare the data buffer
    constexpr auto BPP = 3;
    const std::size_t imageSize = static_cast<size_t>(width) * height * BPP;

    std::vector<BYTE> imageData(imageSize);

    std::size_t row = 0;
    std::string r, g, b;
    for (row = 0; row < imageSize && fd; row += BPP) {
        fd >> r >> g >> b;

        // Windows bitmaps are BGR
        imageData[row] = static_cast<BYTE> (std::stoi(b));
        imageData[row + 1] = static_cast<BYTE> (std::stoi(g));
        imageData[row + 2] = static_cast<BYTE> (std::stoi(r));
    }

    return imageData;
}



void CreateImage(const std::string& filename, ImageEncondings enconding, int imageWidth, int imageHeight, BYTE* imageData)
{

    // Windows APIs expect to deal with wide strings anyway
    std::wstring wfilename(filename.begin(), filename.end());
    
    SaveImage(wfilename, enconding, imageWidth, imageHeight, imageData);
}
*/