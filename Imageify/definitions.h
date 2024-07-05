
#include <vector>
#include <string>



// Class Definitons

struct pixel_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};


struct bitmap_t {
    pixel_t* pixels;
    size_t width;
    size_t height;
};




// Function Definitions

std::vector<short> convertFileToArray(std::string);

std::pair<size_t, size_t> getDimensions(size_t);

int save_png_to_file(bitmap_t*, const char*);

void convertArrayToPNG(std::vector<short> *, size_t, size_t);