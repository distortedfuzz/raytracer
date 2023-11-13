#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <atomic>
#include <thread>

#include "color.h"
#include "camera.h"
#include "triangle.h"
#include "math.h"
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{

    parser::Scene scene;
    scene.loadFromXml(argv[1]);

    for(auto & cam : scene.cameras){
        int height = cam.image_height;
        int width = cam.image_width;

        std::vector<parser::Vec3f> directions = get_directions(cam);

        std::vector<std::vector<parser::Vec3f>> triangle_normals;
        std::vector<parser::Vec3f> one_mesh;

        for(auto & mesh: scene.meshes){
            one_mesh.clear();
            for(auto & face: mesh.faces){
                one_mesh.push_back(get_triangle_normal(face, scene.vertex_data));

            }
            triangle_normals.push_back(one_mesh);
        }

        unsigned char* image = new unsigned char [width * height * 3];


        uint32_t processing_unit_count = std::thread::hardware_concurrency();
        if (processing_unit_count == 0) {
            processing_unit_count = 8;
        }

        std::vector<std::thread> processing_units;
        processing_units.reserve(processing_unit_count);

        std::atomic<uint32_t> cursor = 0;

        for (uint32_t i = 0; i < processing_unit_count; i++) {
            processing_units.push_back(std::thread([&]() {
                while (1) {
                    uint32_t j = cursor.fetch_add(1, std::memory_order_relaxed);
                    if (j >= height) {
                        break;
                    }

                    parser::Vec3f current_direction;
                    parser::Vec3f color;
                    for (uint32_t i = 0; i < width; i++) {
                        int depth = 0;
                        current_direction = directions[(j*width)+i];
                        color = compute_color(scene, cam.position, current_direction,triangle_normals, scene.triangles, scene.meshes,
                                              scene.spheres, depth);

                        image[((j*width)+i)*3] = std::round(clamp(color.x));
                        image[(((j*width)+i)*3)+1] = std::round(clamp(color.y));
                        image[(((j*width)+i)*3)+2] = std::round(clamp(color.z));

                    }

                }
            }));
        }

        for (auto &processing_unit: processing_units) {
            processing_unit.join();
        }

        write_ppm(cam.image_name.data(), image, width, height);
    }

}
