#ifndef CODE_TEMPLATE_TRIANGLE_H
#define CODE_TEMPLATE_TRIANGLE_H

#include <vector>
#include <cmath>
#include "parser.h"

parser::Vec3f get_triangle_normal(const parser::Face &face,
                                  const std::vector<parser::Vec3f> &vertex_data);

float triangle_intersection_parameter(const parser::Vec3f &cam_position,
                                      const parser::Vec3f &direction,
                                      const std::vector<parser::Triangle> &triangles,
                                      const std::vector<parser::Vec3f> &vertices,
                                      int &triangle_id, int &material_id);

bool triangle_point_in_shadow(const parser::Vec3f &point,
                              const parser::Face &face,
                              float shadow_ray_epsilon ,
                              const parser::PointLight &light,
                              const std::vector<parser::Sphere> &spheres,
                              const std::vector<parser::Mesh> &meshes,
                              const std::vector<parser::Vec3f> &vertex_data);

parser::Vec3f triangle_diffuse_light(const parser::Vec3f &normal,
                                     const parser::Material &material,
                                     const parser::PointLight &light,
                                     const parser::Face &face,
                                     const parser::Vec3f &point,
                                     const std::vector<parser::Vec3f> &vertex_data);

parser::Vec3f triangle_specular_light(const parser::Vec3f &normal,
                                      const parser::Material &material,
                                      const parser::PointLight &light,
                                      const parser::Face &face,
                                      const parser::Vec3f &point,
                                      const std::vector<parser::Vec3f> &vertex_data,
                                      const parser::Vec3f &cam_position);

#endif
