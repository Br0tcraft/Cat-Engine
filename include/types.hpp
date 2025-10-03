#pragma once

struct vec2 {
    float x, y;
    vec2(float nx, float ny) {x = nx, y = ny;}
};

struct vec2i {
    float x, y;
    vec2i(int nx, int ny) {x = nx, y = ny;}
};

struct vec3 {
    float x, y, z;
    vec3(float nx, float ny, float nz) {x = nx, y = ny; z = nz;}
};

struct vec3i {
    float x, y, z;
    vec3i(int nx, int ny, int nz) {x = nx, y = ny; z = nz;}
};

struct vec4 {
    float x, y, dx, dy;
    vec4(float nx, float ny, float ndx, float ndy) {x = nx, y = ny; dx = ndx; dy = ndy;}
};

struct vec4i {
    float x, y, dx, dy;
    vec4i(int nx, int ny, int ndx, int ndy) {x = nx, y = ny; dx = ndx; dy = ndy;}
};