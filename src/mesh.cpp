#include <vector>
#include <iostream>
#include <cmath>
#include "math.h"
#include "mesh.h"
#include "parser.h"

float mesh_intersection_parameter(const parser::Vec3f &cam_position, const parser::Vec3f &direction,
                                  const std::vector<parser::Mesh> &meshes,
                                  const std::vector<parser::Vec3f> &vertices,
                                  int &mesh_id, int &face_id, int &material_id){

    float det_a;
    float parameter;
    float beta;
    float gamma;
    float min_parameter = 0.0;
    bool found = false;
    int face_loop = 0;
    int mesh_loop = 0;

    for(auto &mesh : meshes){
        face_loop = 0;
        for (auto &face : mesh.faces){
            parser::Vec3f p1 = vertices[face.v0_id-1];
            parser::Vec3f p2 = vertices[face.v1_id-1];
            parser::Vec3f p3 = vertices[face.v2_id-1];

            det_a = get_3x3_determinant(vector_extract(p1, p2), vector_extract(p1, p3), direction);

            beta = get_3x3_determinant(vector_extract(p1, cam_position), vector_extract(p1, p3), direction)/ det_a;

            gamma = get_3x3_determinant(vector_extract(p1,p2), vector_extract(p1, cam_position), direction)/ det_a;

            parameter = get_3x3_determinant(vector_extract(p1, p2), vector_extract(p1, p3), vector_extract(p1, cam_position))/ det_a;

            if(gamma>=0  && beta>=0 && (beta +gamma)<=1 && parameter >0){
                if(!found){

                    found = true;
                    face_id = face_loop;
                    mesh_id = mesh_loop;
                    min_parameter = parameter;
                }
                if(parameter< min_parameter){
                    face_id = face_loop;
                    mesh_id = mesh_loop;
                    min_parameter = parameter;
                }


            }
            face_loop++;
        }
        mesh_loop++;
    }

    material_id = meshes[mesh_id].material_id;
    return min_parameter;
}