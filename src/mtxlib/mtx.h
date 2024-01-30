
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

// vector representation
typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat w;
} vec4;

// vector representation
typedef struct {
	GLfloat x;
	GLfloat y;
} vec2;

// matrix representation
typedef struct {
	vec4 x;
	vec4 y;
	vec4 z;
	vec4 w;
} mat4;

// returns 1 if all the values of each vec4 are the same, 0 otherwise
int vec4sAreEqual(vec4 v1, vec4 v2);

// converts a vec4 to a vector vec4
vec4 vec4ToVector(vec4 v);

//converts a vec4 for a point vec4
vec4 vec4ToPoint(vec4 v);

// prints a vec4
void printV(vec4 v);

// scalar-vector multiplication
vec4 mulSV(GLfloat s, vec4 v);

// vector-vector addition
vec4 addVV(vec4 v1, vec4 v2);

// vector-vector subtraction
vec4 subVV(vec4 v1, vec4 v2);

// magnitude of a vector
GLfloat magV(vec4 v);

// normalize vector
vec4 normV(vec4 v);

// dot product of two vectors
GLfloat dotVV(vec4 v1, vec4 v2);

// cross product of two vectors
vec4 crossVV(vec4 v1, vec4 v2);

// prints a mat4
void printM(mat4 m);

// scalar-matrix multiplication
mat4 mulSM(GLfloat s, mat4 m);

// matrix-matrix multiplication
mat4 addMM(mat4 m1, mat4 m2);

// matrix-matrix subtraction
mat4 subMM(mat4 m1, mat4 m2);

// matrix-matrix multiplication
mat4 mulMM(mat4 m1, mat4 m2);

// inverse of a matrix
mat4 invM(mat4 m);

// transpose of a matrix
mat4 transpM(mat4 m);

// matrix-vector multiplication
vec4 mulMV(mat4 m, vec4 v);

// minor of a matrix
mat4 minorM(mat4 m);

// cofactor of a matrix
mat4 cofactM(mat4 m);

// determinant of a matrix
GLfloat deterM(mat4 m);

// create the translation matrix
mat4 transl(GLfloat a1, GLfloat a2, GLfloat a3);

// create the inverse of the translation matrix
mat4 transl_inv(GLfloat a1, GLfloat a2, GLfloat a3);

// create the scale matrix
mat4 scale(GLfloat b1, GLfloat b2, GLfloat b3);

// create the inverse of the scale matrix
mat4 scale_inv(GLfloat b1, GLfloat b2, GLfloat b3);

// create the matrix to rotate about x
mat4 rot_x(GLfloat th);

// create the matrix to rotate about y
mat4 rot_y(GLfloat th);

// create the matrix to rotate about z
mat4 rot_z(GLfloat th);

// create the matrix to rotate about an arbitrary axis and point
mat4 rot_a(vec4 v, vec4 p, GLfloat th);

// create the matrix to rotate about an arbitrary axis and the origin
mat4 rot_ao(vec4 v, GLfloat th);

// returns the identity matrix
mat4 identM();

// returns an orthoagonal projection matrix
mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

// returns a perspective projection matrix in the form of a frustum
mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

// returns a perspective projection matrix
mat4 perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);

// returns a model view matrix
mat4 look_at(vec4 eye, vec4 at, vec4 up);
