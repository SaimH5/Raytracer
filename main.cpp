#include <chrono>
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
#include "src/thread_pool.h"
#include "src/utilities.h"
#include "src/material.h"

using namespace std::chrono;

struct render_info
{
    const int img_width;
    const int img_height;
    const int samples_per_pixel;
    int scanline_count;
    const int max_depth;
    camera cam;

    render_info(const int width, 
                const int height,
                const int sampling_rate,
                int scanlines,
                const int depth,
                camera camera)
                : 
                img_width(width), 
                img_height(height), 
                samples_per_pixel(sampling_rate),
                scanline_count(scanlines),
                max_depth(depth),
                cam(camera) {}
                
};

color ray_color(const ray& r, const hittable_list& h, int max_depth)
{
    if(max_depth <= 0)
    {
        return color(0, 0, 0);
    }
    hit_record rec;
    if(h.hit(r, 0.001, inf, rec)) 
    {
        ray scattered;
        color attenuation;
        // color attenuation(0.5, 0, 0);
        // ray scattered(rec.p, rec.normal + random_in_unit_sphere());
        rec.mat_ptr->scatter(r, rec, scattered, attenuation);
        return attenuation * ray_color(scattered, h, max_depth-1);
    }

    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return (1-t) * color(1, 1, 1) + t * color(0.5, 0.7, 1);
}

std::string write_color(color col)
{
    col[0] = sqrt(col[0]);
    col[1] = sqrt(col[1]);
    col[2] = sqrt(col[2]);

    return std::to_string(static_cast<int>(255 * col[0])) + " " +
           std::to_string(static_cast<int>(255 * col[1])) + " " +
           std::to_string(static_cast<int>(255 * col[2])) + "\n";
}


// Main render loop function, renders scanlines from starting_scanline to ending_scanline and stores results in pixelColors
void render_lines(std::vector<std::string>& pixelColors, int starting_scanline, int ending_scanline, render_info& inf, const hittable_list& h)
{
    for(int row = starting_scanline; row >= ending_scanline; row--)
    {
        std::string log = "Scanlines remaining: " + std::to_string(inf.scanline_count) + "   \r";
        std::cerr << log;
        for(int col = 0; col < inf.img_width; col++)
        {
            color px_col;
            for(int i = 0; i < inf.samples_per_pixel; i++)
            {
                auto u = static_cast<double>(col + random_double()) / (inf.img_width - 1);
                auto v = static_cast<double>(row + random_double()) / (inf.img_height - 1);
                ray r = inf.cam.get_ray(u, v);
                px_col += ray_color(r, h, inf.max_depth);
            }
            px_col /= inf.samples_per_pixel;
            pixelColors[((inf.img_height - 1 - row) * inf.img_width) + col] = write_color(px_col);
        }
        inf.scanline_count--;
    }
}





int main()
{
    thread_pool pool;
    std::ofstream outputImage("output.ppm", std::ios::trunc);

    // Image dimensions
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 600;
    const int image_height = image_width / aspect_ratio;
    const int samples_per_pixel = 200;

    std::vector<std::string> pixelColors(image_width * image_height);

    // Camera setup
    point3 origin(0, 0, 0);
    camera cam(aspect_ratio, origin);
    const int max_depth = 50;

    int scanline_count = image_height;
    render_info inf(image_width, image_height, samples_per_pixel, scanline_count, max_depth, cam);

    // Scene setup
    hittable_list scene;
    scene.add(std::make_shared<sphere>(point3(0.5, 0, -1), 0.5, std::make_shared<lambertian>(color(1, 0, 0))));
    scene.add(std::make_shared<sphere>(point3(-0.5, 0, -1), 0.5, std::make_shared<metal>(color(0.4, 0.4, 0.4), 0.1)));
    scene.add(std::make_shared<sphere>(point3(0, -1000.5, -1), 1000, std::make_shared<lambertian>(color(0, 1, 0))));

    for( int i = 0; i < 20; i++)
    {
        color c(random_double(), random_double(), random_double());
        scene.add(std::make_shared<sphere>(point3(random_double(-5, 5), random_double(0, 2), random_double(0, 2)), 0.2, std::make_shared<lambertian>(c)));
    }

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    const int thread_count = pool.thread_count();
    int loop_count = thread_count * 10 + 1;
    int scanlines_per_loop = image_height / (loop_count + 1);
    std::vector<std::future<void>> futures;

    auto t1 = high_resolution_clock::now();
    for(int i = 0; i < loop_count + 1; i++)
    {
        // Multithreaded_version
        if(i == loop_count)
        {
            render_lines(pixelColors, (image_height - 1) - i * scanlines_per_loop, 0, inf, scene);
        }
        else
        {
            futures.push_back(pool.submit(render_lines, std::ref(pixelColors), (image_height - 1) - i * scanlines_per_loop, (image_height) - (i + 1) * scanlines_per_loop, std::ref(inf), std::ref(scene)));
        }

        // Single threaded version
        // render_lines(pixelColors, (image_height - 1) - i * scanlines_per_loop, (image_height) - (i + 1) * scanlines_per_loop, inf, scene);
    }

    for(const auto& ft : futures)
    {
        ft.wait();
    }
    auto t2 = high_resolution_clock::now();
    std::cerr << "\nTime taken: " << duration_cast<milliseconds>(t2-t1).count();
    std::cerr << "\nDone!";
    std::cerr << "\nWriting to file.";
    std::string outputImageString;
    for(auto& px_color : pixelColors)
    {
        outputImageString += px_color;
    }

    outputImage << outputImageString;
    outputImage.close();


    return 0;
}