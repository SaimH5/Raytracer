#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "src/vec3.h"

std::string write_color(color col)
{
    return std::to_string(static_cast<int>(255 * col[0])) + " " +
           std::to_string(static_cast<int>(255 * col[1])) + " " +
           std::to_string(static_cast<int>(255 * col[2])) + "\n";
}

void render_lines(std::vector<std::string>& pixelColors, int starting_scanline, int ending_scanline, int scanline_width)
{
    color bg_color(1, 0.5, 0);
    for(int row = starting_scanline; row >= ending_scanline; row--)
    {
        std::cerr << "Scanlines remaining: " << row << " \r" << std::flush;
        for(int col = 0; col < scanline_width; col++)
        {
            pixelColors.push_back(write_color(bg_color));
        }
    }
}


int main()
{
    std::ofstream outputImage("output.ppm", std::ios::trunc);
    std::vector<std::string> pixelColors;
    // Image dimensions
    const int aspect_ratio = 2.0;
    const int image_width = 1000;
    const int image_height = image_width / aspect_ratio;

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    int loop_count = 10;
    int scanlines_per_loop = image_height / loop_count;
    for(int i = 0; i < loop_count; i++)
    {
        render_lines(pixelColors, (image_height - 1) - i * scanlines_per_loop, (image_height) - (i + 1) * scanlines_per_loop, image_width);
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