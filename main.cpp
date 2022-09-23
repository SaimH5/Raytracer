#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "src/vec3.h"
#include "src/ray.h"

struct render_info
{
    const int img_width;
    const int img_height;
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    render_info(const int width, 
                const int height,
                point3 org,
                point3 llc,
                vec3 horz,
                vec3 vert ) : 
                img_width(width), 
                img_height(height), 
                origin(org), 
                lower_left_corner(llc), 
                horizontal(horz), 
                vertical(vert) {}
};

color ray_color(const ray& r)
{
    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return t * color(0, 0, 0) + (1-t) * color(1, 1, 1);
}

std::string write_color(color col)
{
    return std::to_string(static_cast<int>(255 * col[0])) + " " +
           std::to_string(static_cast<int>(255 * col[1])) + " " +
           std::to_string(static_cast<int>(255 * col[2])) + "\n";
}

void render_lines(std::vector<std::string>& pixelColors, int starting_scanline, int ending_scanline, render_info inf)
{
    // color bg_color(1, 0.5, 0);
    for(int row = starting_scanline; row >= ending_scanline; row--)
    {
        std::cerr << "Scanlines remaining: " << row << " \r" << std::flush;
        for(int col = 0; col < inf.img_width; col++)
        {
            auto u = static_cast<double>(col) / inf.img_width;
            auto v = static_cast<double>(row) / inf.img_height;
            ray r(inf.origin, inf.lower_left_corner + u * inf.horizontal + v * inf.vertical);
            color px_col = ray_color(r);
            pixelColors.push_back(write_color(px_col));
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

    // Camera setup
    auto viewport_height = 1.0;
    auto viewport_width = aspect_ratio * viewport_height;
    point3 origin(0, 0, 0);
    vec3 horizontal(viewport_width, 0, 0);
    vec3 vertical(0, viewport_height, 0);
    vec3 lower_left_corner(-viewport_width/2, -viewport_height/2, -1);

    render_info inf(image_width, image_height, origin, lower_left_corner, horizontal, vertical);

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    int loop_count = 10;
    int scanlines_per_loop = image_height / loop_count;
    for(int i = 0; i < loop_count; i++)
    {
        render_lines(pixelColors, (image_height - 1) - i * scanlines_per_loop, (image_height) - (i + 1) * scanlines_per_loop, inf);
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