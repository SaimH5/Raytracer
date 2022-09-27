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
    int sample_count;
    const int max_depth;
    camera cam;

    render_info(const int width, 
                const int height,
                const int sampling_rate,
                int total_samples,
                const int depth,
                camera camera)
                : 
                img_width(width), 
                img_height(height), 
                samples_per_pixel(sampling_rate),
                sample_count(total_samples),
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
        if(rec.mat_ptr->scatter(r, rec, scattered, attenuation))
            return attenuation * ray_color(scattered, h, max_depth-1);
            
        return rec.mat_ptr->emitted();
    }

    return color(0, 0, 0);

    // For sky background 
    /*
    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return (1-t) * color(1, 1, 1) + t * color(0.5, 0.7, 1);
    */
}

std::string write_color(color col)
{
    col[0] = sqrt(col[0]);
    col[1] = sqrt(col[1]);
    col[2] = sqrt(col[2]);

    return std::to_string(static_cast<int>(255 * clamp(col[0], 0, 1))) + " " +
           std::to_string(static_cast<int>(255 * clamp(col[1], 0, 1))) + " " +
           std::to_string(static_cast<int>(255 * clamp(col[2], 0, 1))) + "\n";
}


// Main render loop function, renders scanlines from starting_scanline to ending_scanline and stores results in pixelColors
void render_lines(std::vector<color>& pixelColors, int no_samples, render_info& inf, const hittable_list& h)
{
    for(int samples = 0; samples < no_samples; samples++)
    {
        std::string log = "Samples remaining: " + std::to_string(inf.sample_count) + "   \r";
        std::cerr << log;
        for(int row = inf.img_height - 1; row >= 0; row--)
        {
            for(int col = 0; col < inf.img_width; col++)
            {
                auto u = static_cast<double>(col + random_double()) / (inf.img_width - 1);
                auto v = static_cast<double>(row + random_double()) / (inf.img_height - 1);
                ray r = inf.cam.get_ray(u, v);
                color px_col = ray_color(r, h, inf.max_depth);
                pixelColors[((inf.img_height - 1 - row) * inf.img_width) + col] += px_col;
            }
        }
        inf.sample_count--;
    }
}





int main()
{
    thread_pool pool;
    std::ofstream outputImage("output.ppm", std::ios::trunc);

    // Image dimensions
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = image_width / aspect_ratio;
    const int samples_per_pixel = 50;

    std::vector<color> pixelColors(image_width * image_height);

    // Camera setup
    point3 origin(0, 0, 0);
    camera cam(aspect_ratio, origin);
    const int max_depth = 50;

    // int scanline_count = image_height;
    render_info inf(image_width, image_height, samples_per_pixel, samples_per_pixel, max_depth, cam);

    // Scene setup
    hittable_list scene;
    scene.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, std::make_shared<lambertian>(color(1, 0, 0))));
    scene.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.5, std::make_shared<metal>(color(0.4, 0.4, 0.4), 0.1)));
    scene.add(std::make_shared<sphere>(point3(1, 0, -1), 0.5, std::make_shared<dielectric>(1.4)));
    scene.add(std::make_shared<sphere>(point3(0, 15, 0), 10, std::make_shared<diffuse_light>(color(5, 5, 5))));
    scene.add(std::make_shared<sphere>(point3(0, -1000.5, -1), 1000, std::make_shared<lambertian>(color(0, 1, 0))));

    // for( int i = 0; i < 20; i++)
    // {
    //     color c(random_double(), random_double(), random_double());
    //     scene.add(std::make_shared<sphere>(point3(random_double(-5, 5), random_double(0, 2), random_double(0, 2)), 0.2, std::make_shared<lambertian>(c)));
    // }

    // PPM file data
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Render loop
    const int thread_count = pool.thread_count();
    int samples_per_thread = samples_per_pixel / (thread_count + 1);
    std::vector<std::future<void>> futures;

    auto t1 = high_resolution_clock::now();
    for(int i = 0; i <= thread_count; i++)
    {
        // Multithreaded_version
        if(i == thread_count)
        {
            render_lines(pixelColors, samples_per_pixel -  i * samples_per_thread, inf, scene);
        }
        else
        {
            futures.push_back(pool.submit(render_lines, std::ref(pixelColors), samples_per_thread, std::ref(inf), std::ref(scene)));
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

    auto scale = 1.0 / samples_per_pixel;
    for(auto& px_color : pixelColors)
    {
        px_color *= scale;
        outputImageString += write_color(px_color);
    }

    outputImage << outputImageString;
    outputImage.close();


    return 0;
}