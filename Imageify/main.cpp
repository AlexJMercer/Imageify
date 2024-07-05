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
* This project uses OpenCV 4 along with libpng
* 
* The precompiled libpng headers can be found in the main directory of the project.
*
*/


#include "headers.h"




void test_OpenCV_Module()
{
    cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);
    image.setTo(cv::Scalar(255, 255, 255));

    // Draw a blue circle at the center of the image
    cv::circle(image, cv::Point(200, 200), 50, cv::Scalar(255, 0, 0), -1);

    // Draw a green rectangle
    cv::rectangle(image, cv::Point(50, 50), cv::Point(350, 350), cv::Scalar(0, 255, 0), 3);

    // Draw a red line
    cv::line(image, cv::Point(50, 350), cv::Point(350, 50), cv::Scalar(0, 0, 255), 2);

    // Put some text on the image
    cv::putText(image, "OpenCV Test", cv::Point(100, 390), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

    // Display the image in a window
    cv::imshow("OpenCV Test", image);
    cv::waitKey(0); // Wait for a key press

    // Save the image to a file
    std::string outputFileName = "opencv_test_output.png";
    if (cv::imwrite(outputFileName, image)) {
        std::cout << "Image saved successfully: " << outputFileName << std::endl;
    }
    else {
        std::cerr << "Failed to save the image" << std::endl;
    }
}



int main()
{
    /*
     * Uncomment this line and run the program to verify that openCV is properly configured on your system.
     *
     * The output will be a PNG Image saved in the same directory as this project.
    */
    //test_OpenCV_Module();


    
    std::string filepath;

    std::pair<size_t, size_t> dimensions;
    std::vector<short> rawPixelArray;


    // First, we take input of a file.
    std::cout << "Enter a file or provide a path to the file\n";
    std::cin >> filepath;

    // *** Validate filepath

    // Now we covert the file into an int vector
    rawPixelArray = convertFileToArray(filepath);


    // Get Dimensions of the Image we are going to create
    dimensions = getDimensions(rawPixelArray.size());


    // Now convert it to PNG and save it
    convertArrayToPNG(&rawPixelArray, dimensions.first, dimensions.second);
}



