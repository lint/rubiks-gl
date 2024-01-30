
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "../mtxlib/mtx.h"
#include "../shaders/initShader.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// some vertice sizes
int num_cubie_verts = 132;

// vertex and color arrays and sizes

int num_rubiks_vertices;
vec4 *rubiks_vertices;
vec4 *rubiks_colors;

int num_sphere_vertices;
vec4 *sphere_vertices;
vec4 *sphere_colors;

int num_plane_vertices;
vec4 *plane_vertices;
vec4 *plane_colors;

vec4 *normals;

// locations of vertex shader values
GLuint model_view_location;
GLuint projection_location;
GLuint ctm_location;
GLuint light_position_location;
GLuint is_shadow_location;
GLuint plane_height_location;

// various matrices

mat4 cubie_ctms[27];
mat4 ctm = (mat4){
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};
mat4 model_view_matrix = (mat4){
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};
mat4 projection_matrix = (mat4){
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};
mat4 sphere_ctm = (mat4){
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};
mat4 plane_ctm = (mat4){
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

// variables to calculate shadow and/or lighting effects
GLfloat plane_height;
vec4 light_position;

// misc values
float curr_scale_val = 1;
int window_size  = 512;
const float pi = 3.14159265;

// states determining current animation
typedef enum {
    NONE = 0,
    ROTATING,
    ZOOMING,
    TURN_U,
    TURN_D,
    TURN_F,
    TURN_R,
    TURN_B,
    TURN_L,
    TURN_U_PRIME,
    TURN_D_PRIME,
    TURN_F_PRIME,
    TURN_R_PRIME,
    TURN_B_PRIME,
    TURN_L_PRIME,
    TRANSLATING_PLANE,
    TRANSLATING_SPHERE
} state;

// various variables about animation data

state current_state_view = NONE;
state current_state_turn = NONE;
int is_animating_view = 0;
int is_animating_turn = 0;
int is_animating_scramble = 0;
int is_animating_plane = 0;
int is_animating_sphere = 0;

int current_step_view = 0;
int max_steps_view;

int current_step_turn = 0;
int max_steps_turn = 20;

int current_step_transl_sphere = 0;
int max_steps_transl_sphere = 5;
mat4 starting_sphere_ctm;
mat4 changing_sphere_ctm;

int current_step_transl_plane = 0;
int max_steps_transl_plane = 10;
mat4 starting_plane_ctm;
mat4 changing_plane_ctm;

char *current_scramble;
int current_scramble_index = 0;

vec4 curr_eye = (vec4) {3, 3, 3, 1};
vec4 curr_at = (vec4) {0, 0, 0, 1};
vec4 starting_eye;
vec4 changing_eye;

// data that represents the rubiks cube
int cubies_to_rotate[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
int rubiks_cube[3][3][3] = {{{0,1,2},{3,4,5},{6,7,8}}, {{9,10,11},{12,13,14},{15,16,17}}, {{18,19,20},{21,22,23},{24,25,26}}};

// helper method to convert degrees to radians
float degToRad(float deg) {
    return deg * pi / 180;
}

// generates vertices to produce a sphere and fills its color array
void genSphere() {

    int verts_per_circle = 32;
    int bands_per_half_sphere = 10; // or should I do like circles_per_half_sphere

    num_sphere_vertices = (bands_per_half_sphere * verts_per_circle * 6 + (verts_per_circle * 3)) * 2;
    vec4 *verts = malloc(num_sphere_vertices * sizeof(vec4));

    float horizontal_deg_inc = 360.0 / verts_per_circle;
    float vertical_deg_inc = 90.0 / (bands_per_half_sphere + 1); // + 1 to account for the cap (top/bottom)

    // iterate for each band in the half sphere
    for (int a = 0; a < bands_per_half_sphere; a++) {

        // create the 4 vertices of the starting rectangle
        vec4 rectBR = mulMV(rot_z(degToRad(a * vertical_deg_inc)), (vec4){1, 0, 0, 1});
        vec4 rectBL = mulMV(rot_y(degToRad(horizontal_deg_inc)), rectBR);
        vec4 rectTR = mulMV(rot_z(degToRad(vertical_deg_inc)), rectBR);
        vec4 rectTL = mulMV(rot_y(degToRad(horizontal_deg_inc)), mulMV(rot_z(degToRad(vertical_deg_inc)), rectBR));

        // the start index of each band around the sphere
        int band_offset = (verts_per_circle * 6) * a;

        // creating the two triangles for first rectangle
        verts[band_offset]     = rectBR;
        verts[band_offset + 1] = rectBL;
        verts[band_offset + 2] = rectTR;
        verts[band_offset + 3] = rectBL;
        verts[band_offset + 4] = rectTL;
        verts[band_offset + 5] = rectTR;

        // iterate for each rectangle in the band (each vertice in the circle)
        for (int b = 1; b < verts_per_circle; b++) {

            // each vertex in the current rectangle is the relative one in the previous rectangle rotated about y
            for (int c = 0; c < 6; c++) {
                int curr_vert_offset = band_offset + (b * 6) + c;
                verts[curr_vert_offset] = mulMV(rot_y(degToRad(horizontal_deg_inc)), verts[curr_vert_offset - 6]);
            }
        }
    }

    int after_bands_offset = bands_per_half_sphere * verts_per_circle * 6;
    int last_band_offset = (bands_per_half_sphere - 1) * verts_per_circle * 6;

    // iterate for each triangle in the top of the half sphere
    for (int i = 0; i < verts_per_circle; i++) {

        // gets the "top right" and "top left" vertices of the rectangles in the last band, and forms a triangle with a point in the center
        verts[after_bands_offset + (i * 3)]     = verts[last_band_offset + (i * 6) + 2];
        verts[after_bands_offset + (i * 3) + 1] = verts[last_band_offset + (i * 6) + 4];
        verts[after_bands_offset + (i * 3) + 2] = (vec4){0, 1, 0, 1};
    }

    int after_half_sphere_offset = after_bands_offset + (verts_per_circle * 3);

    // iterate through every vertex generated so far and rotate it 180 deg to get the bottom half of the sphere
    for (int i = 0; i < after_half_sphere_offset; i++) {
        verts[after_half_sphere_offset + i] = mulMV(rot_z(degToRad(180)), verts[i]);
    }

    // scale and translate the sphere using its ctm
    GLfloat scale_val = 1.0 / 8;
    mat4 scale_matrix = scale(scale_val, scale_val, scale_val);
    mat4 translation_matrix = transl(0, 2, 0);
    sphere_ctm = mulMM(translation_matrix, scale_matrix);

    // generate the colors for the sphere
    sphere_colors = malloc(num_sphere_vertices * sizeof(vec4));
    for (int i = 0; i < num_sphere_vertices; i++) {
        sphere_colors[i] = (vec4){1,1,1,1};
    }

    // assign the verts array
    sphere_vertices = verts;

    printf("Num of Sphere Vertices: %d\n", num_sphere_vertices);
}

// generates vertices to produce a plane and fills its color array
void genPlane() {

    num_plane_vertices = 6;
    plane_vertices = malloc(sizeof(vec4) * num_plane_vertices);

    // creating the plane
    plane_vertices[0] = (vec4){-1,0,1,1}; // bottom left
    plane_vertices[1] = (vec4){1,0,1,1}; // bottom right
    plane_vertices[2] = (vec4){1,0,-1,1}; // top right
    plane_vertices[3] = (vec4){-1,0,1,1}; // bottom left
    plane_vertices[4] = (vec4){1,0,-1,1}; // top right
    plane_vertices[5] = (vec4){-1,0,-1,1}; // top left

    // scaling and translating the plane via its ctm
    mat4 scale_matrix = scale(100,100,100);
    mat4 translation_matrix = transl(0,-2,0);

    plane_ctm = mulMM(translation_matrix, scale_matrix);

    // filling the color array
    plane_colors = malloc(sizeof(vec4) * num_plane_vertices);

    for (int i = 0; i < num_plane_vertices; i++) {
        plane_colors[i] = (vec4){60./255, 120./255, 140./255, 1};
    }

    printf("Num of Plane Vertices: %d\n", num_plane_vertices);
}

// prints a representation of the rubiks_cube to console
void printRubiksCube() {

    printf("Rubik's Cube Layout:\n");

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                printf("%*d ", 2, rubiks_cube[i][j][k]);
            }
            printf("\n");
        }
        if (i == 0 || i == 1) {
            printf("---------\n");
        }
    }

    printf("\n");
}

