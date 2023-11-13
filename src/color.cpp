#include <vector>
#include <cmath>
#include <iostream>
#include "math.h"
#include "mesh.h"
#include "triangle.h"
#include "color.h"
#include "sphere.h"
#include "parser.h"

parser::Vec3f compute_color(const parser::Scene &scene, const parser::Vec3f &pos, const parser::Vec3f &direction,
                            const std::vector<std::vector<parser::Vec3f>> &triangle_normals,
                            const std::vector<parser::Triangle> &triangles, const std::vector<parser::Mesh> &meshes,
                            const std::vector<parser::Sphere> &spheres,int & depth){

    parser::Vec3f result;
    result.x = 0;
    result.y = 0;
    result.z = 0;

    if(depth == scene.max_recursion_depth+1){

        return result;
    }
    depth++;

    bool sphere = false;
    bool triangle = false;
    bool mesh = false;

    int sphere_id = 0;
    int mesh_id = 0;
    int face_id = 0;
    int triangle_id = 0;

    int mesh_material_id = 0;
    int sphere_material_id = 0;
    int triangle_material_id = 0;

    float parameter_sphere = 0.0;
    float parameter_mesh = 0.0;
    float parameter_triangle = 0.0;

    if(!meshes.empty()){
        parameter_mesh = mesh_intersection_parameter(pos,
                                                     direction,
                                                     meshes,
                                                     scene.vertex_data,mesh_id, face_id, mesh_material_id);
    }

    if(!spheres.empty()){
        parameter_sphere = sphere_intersection_parameter(pos,
                                                         direction,
                                                         spheres,scene.vertex_data ,sphere_id, sphere_material_id);
    }

    if(!triangles.empty()){
        parameter_triangle = triangle_intersection_parameter(pos,
                                                             direction,
                                                             triangles, scene.vertex_data, triangle_id, triangle_material_id);
    }

    if(parameter_sphere == 0.0 && parameter_mesh == 0.0 && parameter_triangle == 0.0){
        if(depth == 1){
            result.x = scene.background_color.x;
            result.y = scene.background_color.y;
            result.z = scene.background_color.z;
        }else{
            result.x = 0;
            result.y = 0;
            result.z = 0;
        }

    }else{

        if(parameter_sphere != 0.0 ){
            if((parameter_triangle == 0.0 && parameter_mesh == 0.0)||
               (parameter_triangle == 0.0 && parameter_mesh != 0.0 && parameter_sphere< parameter_mesh)||
               (parameter_triangle != 0.0 && parameter_mesh == 0.0 && parameter_sphere< parameter_triangle)||
               (parameter_triangle != 0.0 && parameter_mesh != 0.0 && parameter_sphere< parameter_mesh &&
                parameter_sphere< parameter_triangle)){
                sphere = true;
            }
        }

        if(parameter_triangle != 0.0 && !sphere){
            if((parameter_sphere == 0.0 && parameter_mesh == 0.0)||
               (parameter_sphere == 0.0 && parameter_mesh != 0.0 && parameter_triangle< parameter_mesh)||
               (parameter_sphere != 0.0 && parameter_mesh == 0.0 && parameter_triangle< parameter_sphere)||
               (parameter_sphere != 0.0 && parameter_mesh != 0.0 && parameter_triangle< parameter_mesh &&
                parameter_triangle< parameter_sphere)){
                triangle = true;
            }
        }

        if(!sphere && ! triangle){
            mesh = true;
        }

        if(mesh){

            parser::Vec3f triangle_point;
            triangle_point = vector_add(pos,vector_multiply(direction, parameter_mesh));

            parser::Vec3f ambient_l = ambient_light(scene.materials[mesh_material_id-1], scene.ambient_light);

             result.x += ambient_l.x;
             result.y += ambient_l.y;
             result.z += ambient_l.z;

            parser::Vec3f normal = triangle_normals[mesh_id][face_id];

            if(scene.materials[mesh_material_id-1].is_mirror){

                parser::Vec3f mirror_color;

                parser::Vec3f reflection_ray = vector_add(vector_multiply(normal, 2* dot_product(normal,
                                                                                                 vector_multiply(direction,-1))), direction);
                mirror_color = compute_color(scene, vector_add(triangle_point, vector_multiply(normal, scene.shadow_ray_epsilon)),
                                             reflection_ray,triangle_normals,triangles, meshes, spheres, depth);

                result.x += (mirror_color.x * scene.materials[mesh_material_id-1].mirror.x);
                result.y += (mirror_color.y * scene.materials[mesh_material_id-1].mirror.y);
                result.z += (mirror_color.z * scene.materials[mesh_material_id-1].mirror.z);
            }

            for(auto &light: scene.point_lights){
                bool in_shadow = triangle_point_in_shadow(triangle_point, scene.meshes[mesh_id].faces[face_id], scene.shadow_ray_epsilon, light, scene.spheres, scene.meshes, scene.vertex_data);
                if(!in_shadow){

                    parser::Vec3f diffuse_l = triangle_diffuse_light(normal,scene.materials[mesh_material_id-1], light,scene.meshes[mesh_id].faces[face_id],triangle_point,scene.vertex_data);
                    parser::Vec3f specular_l = triangle_specular_light(normal,scene.materials[mesh_material_id-1], light, scene.meshes[mesh_id].faces[face_id], triangle_point,scene.vertex_data, pos);

                    result.x += (diffuse_l.x + specular_l.x);
                    result.y += (diffuse_l.y + specular_l.y);
                    result.z += (diffuse_l.z + specular_l.z);
                }

            }

        }else if(sphere){

            parser::Vec3f sphere_point;
            sphere_point = vector_add(pos,vector_multiply(direction, parameter_sphere));

            parser::Vec3f ambient_l = ambient_light(scene.materials[sphere_material_id-1], scene.ambient_light);

            result.x += ambient_l.x;
            result.y += ambient_l.y;
            result.z += ambient_l.z;

            parser::Vec3f normal = get_sphere_normal(sphere_point, spheres[sphere_id], scene.vertex_data);

            if(scene.materials[sphere_material_id-1].is_mirror){

                parser::Vec3f reflection_ray = vector_add(vector_multiply(normal, -2* dot_product(normal,
                                                                                                 vector_multiply(direction,-1))), direction);


                parser::Vec3f mirror_color = compute_color(scene, sphere_point,
                                             reflection_ray,triangle_normals,triangles, meshes, spheres, depth);
                result.x += (mirror_color.x * scene.materials[sphere_material_id-1].mirror.x);
                result.y += (mirror_color.y * scene.materials[sphere_material_id-1].mirror.y);
                result.z += (mirror_color.z * scene.materials[sphere_material_id-1].mirror.z);

            }

            for(auto & light: scene.point_lights){

                bool in_shadow = sphere_point_in_shadow(sphere_point, spheres[sphere_id],scene.shadow_ray_epsilon, light, scene.spheres, scene.meshes, scene.vertex_data);
                if(!in_shadow){
                    parser::Vec3f diffuse_l = sphere_diffuse_light(scene.materials[sphere_material_id-1], light, spheres[sphere_id], vector_add(sphere_point, vector_multiply(normal,scene.shadow_ray_epsilon)),scene.vertex_data);
                    parser::Vec3f specular_l = sphere_specular_light(scene.materials[sphere_material_id-1], light, spheres[sphere_id], vector_add(sphere_point, vector_multiply(normal,scene.shadow_ray_epsilon)),scene.vertex_data, pos);


                    result.x += (diffuse_l.x + specular_l.x);
                    result.y += (diffuse_l.y + specular_l.y);
                    result.z += (diffuse_l.z + specular_l.z);
                }

            }

        }else{
            parser::Vec3f triangle_point;
            triangle_point = vector_add(pos,vector_multiply(direction, parameter_triangle));

            parser::Vec3f ambient_l = ambient_light(scene.materials[triangle_material_id-1], scene.ambient_light);

            result.x += ambient_l.x;
            result.y += ambient_l.y;
            result.z += ambient_l.z;

            parser::Vec3f normal = get_triangle_normal(scene.triangles[triangle_id].indices,scene.vertex_data);

            if(scene.materials[triangle_material_id-1].is_mirror){

                parser::Vec3f mirror_color;
                parser::Vec3f reflection_ray = vector_add(vector_multiply(normal, 2* dot_product(normal,
                                                                                                 vector_multiply(direction,-1))), direction);

                mirror_color = compute_color(scene, vector_add(triangle_point, vector_multiply(normal, scene.shadow_ray_epsilon)), reflection_ray,
                                             triangle_normals,triangles, meshes, spheres, depth);

                result.x += (mirror_color.x * scene.materials[triangle_material_id-1].mirror.x);
                result.y += (mirror_color.y * scene.materials[triangle_material_id-1].mirror.y);
                result.z += (mirror_color.z * scene.materials[triangle_material_id-1].mirror.z);
            }


            for(auto &light: scene.point_lights){

                bool in_shadow = triangle_point_in_shadow(triangle_point, scene.triangles[triangle_id].indices, scene.shadow_ray_epsilon, light, scene.spheres, scene.meshes, scene.vertex_data);
                if(!in_shadow){

                    parser::Vec3f diffuse_l = triangle_diffuse_light(normal, scene.materials[triangle_material_id-1], light,scene.triangles[triangle_id].indices,triangle_point,scene.vertex_data);
                    parser::Vec3f specular_l = triangle_specular_light(normal, scene.materials[triangle_material_id-1], light,scene.triangles[triangle_id].indices, triangle_point,scene.vertex_data, pos);

                    result.x += (diffuse_l.x + specular_l.x);
                    result.y += (diffuse_l.y + specular_l.y);
                    result.z += (diffuse_l.z + specular_l.z);
                }

            }
        }

    }

    return result;
}


parser::Vec3f ambient_light(const parser::Material &material, const parser::Vec3f &ambient_light){

    parser::Vec3f result;
    result.x = material.ambient.x* ambient_light.x;
    result.y = material.ambient.y* ambient_light.y;
    result.z = material.ambient.z* ambient_light.z;
    return result;

}