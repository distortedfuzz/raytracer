#ifndef CODE_TEMPLATE_SPHERE_H
#define CODE_TEMPLATE_SPHERE_H

#include <vector>
#include <cmath>
#include "math.h"
#include "parser.h"

float sphere_intersection_parameter(const parser::Vec3f &starting_position,
                                    const parser::Vec3f &direction,
                                    const std::vector<parser::Sphere> &spheres,
                                    const std::vector<parser::Vec3f> &vertex_data,
                                    int& sphere_id, int& material_id);


parser::Vec3f get_sphere_normal(const parser::Vec3f &point,
                                const parser::Sphere &sphere,
                                const std::vector<parser::Vec3f> &vertex_data);

bool sphere_point_in_shadow(const parser::Vec3f &point,
                            const parser::Sphere &sphere,
                            float shadow_ray_epsilon ,
                            const parser::PointLight &light,
                            const std::vector<parser::Sphere> &spheres,
                            const std::vector<parser::Mesh> &meshes,
                            const std::vector<parser::Vec3f> &vertex_data);

parser::Vec3f sphere_diffuse_light(const parser::Material &material,
                                   const parser::PointLight &light,
                                   const parser::Sphere &sphere,
                                   const parser::Vec3f &point,
                                   const std::vector<parser::Vec3f> &vertex_data);

parser::Vec3f sphere_specular_light(const parser::Material &material,
                                    const parser::PointLight &light,
                                    const parser::Sphere &sphere,
                                    const parser::Vec3f &point,
                                    const std::vector<parser::Vec3f> &vertex_data,
                                    const parser::Vec3f &cam_position);
#endif
