
#include "pngHeaders.h"
#include "definitions.h"



void convertPNGToArray(char *path)
{
	// TO-DO
	// Validate path
	 
	FILE* fp = fopen(path, "rb");
	png_byte** row_pointers = NULL;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) {
		std::cout << "\nERROR: Cannot create PNG.\n";
		abort();
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		std::cout << "\nERROR: Cannot create PNG.\n";
		abort();
	}

	if (setjmp(png_jmpbuf(png))) {
		std::cout << "\nERROR: Cannot create PNG.\n";
		abort();
	}


	png_init_io(png, fp);
	png_read_info(png, info);

	int width = png_get_image_width(png, info);
	int height = png_get_image_height(png, info);


	png_byte bit = png_get_bit_depth(png, info);
	std::cout << "\nBit depth of Image: " << (int)bit << std::endl;

	
	png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	png_read_update_info(png, info);

	if (row_pointers) {
		std::cout << "\nERROR: Cannot create data array.\n";
		abort();
	}

	
	
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (int iter = 0; iter < height; iter++) {
		row_pointers[iter] = (png_byte*)malloc(png_get_rowbytes(png, info));
	}



	if (!row_pointers) {
		std::cerr << "Error: row_pointers is null." << std::endl;
		return;
	}

	for (int y = 0; y < height; y++) {
		png_bytep row = row_pointers[y];

		for (int x = 0; x < width; x++) {
			png_bytep px = &(row[x * 3]);
			
			std::cout << ( static_cast<int>(px[0]) + 30 ) << " ";
			std::cout << ( static_cast<int>(px[1]) + 30 ) << " ";
			std::cout << ( static_cast<int>(px[2]) + 30 ) << " \n";
		}
		std::cout << std::endl; // Print a new line after each row
	}



	png_read_image(png, row_pointers);

	fclose(fp);
	png_destroy_read_struct(&png, &info, NULL);

	return;
}
