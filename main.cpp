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
#include "src/box.h"
#include "src/constant_medium.h"
#include "src/stb_image_write.h"

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
    if(h.hit(r, 0.001, infinity, rec)) 
    {
        ray scattered;
        color attenuation;
        if(rec.mat_ptr->scatter(r, rec, scattered, attenuation))
            return attenuation * ray_color(scattered, h, max_depth-1);
            
        return rec.mat_ptr->emitted();  
    }

    // For black background
    // return color(0, 0, 0);

    // For sky background 
    // /*
    auto unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1);
    return (1-t) * color(1, 1, 1) + t * color(0.5, 0.7, 1);
    // */
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

void output_ppm(std::vector<color>& pixelColors, double scale, int img_width, int img_height);
void output_jpg(std::vector<color>& pixelColors, double scale, int img_width, int img_height);

// Forward declarations of scene functions
hittable_list random_scene();
hittable_list two_perlin_spheres();
hittable_list two_spheres();
hittable_list cornell_box();
hittable_list final_scene();

// Main entry function
int main()
{
    thread_pool pool;

    // Image dimensions
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1000;  
    const int max_depth = 50;

    std::vector<color> pixelColors(image_width * image_height);

    // Camera setup
    point3 cam_lookfrom(5, 6, 7);  
    point3 cam_lookat(0, 1, 0);
    // point3 cam_lookfrom(278, 278, -800);  
    // point3 cam_lookat(278, 278, 0);
    double aperture = 0;
    double dist_to_focus = 10.0;    
    camera cam(cam_lookfrom, cam_lookat, vec3(0, 1, 0), 40, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    render_info rend_inf(image_width, image_height, samples_per_pixel, samples_per_pixel, max_depth, cam);

    // Scene setup
    // hittable_list scene_list = random_scene();
    // hittable_list scene_list = cornell_box();
    // hittable_list scene_list = final_scene();
    hittable_list scene_list = two_perlin_spheres();
    bvh_node scene(scene_list, 0.0, 1.0);

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
    

    // Scales color values stored in pixelColors by the sampling rate,
    // then concatenates it to a single output string
    auto scale = 1.0 / samples_per_pixel;

    

    // The final output string with all the pixel RGB values
    // is written to the output file in one go
    // output_ppm(pixelColors, scale, image_width, image_height);
    output_jpg(pixelColors, scale, image_width, image_height);
    

    return 0;
}


void output_ppm(std::vector<color>& pixelColors, double scale, int img_width, int img_height)
{
    // PPM file data
    std::ofstream outputImage("output.ppm", std::ios::trunc);
    outputImage << "P3\n" << img_width << " " << img_height << "\n255\n";

    std::string outputImageString;
    for(auto& px_color : pixelColors)
    {
        px_color *= scale;
        outputImageString += write_color(px_color);
    }

    outputImage << outputImageString;
    outputImage.close();
}

void output_jpg(std::vector<color>& pixelColors, double scale, int img_width, int img_height)
{
    unsigned char* data = new unsigned char[img_width*img_height*3];
    int ix = 0;
    for(auto& col : pixelColors)
    {
        col *= scale;
        col[0] = sqrt(col[0]);
        col[1] = sqrt(col[1]);
        col[2] = sqrt(col[2]);

        data[ix++] = static_cast<int>(255 * clamp(col[0], 0, 1));
        data[ix++] = static_cast<int>(255 * clamp(col[1], 0, 1));
        data[ix++] = static_cast<int>(255 * clamp(col[2], 0, 1));
    }

    if(!stbi_write_jpg("output.jpg", img_width, img_height, 3, data, 90))
    {
        std::cerr << "Failed to write image to file.";
    }
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
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

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
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295)); 

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65)); 

    objects.add(box1);
    objects.add(box2);
    // objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    // objects.add(make_shared<constant_medium>(box2, 0.01, color(0, 0, 0)));

    return objects;
}

hittable_list final_scene() 
{
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));
    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;
            boxes1.add(make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }
    hittable_list objects;
    objects.add(make_shared<bvh_node>(boxes1, 0, 1));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));

    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));
    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
    point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));
    auto emat = make_shared<lambertian>(make_shared<image_texture>("texture images/earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) 
    {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }
    objects.add(make_shared<translate>(
    make_shared<rotate_y>(
    make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
    vec3(-100,270,395)
    )   
    );
    return objects;
}