// creates a rubik's cube and fills the colors array and vertices array
void genRubiksCube() {

    // num vertices
    // 6 per square * 6 sides = 36 for main faces
    // 6 per edge * 12 edges = 72 for beveled edges
    // 3 per corner * 8 vertices = 24 for beveled corners
    // 36 + 72 + 24 = 132 total per cubie
    // 132 * 27 for whole cube
    num_rubiks_vertices = num_cubie_verts * 27;
    rubiks_vertices = malloc(sizeof(vec4) * num_rubiks_vertices);

    // ---- creating the base cubie ----

    vec4 cubieVerts[num_cubie_verts];

    // -- creating the 6 main faces of the cubie --

    // vertices for a square
    vec4 squareVerts[6] =
    {{-0.5, -0.5,  0.0, 1.0},  // bottom left
     { 0.5, -0.5,  0.0, 1.0},  // bottom right
     { 0.5,  0.5,  0.0, 1.0},  // top right
     {-0.5, -0.5,  0.0, 1.0},  // bottom left
     { 0.5,  0.5,  0.0, 1.0},  // top right
     {-0.5,  0.5,  0.0, 1.0}}; // top left

    GLfloat bevel_offset = 0.1;

    for (int i = 0; i < 6; i++) {

        int offset = i * 6;

        for (int j = 0; j < 6; j++) {

            vec4 v;

            if (i == 0) { // front face
                v = mulMV(transl(0, 0, 0.5 + bevel_offset), squareVerts[j]);
            } else if (i == 1) { // right face
                v = mulMV(mulMM(transl(0.5 + bevel_offset, 0, 0), rot_y(degToRad(90))), squareVerts[j]);
            } else if (i == 2) { // back face
                v = mulMV(mulMM(transl(0, 0, -0.5 - bevel_offset), rot_y(degToRad(180))), squareVerts[j]);
            } else if (i == 3) { // left face
                v = mulMV(mulMM(transl(-0.5 - bevel_offset, 0, 0), rot_y(degToRad(-90))), squareVerts[j]);
            } else if (i == 4) { // up face
                v = mulMV(mulMM(transl(0, 0.5 + bevel_offset, 0), rot_x(degToRad(-90))), squareVerts[j]);
            } else if (i == 5) { // down face
                v = mulMV(mulMM(transl(0, -0.5 - bevel_offset, 0), rot_x(degToRad(90))), squareVerts[j]);
            }

            cubieVerts[offset + j] = v;
        }
    }

    // -- creating beveled edges of the cubie --

    int curr_edge_offset = 36;

    // up-x edges
    for (int i = 0; i < 4; i++) {

        vec4 edge_top_left;
        vec4 edge_top_right;
        vec4 edge_bottom_left  = cubieVerts[(i * 6) + 5];
        vec4 edge_bottom_right = cubieVerts[(i * 6) + 2];

        if (i == 0) {
            edge_top_left  = cubieVerts[(4 * 6) + 0]; // up bottom left
            edge_top_right = cubieVerts[(4 * 6) + 1]; // up bottom right
        } else if (i == 1) {
            edge_top_left  = cubieVerts[(4 * 6) + 1]; // up bottom right
            edge_top_right = cubieVerts[(4 * 6) + 2]; // up top right
        } else if (i == 2) {
            edge_top_left  = cubieVerts[(4 * 6) + 2]; // up top right
            edge_top_right = cubieVerts[(4 * 6) + 5]; // up top left
        } else {
            edge_top_left  = cubieVerts[(4 * 6) + 5]; // up top left
            edge_top_right = cubieVerts[(4 * 6) + 0]; // up bottom left
        }

        cubieVerts[curr_edge_offset + 0] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 1] = edge_bottom_right;
        cubieVerts[curr_edge_offset + 2] = edge_top_right;
        cubieVerts[curr_edge_offset + 3] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 4] = edge_top_right;
        cubieVerts[curr_edge_offset + 5] = edge_top_left;

        curr_edge_offset += 6;
    }

    // bottom-x edges
    for (int i = 0; i < 4; i++) {

        vec4 edge_top_left  = cubieVerts[(i * 6) + 0];
        vec4 edge_top_right = cubieVerts[(i * 6) + 1];
        vec4 edge_bottom_left;
        vec4 edge_bottom_right;

        if (i == 0) {
            edge_bottom_left  = cubieVerts[(5 * 6) + 5]; // down top left
            edge_bottom_right = cubieVerts[(5 * 6) + 4]; // down top right
        } else if (i == 1) {
            edge_bottom_left  = cubieVerts[(5 * 6) + 4]; // down top right
            edge_bottom_right = cubieVerts[(5 * 6) + 1]; // down bottom right
        } else if (i == 2) {
            edge_bottom_left  = cubieVerts[(5 * 6) + 1]; // down bottom right
            edge_bottom_right = cubieVerts[(5 * 6) + 0]; // down bottom left
        } else {
            edge_bottom_left  = cubieVerts[(5 * 6) + 0]; // down bottom left
            edge_bottom_right = cubieVerts[(5 * 6) + 5]; // down top left
        }

        cubieVerts[curr_edge_offset + 0] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 1] = edge_bottom_right;
        cubieVerts[curr_edge_offset + 2] = edge_top_right;
        cubieVerts[curr_edge_offset + 3] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 4] = edge_top_right;
        cubieVerts[curr_edge_offset + 5] = edge_top_left;

        curr_edge_offset += 6;
    }

    // vertical edges
    for (int i = 0; i < 4; i++) {

        vec4 edge_top_left     = cubieVerts[(i * 6) + 2]; // side top right
        vec4 edge_top_right    = cubieVerts[((i + 1) % 4 * 6) + 5]; // next side top left
        vec4 edge_bottom_left  = cubieVerts[(i * 6) + 1]; // side bottom right
        vec4 edge_bottom_right = cubieVerts[((i + 1) % 4 * 6) + 0]; // next side bottom left

        cubieVerts[curr_edge_offset + 0] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 1] = edge_bottom_right;
        cubieVerts[curr_edge_offset + 2] = edge_top_right;
        cubieVerts[curr_edge_offset + 3] = edge_bottom_left;
        cubieVerts[curr_edge_offset + 4] = edge_top_right;
        cubieVerts[curr_edge_offset + 5] = edge_top_left;

        curr_edge_offset += 6;
    }

    // -- creating beveled corners of the cubie--

    int curr_corner_offset = curr_edge_offset;

    // up-x-x corners
    for (int i = 0; i < 4; i++) {

        vec4 side1_corner = cubieVerts[(i * 6) + 2]; // side top right
        vec4 side2_corner = cubieVerts[((i + 1) % 4 * 6) + 5]; // next side top left
        vec4 up_corner;

        if (i == 0) {
            up_corner = cubieVerts[(4 * 6) + 1]; // up bottom right
        } else if (i == 1) {
            up_corner = cubieVerts[(4 * 6) + 2]; // up top right
        } else if (i == 2) {
            up_corner = cubieVerts[(4 * 6) + 5]; // up top left
        } else {
            up_corner = cubieVerts[(4 * 6) + 0]; // up bottom left
        }

        cubieVerts[curr_corner_offset + 0] = side1_corner;
        cubieVerts[curr_corner_offset + 1] = side2_corner;
        cubieVerts[curr_corner_offset + 2] = up_corner;

        curr_corner_offset += 3;
    }

    // down-x-x corners
    for (int i = 0; i < 4; i++) {

        vec4 side1_corner = cubieVerts[(i * 6) + 1]; // side bottom right
        vec4 side2_corner = cubieVerts[((i + 1) % 4 * 6) + 0]; // next side bottom left
        vec4 down_corner;

        if (i == 0) {
            down_corner = cubieVerts[(5 * 6) + 4]; // down top right
        } else if (i == 1) {
            down_corner = cubieVerts[(5 * 6) + 1]; // down bottom right
        } else if (i == 2) {
            down_corner = cubieVerts[(5 * 6) + 0]; // down bottom left
        } else {
            down_corner = cubieVerts[(5 * 6) + 5]; // down top left
        }

        cubieVerts[curr_corner_offset + 0] = side1_corner;
        cubieVerts[curr_corner_offset + 1] = down_corner;
        cubieVerts[curr_corner_offset + 2] = side2_corner;

        curr_corner_offset += 3;
    }

    // ---- creating the rubik's cube ----

    // -- create a single layer --

    vec4 layer[num_cubie_verts * 9];
    GLfloat cubie_between_offset = 0.05;
    GLfloat cubie_bevel_offset = 0.2;
    GLfloat cubie_transl_offset = 1 + cubie_between_offset + cubie_bevel_offset;
    int num_layer_verts = num_cubie_verts * 9;

    // layer takes the form 0,1,2/3,4,5/6,7,8

    for (int i = 0; i < 9; i++) {

        int transl_x, transl_z;

        // set the values the cubie must translate for each position in the layer
        if (i == 0) {
            transl_x = -1, transl_z = -1;
        } else if (i == 1) {
            transl_x = 0, transl_z = -1;
        } else if (i == 2) {
            transl_x = 1, transl_z = -1;
        } else if (i == 3) {
            transl_x = -1, transl_z = 0;
        } else if (i == 4) {
            transl_x = 0, transl_z = 0;
        } else if (i == 5) {
            transl_x = 1, transl_z = 0;
        } else if (i == 6) {
            transl_x = -1, transl_z = 1;
        } else if (i == 7) {
            transl_x = 0, transl_z = 1;
        } else if (i == 8) {
            transl_x = 1, transl_z = 1;
        }

        // set the cubie for each position
        for (int j = 0; j < num_cubie_verts; j++) {
            layer[(num_cubie_verts * i) + j] = mulMV(transl(cubie_transl_offset * transl_x, 0, cubie_transl_offset * transl_z), cubieVerts[j]);
        }
    }

    // -- create the three layers --

    // rubik's cube cubies will be located in the array in positions such that
    // top layer:    0,1,2    /3,4,5     /6,7,8
    // middle layer: 9,10,11  /12,13,14  /15,16,17
    // bottom layer: 18,19,20 /21,22,23  /24,25,26

    // top layer
    for (int i = 0; i < num_layer_verts; i++) {
        rubiks_vertices[i] = mulMV(transl(0, cubie_transl_offset, 0), layer[i]);
    }

    // middle layer
    for (int i = 0; i < num_layer_verts; i++) {
        rubiks_vertices[num_layer_verts + i] = layer[i];
    }

    // bottom layer
    for (int i = 0; i < num_layer_verts; i++) {
        rubiks_vertices[num_layer_verts * 2 + i] = mulMV(transl(0, -cubie_transl_offset, 0), layer[i]);
    }

    // -- resizing the rubik's cube --

    GLfloat scale_val = 1 / (6 * bevel_offset + 2 * cubie_between_offset + 3);
    mat4 scale_matrix = scale(scale_val, scale_val, scale_val);

    // iterate through all the points and translate then scale each one
    for (int i = 0; i < num_rubiks_vertices; i++) {
        rubiks_vertices[i] = mulMV(scale_matrix, rubiks_vertices[i]);
    }

    // ---- coloring the cube ----

    rubiks_colors = malloc(num_rubiks_vertices * sizeof(vec4));

    // color each vertice black to start
    for (int i = 0; i < num_rubiks_vertices; i++) {
        rubiks_colors[i] = (vec4){.1, .1, .1, 1};
    }

    // up (white)
    // cubies #: 0, 1, 2, 3, 4, 5, 6, 7, 8
    for (int i = 0; i < 9; i++) { // for each cubie on the face

        int cubie_offset = i * num_cubie_verts;
        int face_offset = 6 * 4; // 4 = up face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){1,1,1,1};
        }
    }

    // down (yellow)
    // cubies #: 18, 19, 20, 21, 22, 23, 24, 25, 26
    for (int i = 18; i < 27; i++) { // for each cubie on the face

        int cubie_offset = i * num_cubie_verts;
        int face_offset = 6 * 5; // 5 = down face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){1,1,0,1};
        }
    }

    // front (green)
    int front_cubies[9] = {6, 7, 8, 15, 16, 17, 24, 25, 26};
    for (int i = 0; i < 9; i++) { // for each cubie on the face

        int cubie_offset = front_cubies[i] * num_cubie_verts;
        int face_offset = 6 * 0; // 0 = front face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){0,1,0,1};
        }
    }

    // right (red)
    int right_cubies[9] = {2, 5, 8, 11, 14, 17, 20, 23, 26};
    for (int i = 0; i < 9; i++) { // for each cubie on the face

        int cubie_offset = right_cubies[i] * num_cubie_verts;
        int face_offset = 6 * 1; // 1 = right face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){1,0,0,1};
        }
    }

    // back (blue)
    int back_cubies[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};
    for (int i = 0; i < 9; i++) { // for each cubie on the face

        int cubie_offset = back_cubies[i] * num_cubie_verts;
        int face_offset = 6 * 2; // 2 = back face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){0,0,1,1};
        }
    }

    // left (orange)
    int left_cubies[9] = {0, 3, 6, 9, 12, 15, 18, 21, 24};
    for (int i = 0; i < 9; i++) { // for each cubie on the face

        int cubie_offset = left_cubies[i] * num_cubie_verts;
        int face_offset = 6 * 3; // 3 = left face

        for (int j = 0; j < 6; j++) { // for each vertex in each cubie
            rubiks_colors[cubie_offset + face_offset + j] = (vec4){1,0.5,0,1};
        }
    }

    printf("Num of Rubik's Cube Vertices: %d\n", num_rubiks_vertices);
}

