#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "src/ray.h"
#include "src/vec3.h"
#include "src/sphere.h"
#include "src/camera.h"
#include "src/hittable_list.h"
#include "src/thread_pool.h"
#include "src/utilities.h"
#include "src/material.h"
#include "src/moving_sphere.h"
#include "src/bvh.h"
#include "src/aarect.h"

using namespace std::chrono;

// Struct for holding details that the render loop needs 
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


// Core function for computing colors of pixels by shooting rays at objects in the scene
// The function is recursive and calculates up to {max_depth} bounces before terminating
color ray_color(const ray& r, const bvh_node& h, int max_depth)
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

    // For black background
    return color(0, 0, 0);

    // For sky background 
    /*
    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return (1-t) * color(1, 1, 1) + t * color(0.5, 0.7, 1);
    */
}

// Utility function for converting color values to a string that holds the RGB values for a pixel
// The input values are doubles which are first clamped to the range [0, 1],
// then converted to integers in the range [0, 255], then converted to strings and concatenated
std::string write_color(color col)
{
    col[0] = sqrt(col[0]);
    col[1] = sqrt(col[1]);
    col[2] = sqrt(col[2]);

    return std::to_string(static_cast<int>(255 * clamp(col[0], 0, 1))) + " " +
           std::to_string(static_cast<int>(255 * clamp(col[1], 0, 1))) + " " +
           std::to_string(static_cast<int>(255 * clamp(col[2], 0, 1))) + "\n";
}


// Main render loop function, renders up to {no_samples} samples of the entire image and adds the result of pixelColors
void render_lines(std::vector<color>& pixelColors, int no_samples, render_info& rend_inf, const bvh_node& h)
{
    for(int samples = 0; samples < no_samples; samples++)
    {
        std::string log = "Samples remaining: " + std::to_string(rend_inf.sample_count) + "   \r";
        std::cerr << log;
        for(int row = rend_inf.img_height - 1; row >= 0; row--)
        {
            for(int col = 0; col < rend_inf.img_width; col++)
            {
                auto u = static_cast<double>(col + random_double()) / (rend_inf.img_width - 1);
                auto v = static_cast<double>(row + random_double()) / (rend_inf.img_height - 1);
                ray r = rend_inf.cam.get_ray(u, v);
                color px_col = ray_color(r, h, rend_inf.max_depth);
                pixelColors[((rend_inf.img_height - 1 - row) * rend_inf.img_width) + col] += px_col;
            }
        }
        rend_inf.sample_count--;
    }
}


// Forward declarations of scene functions
hittable_list random_scene();
hittable_list two_perlin_spheres();
hittable_list two_spheres();
hittable_list cornell_box();

// Main entry function
int main()
{
    thread_pool pool;

    // Image dimensions
    const double aspect_ratio = 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;  
    const int max_depth = 50;

    std::vector<color> pixelColors(image_width * image_height);

    // Camera setup
    // point3 cam_lookfrom(0, 3, 5);  
    point3 cam_lookfrom(278, 278, -800);  
    point3 cam_lookat(278, 278, 0);
    double aperture = 0;
    double dist_to_focus = 10.0;    
    camera cam(cam_lookfrom, cam_lookat, vec3(0, 1, 0), 40, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // int scanline_count = image_height;
    render_info rend_inf(image_width, image_height, samples_per_pixel, samples_per_pixel, max_depth, cam);

    // Scene setup
    // hittable_list scene_list = random_scene();
    hittable_list scene_list = cornell_box();
    // scene_list.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, make_shared<lambertian>(color(1, 0, 0))));
    // scene_list.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, std::make_shared<metal>(color(0.4, 0.4, 0.4), 0.1)));
    // scene_list.add(std::make_shared<sphere>(point3(1, 0, -1), 0.5, std::make_shared<dielectric>(1.4)));
    // scene_list.add(make_shared<sphere>(point3(0, -1000.5, -1), 1000, make_shared<lambertian>(color(0, 1, 0))));
    // scene_list.add(std::make_shared<sphere>(point3(0, 15, 0), 10, std::make_shared<diffuse_light>(color(4, 4, 4))));
    bvh_node scene(scene_list, 0.0, 1.0);

    // for( int i = 0; i < 20; i++)
    // {
    //     color c(random_double(), random_double(), random_double());
    //     scene.add(std::make_shared<sphere>(point3(random_double(-5, 5), random_double(0, 2), random_double(0, 2)), 0.2, std::make_shared<lambertian>(c)));
    // }


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
            render_lines(pixelColors, samples_per_pixel -  i * samples_per_thread, rend_inf, scene);
        }
        else
        {
            futures.push_back(pool.submit(render_lines, std::ref(pixelColors), samples_per_thread, std::ref(rend_inf), std::ref(scene)));
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

    // PPM file data
    std::ofstream outputImage("output.ppm", std::ios::trunc);
    outputImage << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::string outputImageString;

    // Scales color values stored in pixelColors by the sampling rate,
    // then concatenates it to a single output string
    auto scale = 1.0 / samples_per_pixel;
    for(auto& px_color : pixelColors)
    {
        px_color *= scale;
        outputImageString += write_color(px_color);
    }

    // The final output string with all the pixel RGB values
    // is written to the output file in one go
    outputImage << outputImageString;
    outputImage.close();


    return 0;
}


// Scenes
// Scene with a large sphere serving as the ground, plus 3 big spheres and several smaller ones, with varying material types
hittable_list random_scene() 
{
    hittable_list world;
    auto ground_material = make_shared<lambertian>(make_shared<checker_texture>(color(1, 1, 1), color(0.5, 0.5, 0.5)));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    return world;
}

hittable_list two_perlin_spheres()
{
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    auto imgtext = make_shared<image_texture>("texture images/earthmap.jpg");
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(imgtext)));

    return objects;
}

hittable_list two_spheres()
{
    hittable_list objects;

    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(color(0, 0.7, 0))));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(color(1, 0, 0))));
    objects.add(make_shared<yz_rect>(0, 20, -25, 25, -10, make_shared<diffuse_light>(color(1, 1, 1))));
    return objects;
}

hittable_list cornell_box()
{
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    return objects;
}