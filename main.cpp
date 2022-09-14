#include <iostream>
#include <fstream>
#include <vector>
#include <string>


int main()
{
    std::ofstream outputImage("output.ppm", std::ios::trunc);
    std::vector<std::string> pixelColors;
    // Image dimensions
    const int aspect_ratio = 2.0;
    const int image_width = 200;
    const int image_height = image_width / aspect_ratio;

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    for(int row = image_height - 1; row >= 0; row--)
    {
        std::cerr << "Scanlines remaining: " << row << " \r" << std::flush;
        for(int col = 0; col < image_width; col++)
        {
            pixelColors.push_back("255 0 0\n");
        }
    }
    std::cerr << "\nDone!";

    std::string outputImageString;
    for(auto line : pixelColors)
    {
        outputImageString += line;
    }

    outputImage << outputImageString;
    outputImage.close();


    return 0;
}