// turns a layer in the rubiks_cube data structure and fills cubies_to_rotate with the proper ids
void turnRubiksCubeLayer(char key) {

    //printf("turning layer with move: %c\n", key);

    if (cubies_to_rotate == NULL) {
        return;
    }

    // using different configurations of the values for layer, row, and column
    // to get the ids of pieces within the rubiks_cube data structure that will be rotated for a given face

    int layer_start = -1;
    int layer_end = -1;
    int row_start = -1;
    int row_end = -1;
    int column_start = -1;
    int column_end = -1;

    // determining if the loops should go from 0 to 2 or 2 to 0, which is necessary for getting the proper order of cubies
    int layer_is_asc = 1;
    int row_is_asc = 1;
    int column_is_asc = 1;

    if (key == 'f' || key == 'F') {
        layer_start = 0;
        layer_end = 2;
        row_start = 2;
        row_end = 2;
        column_start = 0;
        column_end = 2;
    } else if (key == 'r' || key == 'R') {
        layer_start = 0;
        layer_end = 2;
        row_start = 0;
        row_end = 2;
        column_start = 2;
        column_end = 2;
        row_is_asc = 0;
    } else if (key == 'u' || key == 'U') {
        layer_start = 0;
        layer_end = 0;
        row_start = 0;
        row_end = 2;
        column_start = 0;
        column_end = 2;
    } else if (key == 'l' || key == 'L') {
        layer_start = 0;
        layer_end = 2;
        row_start = 0;
        row_end = 2;
        column_start = 0;
        column_end = 0;
    } else if (key == 'b' || key == 'B') {
        layer_start = 0;
        layer_end = 2;
        row_start = 0;
        row_end = 0;
        column_start = 0;
        column_end = 2;
        column_is_asc = 0;
    } else if (key == 'd' || key == 'D') {
        layer_start = 2;
        layer_end = 2;
        row_start = 0;
        row_end = 2;
        column_start = 0;
        column_end = 2;
        row_is_asc = 0;
    } else {
        return;
    }

    // determining if the turn is a normal clockwise turn, or a prime counterclockwise turn

    int is_prime_turn = -1;
    if (key == 'f' || key == 'r' || key == 'u' || key == 'l' || key == 'b' || key == 'd' ) {
        is_prime_turn = 0;
    } else if (key == 'F' || key == 'R' || key == 'U' || key == 'L' || key == 'B' || key == 'D' ) {
        is_prime_turn = 1;
    }

    // using the various starts and ends, make a list of the cubies that currently belong to the rotating face
    int count = 0;
    for (int i = layer_start; i <= layer_end; i++) {
        for (int j = row_start; j <= row_end; j++) {
            for (int k = column_start; k <= column_end; k++) {
                int l = layer_is_asc ? i : layer_end - i;
                int r = row_is_asc ? j : row_end - j;
                int c = column_is_asc ? k : column_end - k;
                cubies_to_rotate[count] = rubiks_cube[l][r][c];
                count++;
            }
        }
    }

    // mappings for the next order of the cubies after the turn
    int prime_turn_mapping[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
    int norm_turn_mapping[9] = {6, 3, 0, 7, 4, 1, 8, 5, 2};

    // update the rubiks_cube with the cubies new positions
    count = 0;
    for (int i = layer_start; i <= layer_end; i++) {
        for (int j = row_start; j <= row_end; j++) {
            for (int k = column_start; k <= column_end; k++) {
                int l = layer_is_asc ? i : layer_end - i;
                int r = row_is_asc ? j : row_end - j;
                int c = column_is_asc ? k : column_end - k;
                rubiks_cube[l][r][c] = cubies_to_rotate[is_prime_turn ? prime_turn_mapping[count] : norm_turn_mapping[count]];
                count++;
            }
        }
    }

    //printRubiksCube();
}

// generates a string of some random turns to scramble the rubiks cube
char *genScrambleString() {

    int scramble_length = 25;
    char *scramble = malloc(sizeof(char) * scramble_length + 1);

    char prev = 0;

    for (int i = 0; i < scramble_length; i++) {
        char next = "uUdDfFbBrRlL"[random () % 12];

        // making sure there are no moves that are the inverse of the previous moves
        while ((next < 91 && next + 32 == prev) || (next > 96 && next - 32 == prev)) {
            next = "uUdDfFbBrRlL"[random () % 12];
        }

        scramble[i] = next;
        prev = next;
    }

    scramble[scramble_length] = '\0';

    printf("generated random scramble: %s\n", scramble);

    return scramble;
}

// generates an array of normals for all current vertices
void genNormals() {

    int num_total_verts = num_rubiks_vertices + num_sphere_vertices + num_plane_vertices;
    normals = malloc(sizeof(vec4) * num_total_verts);
    int counter = 0;

    // calculate normals for rubik's cube
    for (int i = 0; i < num_rubiks_vertices; i+=3) {
        vec4 v1 = rubiks_vertices[i + 0];
        vec4 v2 = rubiks_vertices[i + 1];
        vec4 v3 = rubiks_vertices[i + 2];

        vec4 normal = normV(crossVV(subVV(v2, v1), subVV(v3, v1)));

        normals[counter + 0] = normal;
        normals[counter + 1] = normal;
        normals[counter + 2] = normal;

        counter += 3;
    }

    // calculate normals for sphere
    for (int i = 0; i < num_sphere_vertices; i+=3) {
        vec4 v1 = sphere_vertices[i + 0];
        vec4 v2 = sphere_vertices[i + 1];
        vec4 v3 = sphere_vertices[i + 2];

        vec4 normal = normV(crossVV(subVV(v2, v1), subVV(v3, v1)));

        normals[counter + 0] = normal;
        normals[counter + 1] = normal;
        normals[counter + 2] = normal;

        counter += 3;
    }

    // calculate normals for plane
    for (int i = 0; i < num_plane_vertices; i+=3) {
        vec4 v1 = plane_vertices[i + 0];
        vec4 v2 = plane_vertices[i + 1];
        vec4 v3 = plane_vertices[i + 2];

        vec4 normal = normV(crossVV(subVV(v2, v1), subVV(v3, v1)));

        normals[counter + 0] = normal;
        normals[counter + 1] = normal;
        normals[counter + 2] = normal;

        counter += 3;
    }
}

void init(void) {
    srand(time(0));

    // create the various objects
    genRubiksCube();
    genSphere();
    genPlane();
    genNormals();

    // initialize cubie_ctms
    for (int i = 0; i < 27; i++) {
        cubie_ctms[i] = (mat4){
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        };
    }

    // create matrices
    projection_matrix = frustum(-.2, .2, -.2, .2, -.2, .2); //left right bottom top near far
    model_view_matrix = look_at(curr_eye, curr_at, (vec4) {0, 1, 0, 0});

    // some print statements
    printf("Num Total Vertices: %d\n", num_rubiks_vertices + num_sphere_vertices + num_plane_vertices);
    printRubiksCube();
    printf("\n-- Rubik's Cube Turns --\n\ns or S: Scramble\nf: F Turn (CW)\nu: U Turn (CW)\nr: R Turn (CW)\nl: L Turn (CW)\nb: B Turn (CW)\nd: D Turn (CW)\nF: F' Turn (CCW)\nU: U' Turn (CCW)\nL: L' Turn (CCW)\nR: R' Turn (CCW)\nB: B' Turn (CCW)\nD: D' Turn (CCW)\n");
    printf("\n-- Viewing Movement --\n\n- or _: Zoom Out\n+ or =: Zoom In\nUp Arrow: Rotate Up\nDown Arrow: Rotate Down\nLeft Arrow: Rotate Left\nRight Arrow: Rotate Right\n");
    printf("\n-- Light Movement --\n\nx: Move in -X Direction\nX: Move in +X Direction\ny: Move in -Y Direction\nY: Move in +Y Direction\nz: Move in -Z Direction\nZ: Move in +Z Direction\n");
    printf("\n-- Plane Movement --\n\ne or E: Move Plane Up\nw or W: Move Plane Down\n");
    printf("\n-- Other --\n\nq or Q: Quit\n\n");

    // GLuint program = initShader("vshader.glsl", "fshader.glsl");
    GLuint program = initShaderDefault();
    glUseProgram(program);

    int param;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices) * 3, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_rubiks_vertices, rubiks_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_rubiks_vertices, sizeof(vec4) * num_sphere_vertices, sphere_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices), sizeof(vec4) * num_plane_vertices, plane_vertices);

    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices), sizeof(vec4) * num_rubiks_vertices, rubiks_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * (num_rubiks_vertices * 2 + num_sphere_vertices + num_plane_vertices), sizeof(vec4) * num_sphere_vertices, sphere_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ((num_rubiks_vertices + num_sphere_vertices) * 2 + num_plane_vertices), sizeof(vec4) * num_plane_vertices, plane_colors);

    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices) * 2,
    sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices), normals);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices)));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * (num_rubiks_vertices + num_sphere_vertices + num_plane_vertices) * 2));

    ctm_location = glGetUniformLocation(program, "ctm");
    model_view_location = glGetUniformLocation(program, "model_view_matrix");
    projection_location = glGetUniformLocation(program, "projection_matrix");
    light_position_location = glGetUniformLocation(program, "light_position");
    is_shadow_location = glGetUniformLocation(program, "is_shadow");
    plane_height_location = glGetUniformLocation(program, "plane_height");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glDepthRange(1,0);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    plane_height = plane_ctm.w.y + .01;
    light_position = (vec4){sphere_ctm.w.x, sphere_ctm.w.y, sphere_ctm.w.z, 1};

    // place various values into the pipeline
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view_matrix);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection_matrix);
    glUniform4fv(light_position_location, 1, (GLfloat *) &light_position);
    glUniform1f(plane_height_location, plane_height);

    // draw normal rubiks cube
    glUniform1i(is_shadow_location, 0);
    for (int i = 0; i < 27; i++) {
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &cubie_ctms[i]);
        glDrawArrays(GL_TRIANGLES, num_cubie_verts * i, num_cubie_verts);
    }

    // draw light sphere
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &sphere_ctm);
    glDrawArrays(GL_TRIANGLES, num_rubiks_vertices, num_sphere_vertices);

    // draw plane
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &plane_ctm);
    glDrawArrays(GL_TRIANGLES, num_rubiks_vertices + num_sphere_vertices, num_plane_vertices);

    // draw rubik's cube shadow
    glUniform1i(is_shadow_location, 1);
    for (int i = 0; i < 27; i++) {
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &cubie_ctms[i]);
        glDrawArrays(GL_TRIANGLES, num_cubie_verts * i, num_cubie_verts);
    }

    glutSwapBuffers();
}

