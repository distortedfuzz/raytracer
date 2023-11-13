#include <vector>
#include <cmath>
#include <iostream>
#include "math.h"
#include "mesh.h"
#include "sphere.h"
#include "triangle.h"
#include "parser.h"

parser::Vec3f get_triangle_normal(const parser::Face &face,
                                  const std::vector<parser::Vec3f> &vertex_data){
    parser::Vec3f result;

    parser::Vec3f p1 = vertex_data[face.v0_id-1];
    parser::Vec3f p2 = vertex_data[face.v1_id-1];
    parser::Vec3f p3 = vertex_data[face.v2_id-1];

    parser::Vec3f vec1 = vector_extract(p1, p2);
    parser::Vec3f vec2 = vector_extract(p3, p2);
    result = vector_divide(cross_product(vec2, vec1), vector_magnitude(cross_product(vec2, vec1)));

    return result;
}

float triangle_intersection_parameter(const parser::Vec3f &cam_position,
                                      const parser::Vec3f &direction,
                                      const std::vector<parser::Triangle> &triangles,
                                      const std::vector<parser::Vec3f> &vertices,
                                      int &triangle_id, int &material_id){

    float det_a;
    float parameter;
    float beta;
    float gamma;
    float min_parameter = 0.0;
    bool found = false;
    int i = 0;

    for(auto & triangle: triangles){
        parser::Vec3f p1 = vertices[triangle.indices.v0_id-1];
        parser::Vec3f p2 = vertices[triangle.indices.v1_id-1];
        parser::Vec3f p3 = vertices[triangle.indices.v2_id-1];

        det_a = get_3x3_determinant(vector_extract(p1, p2), vector_extract(p1, p3), direction);

        beta = get_3x3_determinant(vector_extract(p1, cam_position), vector_extract(p1, p3), direction)/ det_a;

        gamma = get_3x3_determinant(vector_extract(p1,p2), vector_extract(p1, cam_position), direction)/ det_a;

        parameter = get_3x3_determinant(vector_extract(p1, p2), vector_extract(p1, p3), vector_extract(p1, cam_position))/ det_a;

        if(gamma>=0  && beta>=0 && (beta +gamma)<=1 && parameter >0){
            if(!found){

                found = true;
                triangle_id = i;
                min_parameter = parameter;
            }
            if(parameter< min_parameter){
                triangle_id = i;
                min_parameter = parameter;
            }

        }
        i++;
    }


    material_id = triangles[triangle_id].material_id;
    return min_parameter;
}

bool triangle_point_in_shadow(const parser::Vec3f &point,
                              const parser::Face &face,
                              float shadow_ray_epsilon,
                              const parser::PointLight &light,
                              const std::vector<parser::Sphere> &spheres,
                              const std::vector<parser::Mesh> &meshes,
                              const std::vector<parser::Vec3f> &vertex_data){

    parser::Vec3f normal = get_triangle_normal(face, vertex_data);
    parser::Vec3f offset_point = vector_add(point, vector_multiply(normal, shadow_ray_epsilon));

    float parameter = 0.0;


        int a = 0;
        int b = 0;
        int c = 0;
        parser::Vec3f point_light_vector = vector_divide(vector_extract(light.position, offset_point), vector_magnitude(vector_extract(light.position, offset_point)));

        for(auto &sphere: spheres){

            parameter = sphere_intersection_parameter(offset_point, point_light_vector,spheres, vertex_data, a,b);

            if(parameter> 0.0){

                return true;
            }
        }

        for(auto &mesh: meshes){
            parameter = mesh_intersection_parameter(offset_point, point_light_vector, meshes, vertex_data,a,b,c);
            if(parameter> 0.0){
                return true;
            }
        }


    return false;
}

parser::Vec3f triangle_diffuse_light(const parser::Vec3f &normal,
                                     const parser::Material &material,
                                     const parser::PointLight &light,
                                     const parser::Face &face,
                                     const parser::Vec3f &point,
                                     const std::vector<parser::Vec3f> &vertex_data){

    parser::Vec3f result;

    float distance;

    float cost;


    parser::Vec3f point_light_direction = vector_divide(vector_extract(light.position, point), vector_magnitude(vector_extract(light.position, point)));

    distance = vector_magnitude(vector_extract(light.position, point));

    cost = dot_product(normal, point_light_direction)/ (vector_magnitude(normal)* vector_magnitude(point_light_direction));


    result.x = material.diffuse.x * cost * (light.intensity.x / pow(distance,2));
    result.y = material.diffuse.y * cost * (light.intensity.y / pow(distance,2));
    result.z = material.diffuse.z * cost * (light.intensity.z / pow(distance,2));

    return result;
}

parser::Vec3f triangle_specular_light(const parser::Vec3f &normal,
                                      const parser::Material &material,
                                      const parser::PointLight &light,
                                      const parser::Face &face, const parser::Vec3f &point,
                                      const std::vector<parser::Vec3f> &vertex_data, const parser::Vec3f &cam_position){


    parser::Vec3f result;

    parser::Vec3f cam_vector = vector_divide(vector_extract(cam_position, point), vector_magnitude(vector_extract(cam_position, point)));

    float distance;
    float cosa;
    float zero = 0.0;

    parser::Vec3f point_light_direction = vector_divide(vector_extract(light.position, point), vector_magnitude(vector_extract(light.position, point)));

    parser::Vec3f half_vector = vector_divide(vector_add(point_light_direction, cam_vector), vector_magnitude(vector_add(point_light_direction, cam_vector)));

    distance = vector_magnitude(vector_extract(light.position, point));

    cosa = dot_product(normal, half_vector)/ (vector_magnitude(normal)* vector_magnitude(half_vector));
    if(acos(dot_product(normal, point_light_direction)/ (vector_magnitude(normal)* vector_magnitude(point_light_direction)))>3.14/2){

         result.x = 0.0;
         result.y = 0.0;
         result.z = 0.0;
         return result;
    }

    result.x = material.specular.x * pow(cosa, material.phong_exponent) * (light.intensity.x / pow(distance,2));
    result.y = material.specular.y * pow(cosa, material.phong_exponent) * (light.intensity.y / pow(distance,2));
    result.z = material.specular.z * pow(cosa, material.phong_exponent) * (light.intensity.z / pow(distance,2));


    return result;
}