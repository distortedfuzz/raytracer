#ifndef CODE_TEMPLATE_MESH_H
#define CODE_TEMPLATE_MESH_H

#include <vector>
#include <cmath>
#include "parser.h"

float mesh_intersection_parameter(const parser::Vec3f &cam_position,
                                  const parser::Vec3f &direction,
                                  const std::vector<parser::Mesh> &meshes,
                                  const std::vector<parser::Vec3f> &vertices,
                                  int &mesh_id, int &face_id, int &material_id);

#endif