void idle(void) {

    // if the view is being animated
    if (is_animating_view) {
        current_step_view++;

        GLfloat alpha = (float) current_step_view / max_steps_view;
        curr_eye = addVV(starting_eye, mulSV(alpha, changing_eye));

        if (current_step_view == max_steps_view) {
            is_animating_view = 0;
            current_step_view = 0;
        }

        model_view_matrix = look_at(curr_eye, curr_at, (vec4) {0, 1, 0, 0});
    }

    // if some turn is being animated on the rubiks cube
    if (is_animating_turn) {
        current_step_turn++;

        GLfloat alpha = (float) 1.0 / max_steps_turn;

        // rotate the cubies of a certain face
        for (int i = 0; i < 9; i++) {
            mat4 cubie_ctm = cubie_ctms[cubies_to_rotate[i]];
            mat4 new_cubie_ctm = cubie_ctm;

            if (current_state_turn == TURN_U || current_state_turn == TURN_D_PRIME) {
                new_cubie_ctm = mulMM(rot_y(degToRad(-90 * alpha)), cubie_ctm);
            } else if (current_state_turn == TURN_D || current_state_turn == TURN_U_PRIME) {
                new_cubie_ctm = mulMM(rot_y(degToRad(90 * alpha)), cubie_ctm);
            } else if (current_state_turn == TURN_F || current_state_turn == TURN_B_PRIME) {
                new_cubie_ctm = mulMM(rot_z(degToRad(-90 * alpha)), cubie_ctm);
            } else if (current_state_turn == TURN_B || current_state_turn == TURN_F_PRIME) {
                new_cubie_ctm = mulMM(rot_z(degToRad(90 * alpha)), cubie_ctm);
            }  else if (current_state_turn == TURN_R || current_state_turn == TURN_L_PRIME) {
                new_cubie_ctm = mulMM(rot_x(degToRad(-90 * alpha)), cubie_ctm);
            } else if (current_state_turn == TURN_L || current_state_turn == TURN_R_PRIME) {
                new_cubie_ctm = mulMM(rot_x(degToRad(90 * alpha)), cubie_ctm);
            }

            cubie_ctms[cubies_to_rotate[i]] = new_cubie_ctm;
        }

        if (current_step_turn == max_steps_turn) {
            is_animating_turn = 0;
            current_step_turn = 0;
        }
    }

    // if the scramble is being animated
    if (is_animating_scramble) {

        // don't scramble while a move is in progress
        if (is_animating_turn == 0) {

            char turn = current_scramble[current_scramble_index];
            if (turn == '\0') {
                is_animating_scramble = 0;
                current_scramble_index = 0;
            } else {
                current_scramble_index++;
                is_animating_turn = 1;
                turnRubiksCubeLayer(turn);

                if (turn == 'f') {
                    current_state_turn = TURN_F;
                } else if (turn == 'F') {
                    current_state_turn = TURN_F_PRIME;
                } else if (turn == 'r') {
                    current_state_turn = TURN_R;
                } else if (turn == 'R') {
                    current_state_turn = TURN_R_PRIME;
                } else if (turn == 'u') {
                    current_state_turn = TURN_U;
                } else if (turn == 'U') {
                    current_state_turn = TURN_U_PRIME;
                } else if (turn == 'l') {
                    current_state_turn = TURN_L;
                } else if (turn == 'L') {
                    current_state_turn = TURN_L_PRIME;
                } else if (turn == 'b') {
                    current_state_turn = TURN_B;
                } else if (turn == 'B') {
                    current_state_turn = TURN_B_PRIME;
                } else if (turn == 'd') {
                    current_state_turn = TURN_D;
                } else if (turn == 'D') {
                    current_state_turn = TURN_D_PRIME;
                }
            }
        }
    }

    // if the light sphere is being animated
    if (is_animating_sphere) {
        current_step_transl_sphere++;

        GLfloat alpha = (float) current_step_transl_sphere / max_steps_transl_sphere;
        sphere_ctm = addMM(starting_sphere_ctm, mulSM(alpha, changing_sphere_ctm));

        if (current_step_transl_sphere == max_steps_transl_sphere) {
            is_animating_sphere = 0;
            current_step_transl_sphere = 0;
        }
    }

    // if the plane "floor" is being animated
    if (is_animating_plane) {
        current_step_transl_plane++;

        GLfloat alpha = (float) current_step_transl_plane / max_steps_transl_plane;
        plane_ctm = addMM(starting_plane_ctm, mulSM(alpha, changing_plane_ctm));

        if (current_step_transl_plane == max_steps_transl_plane) {
            is_animating_plane = 0;
            current_step_transl_plane = 0;
        }
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int mousex, int mousey) {

    // key presses for quitting
    if (key == 'q' || key == 'Q') {
        glutLeaveMainLoop();
    }

    // key presses for zooming

    if (key == '=' || key == '+') { // zoom in
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ZOOMING;
            max_steps_view = 10;

            vec4 ending_eye_point = addVV(mulSV(-0.5, normV(vec4ToVector(curr_eye))), curr_eye);
            vec4 temp = subVV(vec4ToPoint(ending_eye_point), curr_eye);

            // don't let eye go inside the rubik's cube
            if (magV(vec4ToVector(ending_eye_point)) < 1) {
                is_animating_view = 0;
                current_state_view = NONE;
            }

            starting_eye = curr_eye;
            changing_eye = temp;
        }
    } else if (key == '-' || key == '_') { // zoom out
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ZOOMING;
            max_steps_view = 10;

            vec4 ending_eye_point = addVV(mulSV(0.5, normV(vec4ToVector(curr_eye))), curr_eye);
            vec4 temp = subVV(vec4ToPoint(ending_eye_point), curr_eye);

            starting_eye = curr_eye;
            changing_eye = temp;
        }
    }

    // key presses for turning the rubiks cube faces

    if (key == 'f') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_F;
            turnRubiksCubeLayer('f');
        }
    } else if (key == 'F') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_F_PRIME;
            turnRubiksCubeLayer('F');
        }
    } else if (key == 'r') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_R;
            turnRubiksCubeLayer('r');
        }
    } else if (key == 'R') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_R_PRIME;
            turnRubiksCubeLayer('R');
        }
    } else if (key == 'u') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_U;
            turnRubiksCubeLayer('u');
        }
    } else if (key == 'U') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_U_PRIME;
            turnRubiksCubeLayer('U');
        }
    } else if (key == 'l') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_L;
            turnRubiksCubeLayer('l');
        }
    } else if (key == 'L') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_L_PRIME;
            turnRubiksCubeLayer('L');
        }
    } else if (key == 'b') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_B;
            turnRubiksCubeLayer('b');
        }
    } else if (key == 'B') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_B_PRIME;
            turnRubiksCubeLayer('B');
        }
    } else if (key == 'd') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_D;
            turnRubiksCubeLayer('d');
        }
    } else if (key == 'D') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_turn = 1;
            current_state_turn = TURN_D_PRIME;
            turnRubiksCubeLayer('D');
        }
    } else if (key == 's' || key == 'S') {
        if (is_animating_turn == 0 && is_animating_scramble == 0) {
            is_animating_scramble = 1;
            current_scramble = genScrambleString();
        }
    }

    // key presses for moving the light sphere

    if (key == 'x') { // move light sphere in the -x direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            changing_sphere_ctm = subMM(mulMM(transl(-0.25, 0, 0), sphere_ctm), sphere_ctm);
        }
    } else if (key == 'X') { // move light sphere in the x direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            changing_sphere_ctm = subMM(mulMM(transl(0.25, 0, 0), sphere_ctm), sphere_ctm);
        }
    } else if (key == 'y') { // move light sphere in the -y direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            mat4 end_ctm = mulMM(transl(0, -0.25, 0), sphere_ctm);

            // prevent light from going under rubiks cube
            if (end_ctm.w.y < 1) {
                is_animating_sphere = 0;
            } else {
                changing_sphere_ctm = subMM(end_ctm, sphere_ctm);
            }
        }
    } else if (key == 'Y') { // move light sphere in the y direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            changing_sphere_ctm = subMM(mulMM(transl(0, 0.25, 0), sphere_ctm), sphere_ctm);
        }
    } else if (key == 'z') { // move light sphere in the -z direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            changing_sphere_ctm = subMM(mulMM(transl(0, 0, -0.25), sphere_ctm), sphere_ctm);
        }
    } else if (key == 'Z') { // move light sphere in the z direction
        if (is_animating_sphere == 0) {
            is_animating_sphere = 1;
            starting_sphere_ctm = sphere_ctm;
            changing_sphere_ctm = subMM(mulMM(transl(0, 0, 0.25), sphere_ctm), sphere_ctm);
        }
    }

    // key presses for moving the plane

    if (key == 'w' || key == 'W') { // move the plane in the -y direction
        if (is_animating_plane == 0) {
            is_animating_plane = 1;
            starting_plane_ctm = plane_ctm;
            changing_plane_ctm = subMM(mulMM(transl(0, -0.25, 0), plane_ctm), plane_ctm);
        }
    } else if (key == 'e' || key == 'E') { // move the plane in the y direction
        if (is_animating_plane == 0) {
            is_animating_plane = 1;
            starting_plane_ctm = plane_ctm;
            changing_plane_ctm = subMM(mulMM(transl(0, 0.25, 0), plane_ctm), plane_ctm);
        }
    }
}

