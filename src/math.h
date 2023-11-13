#ifndef CODE_TEMPLATE_MATH_H
#define CODE_TEMPLATE_MATH_H
#include <vector>
#include <cmath>
#include "parser.h"

inline parser::Vec3f cross_product(parser::Vec3f vec1, parser::Vec3f vec2){
    parser::Vec3f product;

    product.x = (vec1.y*vec2.z - vec1.z*vec2.y);
    product.y = -(vec1.x*vec2.z - vec1.z*vec2.x);
    product.z = (vec1.x*vec2.y - vec1.y*vec2.x);

    return product;
}

inline float dot_product(parser::Vec3f vec1, parser::Vec3f vec2){
    return (vec1.x*vec2.x) + (vec1.y*vec2.y) + (vec1.z*vec2.z);
}

inline parser::Vec3f vector_add(parser::Vec3f vec1, parser::Vec3f vec2){
    parser::Vec3f vec;
    vec.x = vec1.x + vec2.x;
    vec.y = vec1.y + vec2.y;
    vec.z = vec1.z + vec2.z;

    return vec;
}

inline parser::Vec3f vector_extract(parser::Vec3f vec1, parser::Vec3f vec2){
    parser::Vec3f vec;
    vec.x = vec1.x - vec2.x;
    vec.y = vec1.y - vec2.y;
    vec.z = vec1.z - vec2.z;

    return vec;
}

inline float vector_magnitude(parser::Vec3f vec1){

    float result;
    result = sqrt(pow(vec1.x,2) + pow(vec1.y,2) + pow(vec1.z,2));
    return result;

}

inline parser::Vec3f vector_multiply(parser::Vec3f vec1, float multiplier){
    parser::Vec3f vec;
    vec.x = vec1.x*multiplier;
    vec.y = vec1.y*multiplier;
    vec.z = vec1.z*multiplier;

    return vec;
}

inline parser::Vec3f vector_divide(parser::Vec3f vec1, float div){
    parser::Vec3f vec;
    vec.x = vec1.x/div;
    vec.y = vec1.y/div;
    vec.z = vec1.z/div;

    return vec;
}

inline float get_3x3_determinant(parser::Vec3f vec1, parser::Vec3f vec2, parser::Vec3f vec3){

    float result;

    result = vec1.x*((vec2.y*vec3.z)-(vec3.y*vec2.z))
             - vec2.x*((vec1.y*vec3.z)-(vec3.y*vec1.z))
             + vec3.x*((vec1.y*vec2.z)-(vec2.y*vec1.z));

    return result;
}

inline float clamp(float number){
    if(number<=0){
        number = 0;
    }else if(number>255){
        number = 255;
    }
    return number;
}

#endif
