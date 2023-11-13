#ifndef CODE_TEMPLATE_COLOR_H
#define CODE_TEMPLATE_COLOR_H
#include <vector>
#include <cmath>
#include "parser.h"

parser::Vec3f compute_color(const parser::Scene &scene, const parser::Vec3f &pos,
                            const parser::Vec3f &direction, const std::vector<std::vector<parser::Vec3f>> &triangle_normals,
                            const std::vector<parser::Triangle> &triangles, const std::vector<parser::Mesh> &meshes,
                            const std::vector<parser::Sphere> &spheres,
                            int & depth);

parser::Vec3f ambient_light(const parser::Material &material,const parser::Vec3f &ambient_light);
#endif
