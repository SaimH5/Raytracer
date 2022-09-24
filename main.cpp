#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "src/vec3.h"
#include "src/ray.h"
#include "src/sphere.h"
#include "src/camera.h"
#include "src/hittable_list.h"

struct render_info
{
    const int img_width;
    const int img_height;
    camera cam;

    render_info(const int width, 
                const int height,
                camera camera ) 
                : 
                img_width(width), 
                img_height(height), 
                cam(camera) {}
};

color ray_color(const ray& r, hittable_list& h)
{
    color attenuation;
    hit_record rec;
    if(h.hit(r, 0, std::numeric_limits<double>::infinity(), attenuation, rec)) 
    {
        auto norm = unit_vector(rec.normal);
        norm = norm + 1.0;
        norm *= 0.5;
        return norm;
    }

    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return (1-t) * color(1, 1, 1) + t * color(0.5, 0.7, 1);
}

std::string write_color(color col)
{
    return std::to_string(static_cast<int>(255 * col[0])) + " " +
           std::to_string(static_cast<int>(255 * col[1])) + " " +
           std::to_string(static_cast<int>(255 * col[2])) + "\n";
}


// Main render loop function, renders scanlines from starting_scanline to ending_scanline and stores results in pixelColors
void render_lines(std::vector<std::string>& pixelColors, int starting_scanline, int ending_scanline, render_info& inf, hittable_list& h)
{
    for(int row = starting_scanline; row >= ending_scanline; row--)
    {
        std::cerr << "Scanlines remaining: " << row << " \r" << std::flush;
        for(int col = 0; col < inf.img_width; col++)
        {
            auto u = static_cast<double>(col) / (inf.img_width - 1);
            auto v = static_cast<double>(row) / (inf.img_height - 1);
            ray r = inf.cam.get_ray(u, v);
            color px_col = ray_color(r, h);
            pixelColors[((inf.img_height - 1 - row) * inf.img_width) + col] = write_color(px_col);
        }
    }
}

int main()
{
    std::ofstream outputImage("output.ppm", std::ios::trunc);
    // Image dimensions
    const int aspect_ratio = 2.0;
    const int image_width = 1000;
    const int image_height = image_width / aspect_ratio;

    std::vector<std::string> pixelColors(image_width * image_height);

    // Camera setup
    point3 origin(0, 0, 0);
    camera cam(aspect_ratio, origin);
    cam.get_ray(0, 0);

    render_info inf(image_width, image_height, cam);

    // Scene setup
    hittable_list scene;
    scene.add(std::make_shared<sphere>(point3(0.5, 0, -1), 0.5, color(1, 0, 0)));
    scene.add(std::make_shared<sphere>(point3(-0.5, 0, -1), 0.5, color(1, 0, 0)));
    scene.add(std::make_shared<sphere>(point3(0, -1000.5, -1), 1000, color(1, 0, 0)));

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    int loop_count = 4;
    int scanlines_per_loop = image_height / loop_count;
    for(int i = 0; i < loop_count; i++)
    {
        render_lines(pixelColors, (image_height - 1) - i * scanlines_per_loop, (image_height) - (i + 1) * scanlines_per_loop, inf, scene);
    }
    std::cerr << "\nDone!";

    std::string outputImageString;
    for(auto& px_color : pixelColors)
    {
        outputImageString += px_color;
    }

    outputImage << outputImageString;
    outputImage.close();


    return 0;
}