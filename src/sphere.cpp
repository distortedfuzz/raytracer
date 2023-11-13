#include <vector>
#include <cmath>
#include "math.h"
#include "mesh.h"
#include "sphere.h"
#include "parser.h"

float sphere_intersection_parameter(const parser::Vec3f &starting_position,
                                    const parser::Vec3f &direction,
                                    const std::vector<parser::Sphere> &spheres,
                                    const std::vector<parser::Vec3f> &vertex_data,
                                    int& sphere_id, int& material_id){
    float smallest_param = 0.0;
    float parameter;
    float parameter2;
    float discriminant;
    bool found = false;
    int i = 0;
    parser::Vec3f com_vector;

    for(auto & sphere: spheres){
        com_vector.x = starting_position.x- vertex_data[sphere.center_vertex_id-1].x;
        com_vector.y = starting_position.y- vertex_data[sphere.center_vertex_id-1].y;
        com_vector.z = starting_position.z- vertex_data[sphere.center_vertex_id-1].z;

        discriminant = pow(dot_product(direction,com_vector),2)
                       -(dot_product(direction, direction)*
                         (dot_product(com_vector,com_vector)-pow(sphere.radius,2)));


        if(discriminant == 0){
            parameter = (- dot_product(direction,com_vector) +
                         sqrt(discriminant))/ dot_product(direction,direction);

            if(!found){
                smallest_param = parameter;
                found = true;
                sphere_id = i;
            }else{
                if(parameter<smallest_param){
                    smallest_param = parameter;
                    sphere_id = i;
                }
            }
        }else if(discriminant > 0){

            parameter = (- dot_product(direction,com_vector) +
                         sqrt(discriminant))/ dot_product(direction, direction);
            parameter2 = (- dot_product(direction,com_vector) -
                          sqrt(discriminant))/ dot_product(direction,direction);
            if(parameter> parameter2){
                if(!found){
                    smallest_param = parameter2;
                    found = true;
                    sphere_id = i;
                }else{
                    if(parameter2<smallest_param){
                        smallest_param = parameter2;
                        sphere_id = i;
                    }
                }

            }else{
                if(!found){
                    smallest_param = parameter;
                    found = true;
                    sphere_id = i;
                }else{
                    if(parameter<smallest_param){
                        smallest_param = parameter;
                        sphere_id = i;
                    }
                }
            }
        }
        i++;
    }

    material_id = spheres[sphere_id].material_id;
    return smallest_param;

}

parser::Vec3f get_sphere_normal(const parser::Vec3f &point,
                                const parser::Sphere &sphere,
                                const std::vector<parser::Vec3f> &vertex_data){
    parser::Vec3f result;

    result = vector_divide(vector_extract(point, vertex_data[sphere.center_vertex_id -1]), sphere.radius);

    return result;
}

bool sphere_point_in_shadow(const parser::Vec3f &point,
                            const parser::Sphere &sphere,
                            float shadow_ray_epsilon ,
                            const parser::PointLight &light,
                            const std::vector<parser::Sphere> &spheres,
                            const std::vector<parser::Mesh> &meshes,
                            const std::vector<parser::Vec3f> &vertex_data){

    parser::Vec3f normal = get_sphere_normal(point, sphere, vertex_data);
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

parser::Vec3f sphere_diffuse_light(const parser::Material &material,
                                   const parser::PointLight &light,
                                   const parser::Sphere &sphere,
                                   const parser::Vec3f &point,
                                   const std::vector<parser::Vec3f> &vertex_data){

    parser::Vec3f result;

    parser::Vec3f normal = get_sphere_normal(point, sphere, vertex_data);

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

parser::Vec3f sphere_specular_light(const parser::Material &material,
                                    const parser::PointLight &light,
                                    const parser::Sphere &sphere,
                                    const parser::Vec3f &point,
                                    const std::vector<parser::Vec3f> &vertex_data,
                                    const parser::Vec3f &cam_position){

    parser::Vec3f result;

    parser::Vec3f normal = get_sphere_normal(point, sphere, vertex_data);

    parser::Vec3f cam_vector = vector_divide(vector_extract(cam_position, point), vector_magnitude(vector_extract(cam_position, point)));

    float distance;
    float cosa;

    parser::Vec3f point_light_direction = vector_divide(vector_extract(light.position, point), vector_magnitude(vector_extract(light.position, point)));

    parser::Vec3f half_vector = vector_divide(vector_add(point_light_direction, cam_vector), vector_magnitude(vector_add(point_light_direction, cam_vector)));

    distance = vector_magnitude(vector_extract(light.position, point));

    cosa = dot_product(normal, half_vector)/ (vector_magnitude(normal)* vector_magnitude(half_vector));

    if(acos(dot_product(normal, point_light_direction)/ (vector_magnitude(normal)* vector_magnitude(point_light_direction)))>= 3.14/2){
        result.x = 0;
        result.y = 0;
        result.z = 0;
        return result;
    }
    result.x = material.specular.x * pow(cosa, material.phong_exponent) * (light.intensity.x / pow(distance,2));
    result.y = material.specular.y * pow(cosa, material.phong_exponent) * (light.intensity.y / pow(distance,2));
    result.z = material.specular.z * pow(cosa, material.phong_exponent) * (light.intensity.z / pow(distance,2));


    return result;
}