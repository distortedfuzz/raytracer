#include <vector>
#include <iostream>
#include <cmath>
#include "math.h"
#include "camera.h"
#include "parser.h"

std::vector<parser::Vec3f> get_directions(const parser::Camera &cam){
    parser::Vec3f ray;
    parser::Vec3f center_point;
    parser::Vec3f top_left_point;
    parser::Vec3f point_s;
    parser::Vec3f w_neg_vector = vector_divide(cam.gaze, vector_magnitude(cam.gaze));
    parser::Vec3f v_vector = vector_divide(cam.up, vector_magnitude(cam.up));
    parser::Vec3f u_vector = vector_divide(cross_product(w_neg_vector, v_vector), vector_magnitude(cross_product(w_neg_vector, v_vector)));

    float width = cam.near_plane.y - cam.near_plane.x;
    float height = cam.near_plane.w - cam.near_plane.z;


    std::vector<parser::Vec3f> rays;

    float pixel_width = width/cam.image_width;
    float pixel_height = height/cam.image_height;

    center_point = vector_add(cam.position, vector_multiply(w_neg_vector, cam.near_distance));
    top_left_point = vector_add(vector_add(vector_multiply(u_vector, cam.near_plane.x),
                                               vector_multiply(v_vector, cam.near_plane.w)), center_point);
    float coordinate_u;
    float coordinate_v;


    for(int y = 0; y<cam.image_height; y++){

        for(int x = 0; x<cam.image_width; x++){
            coordinate_u = ((x+0.5)*pixel_width);
            coordinate_v = (-(y+0.5)*pixel_height);

            point_s = vector_add(vector_add(top_left_point, vector_multiply(u_vector,coordinate_u)),vector_multiply(v_vector,coordinate_v));


            ray = vector_divide(vector_extract(point_s, cam.position), vector_magnitude(vector_extract(point_s, cam.position)));

            rays.push_back(ray);
        }
    }

    return rays;
}