void keyboard2(int key, int x, int y) {

    // key presses for rotating the view

    if (key == GLUT_KEY_LEFT) { // rotate left
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ROTATING;
            max_steps_view = 5;

            vec4 ending_eye_point = mulMV(rot_y(-pi/16), curr_eye);
            vec4 temp = subVV(ending_eye_point, curr_eye);

            starting_eye = curr_eye;
            changing_eye = temp;
        }

    } else if (key == GLUT_KEY_RIGHT) { // rotate right
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ROTATING;
            max_steps_view = 5;

            vec4 ending_eye_point = mulMV(rot_y(pi/16), curr_eye);
            vec4 temp = subVV(ending_eye_point, curr_eye);

            starting_eye = curr_eye;
            changing_eye = temp;
        }
    } else if (key == GLUT_KEY_UP) { // rotate up
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ROTATING;
            max_steps_view = 5;

            vec4 ending_eye_point = mulMV(mulMM(rot_y(atan2(-curr_eye.z, curr_eye.x)), mulMM(rot_z(pi/32), rot_y(-atan2(-curr_eye.z, curr_eye.x)))), curr_eye);
            vec4 temp = subVV(ending_eye_point, curr_eye);

            // don't let the angle between the y axis and current eye position go under 10 degrees
            if (acos(dotVV(normV((vec4){0,1,0,0}), normV(vec4ToVector(ending_eye_point)))) * 57.29578 < 10) {
                is_animating_view = 0;
                current_state_view = NONE;
            }

            starting_eye = curr_eye;
            changing_eye = temp;
        }
    } else if (key == GLUT_KEY_DOWN) { // rotate down
        if (is_animating_view == 0) {
            is_animating_view = 1;
            current_state_view = ROTATING;
            max_steps_view = 5;

            vec4 ending_eye_point = mulMV(mulMM(rot_y(atan2(-curr_eye.z, curr_eye.x)), mulMM(rot_z(-pi/32), rot_y(-atan2(-curr_eye.z, curr_eye.x)))), curr_eye);
            vec4 temp = subVV(ending_eye_point, curr_eye);

            // don't let the angle between the y axis and current eye position go under 10 degrees
            if (acos(dotVV(normV((vec4){0,-1,0,0}), normV(vec4ToVector(ending_eye_point)))) * 57.29578 < 10) {
                is_animating_view = 0;
                current_state_view = NONE;
            }

            starting_eye = curr_eye;
            changing_eye = temp;
        }
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, window_size, window_size);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(window_size, window_size);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Rubiks-GL");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard2);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
