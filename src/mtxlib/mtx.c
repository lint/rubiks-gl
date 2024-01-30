
#include <stdio.h>
#include <math.h>
#include "mtx.h"

// returns 1 if all the values of each vec4 are the same, 0 otherwise
int vec4sAreEqual(vec4 v1, vec4 v2) {
    return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
}

// converts a vec4 to a vector vec4
vec4 vec4ToVector(vec4 v) {
	return (vec4){v.x, v.y, v.z, 0};
}

//converts a vec4 for a point vec4
vec4 vec4ToPoint(vec4 v) {
	return (vec4){v.x, v.y, v.z, 1};
}

// prints a vec4
void printV(vec4 v) {
	printf("[%10.4f %10.4f %10.4f %10.4f ]\n", v.x, v.y, v.z, v.w);
}

// scalar-vector multiplication
vec4 mulSV(GLfloat s, vec4 v) {
	return (vec4){s * v.x, s * v.y, s * v.z, s * v.w};
}

// vector-vector addition
vec4 addVV(vec4 v1, vec4 v2) {
	return (vec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

// vector-vector subtraction
vec4 subVV(vec4 v1, vec4 v2) {
	return (vec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

// magnitude of a vector
GLfloat magV(vec4 v) {
	return (GLfloat)sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
}

// normalize vector
vec4 normV(vec4 v) {
	if (vec4sAreEqual(v, (vec4) {0, 0, 0, 0})) {
		return v;
	} else {
		return mulSV(1.0 / magV(v), v);
	}
}

// dot product of two vectors
GLfloat dotVV(vec4 v1, vec4 v2) {
	return (GLfloat)((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w));
}

// cross product of two vectors
vec4 crossVV(vec4 v1, vec4 v2) {
	return (vec4){(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x), 0.0};
}

// prints a mat4
void printM(mat4 m) {
	printf("[[%10.4f %10.4f %10.4f %10.4f ]\n", m.x.x, m.y.x, m.z.x, m.w.x);
	printf(" [%10.4f %10.4f %10.4f %10.4f ]\n", m.x.y, m.y.y, m.z.y, m.w.y);
	printf(" [%10.4f %10.4f %10.4f %10.4f ]\n", m.x.z, m.y.z, m.z.z, m.w.z);
	printf(" [%10.4f %10.4f %10.4f %10.4f ]]\n", m.x.w, m.y.w, m.z.w, m.w.w);
}

// scalar-matrix multiplication
mat4 mulSM(GLfloat s, mat4 m) {
	return (mat4){
		{s * m.x.x, s * m.x.y, s * m.x.z, s * m.x.w},
	 	{s * m.y.x, s * m.y.y, s * m.y.z, s * m.y.w},
	 	{s * m.z.x, s * m.z.y, s * m.z.z, s * m.z.w},
	 	{s * m.w.x, s * m.w.y, s * m.w.z, s * m.w.w}
	};
}

// matrix-matrix addition
mat4 addMM(mat4 m1, mat4 m2) {
	return (mat4){
		{m1.x.x + m2.x.x, m1.x.y + m2.x.y, m1.x.z + m2.x.z, m1.x.w + m2.x.w},
	 	{m1.y.x + m2.y.x, m1.y.y + m2.y.y, m1.y.z + m2.y.z, m1.y.w + m2.y.w},
	 	{m1.z.x + m2.z.x, m1.z.y + m2.z.y, m1.z.z + m2.z.z, m1.z.w + m2.z.w},
	 	{m1.w.x + m2.w.x, m1.w.y + m2.w.y, m1.w.z + m2.w.z, m1.w.w + m2.w.w}
	};
}

// matrix-matrix subtraction
mat4 subMM(mat4 m1, mat4 m2) {
	return (mat4){
		{m1.x.x - m2.x.x, m1.x.y - m2.x.y, m1.x.z - m2.x.z, m1.x.w - m2.x.w},
	 	{m1.y.x - m2.y.x, m1.y.y - m2.y.y, m1.y.z - m2.y.z, m1.y.w - m2.y.w},
	 	{m1.z.x - m2.z.x, m1.z.y - m2.z.y, m1.z.z - m2.z.z, m1.z.w - m2.z.w},
	 	{m1.w.x - m2.w.x, m1.w.y - m2.w.y, m1.w.z - m2.w.z, m1.w.w - m2.w.w}
	};
}

// matrix-matrix multiplication
mat4 mulMM(mat4 m1, mat4 m2) {
	return (mat4){
		{(m1.x.x * m2.x.x) + (m1.y.x * m2.x.y) + (m1.z.x * m2.x.z) + (m1.w.x * m2.x.w),
		 (m1.x.y * m2.x.x) + (m1.y.y * m2.x.y) + (m1.z.y * m2.x.z) + (m1.w.y * m2.x.w),
		 (m1.x.z * m2.x.x) + (m1.y.z * m2.x.y) + (m1.z.z * m2.x.z) + (m1.w.z * m2.x.w),
		 (m1.x.w * m2.x.x) + (m1.y.w * m2.x.y) + (m1.z.w * m2.x.z) + (m1.w.w * m2.x.w)},

		{(m1.x.x * m2.y.x) + (m1.y.x * m2.y.y) + (m1.z.x * m2.y.z) + (m1.w.x * m2.y.w),
		 (m1.x.y * m2.y.x) + (m1.y.y * m2.y.y) + (m1.z.y * m2.y.z) + (m1.w.y * m2.y.w),
		 (m1.x.z * m2.y.x) + (m1.y.z * m2.y.y) + (m1.z.z * m2.y.z) + (m1.w.z * m2.y.w),
		 (m1.x.w * m2.y.x) + (m1.y.w * m2.y.y) + (m1.z.w * m2.y.z) + (m1.w.w * m2.y.w)},

		{(m1.x.x * m2.z.x) + (m1.y.x * m2.z.y) + (m1.z.x * m2.z.z) + (m1.w.x * m2.z.w),
		 (m1.x.y * m2.z.x) + (m1.y.y * m2.z.y) + (m1.z.y * m2.z.z) + (m1.w.y * m2.z.w),
		 (m1.x.z * m2.z.x) + (m1.y.z * m2.z.y) + (m1.z.z * m2.z.z) + (m1.w.z * m2.z.w),
		 (m1.x.w * m2.z.x) + (m1.y.w * m2.z.y) + (m1.z.w * m2.z.z) + (m1.w.w * m2.z.w)},

		{(m1.x.x * m2.w.x) + (m1.y.x * m2.w.y) + (m1.z.x * m2.w.z) + (m1.w.x * m2.w.w),
		 (m1.x.y * m2.w.x) + (m1.y.y * m2.w.y) + (m1.z.y * m2.w.z) + (m1.w.y * m2.w.w),
		 (m1.x.z * m2.w.x) + (m1.y.z * m2.w.y) + (m1.z.z * m2.w.z) + (m1.w.z * m2.w.w),
		 (m1.x.w * m2.w.x) + (m1.y.w * m2.w.y) + (m1.z.w * m2.w.z) + (m1.w.w * m2.w.w)}
	};
}

// inverse of a matrix
mat4 invM(mat4 m) {
	return mulSM(1.0 / deterM(m), transpM(cofactM(minorM(m))));
}

// transpose of a matrix
mat4 transpM(mat4 m) {
	return (mat4){
		{m.x.x, m.y.x, m.z.x, m.w.x},
	 	{m.x.y, m.y.y, m.z.y, m.w.y},
	 	{m.x.z, m.y.z, m.z.z, m.w.z},
	 	{m.x.w, m.y.w, m.z.w, m.w.w}
	};
}

// matrix-vector multiplication
vec4 mulMV(mat4 m, vec4 v) {
	return (vec4){
		(m.x.x * v.x) + (m.y.x * v.y) + (m.z.x * v.z) + (m.w.x * v.w),
		(m.x.y * v.x) + (m.y.y * v.y) + (m.z.y * v.z) + (m.w.y * v.w),
		(m.x.z * v.x) + (m.y.z * v.y) + (m.z.z * v.z) + (m.w.z * v.w),
		(m.x.w * v.x) + (m.y.w * v.y) + (m.z.w * v.z) + (m.w.w * v.w)
	};
}

// minor of a matrix
mat4 minorM(mat4 m) {
	return (mat4){
		{(m.y.y * m.z.z * m.w.w) + (m.z.y * m.w.z * m.y.w) + (m.w.y * m.y.z * m.z.w) - (m.y.w * m.z.z * m.w.y) - (m.z.w * m.w.z * m.y.y) - (m.w.w * m.y.z * m.z.y),
		 (m.y.x * m.z.z * m.w.w) + (m.z.x * m.w.z * m.y.w) + (m.w.x * m.y.z * m.z.w) - (m.y.w * m.z.z * m.w.x) - (m.z.w * m.w.z * m.y.x) - (m.w.w * m.y.z * m.z.x),
		 (m.y.x * m.z.y * m.w.w) + (m.z.x * m.w.y * m.y.w) + (m.w.x * m.y.y * m.z.w) - (m.y.w * m.z.y * m.w.x) - (m.z.w * m.w.y * m.y.x) - (m.w.w * m.y.y * m.z.x),
		 (m.y.x * m.z.y * m.w.z) + (m.z.x * m.w.y * m.y.z) + (m.w.x * m.y.y * m.z.z) - (m.y.z * m.z.y * m.w.x) - (m.z.z * m.w.y * m.y.x) - (m.w.z * m.y.y * m.z.x)},

		{(m.x.y * m.z.z * m.w.w) + (m.z.y * m.w.z * m.x.w) + (m.w.y * m.x.z * m.z.w) - (m.x.w * m.z.z * m.w.y) - (m.z.w * m.w.z * m.x.y) - (m.w.w * m.x.z * m.z.y),
		 (m.x.x * m.z.z * m.w.w) + (m.z.x * m.w.z * m.x.w) + (m.w.x * m.x.z * m.z.w) - (m.x.w * m.z.z * m.w.x) - (m.z.w * m.w.z * m.x.x) - (m.w.w * m.x.z * m.z.x),
		 (m.x.x * m.z.y * m.w.w) + (m.z.x * m.w.y * m.x.w) + (m.w.x * m.x.y * m.z.w) - (m.x.w * m.z.y * m.w.x) - (m.z.w * m.w.y * m.x.x) - (m.w.w * m.x.y * m.z.x),
		 (m.x.x * m.z.y * m.w.z) + (m.z.x * m.w.y * m.x.z) + (m.w.x * m.x.y * m.z.z) - (m.x.z * m.z.y * m.w.x) - (m.z.z * m.w.y * m.x.x) - (m.w.z * m.x.y * m.z.x)},

		{(m.x.y * m.y.z * m.w.w) + (m.y.y * m.w.z * m.x.w) + (m.w.y * m.x.z * m.y.w) - (m.x.w * m.y.z * m.w.y) - (m.y.w * m.w.z * m.x.y) - (m.w.w * m.x.z * m.y.y),
 		 (m.x.x * m.y.z * m.w.w) + (m.y.x * m.w.z * m.x.w) + (m.w.x * m.x.z * m.y.w) - (m.x.w * m.y.z * m.w.x) - (m.y.w * m.w.z * m.x.x) - (m.w.w * m.x.z * m.y.x),
 		 (m.x.x * m.y.y * m.w.w) + (m.y.x * m.w.y * m.x.w) + (m.w.x * m.x.y * m.y.w) - (m.x.w * m.y.y * m.w.x) - (m.y.w * m.w.y * m.x.x) - (m.w.w * m.x.y * m.y.x),
 		 (m.x.x * m.y.y * m.w.z) + (m.y.x * m.w.y * m.x.z) + (m.w.x * m.x.y * m.y.z) - (m.x.z * m.y.y * m.w.x) - (m.y.z * m.w.y * m.x.x) - (m.w.z * m.x.y * m.y.x)},

		{(m.x.y * m.y.z * m.z.w) + (m.y.y * m.z.z * m.x.w) + (m.z.y * m.x.z * m.y.w) - (m.x.w * m.y.z * m.z.y) - (m.y.w * m.z.z * m.x.y) - (m.z.w * m.x.z * m.y.y),
 		 (m.x.x * m.y.z * m.z.w) + (m.y.x * m.z.z * m.x.w) + (m.z.x * m.x.z * m.y.w) - (m.x.w * m.y.z * m.z.x) - (m.y.w * m.z.z * m.x.x) - (m.z.w * m.x.z * m.y.x),
 		 (m.x.x * m.y.y * m.z.w) + (m.y.x * m.z.y * m.x.w) + (m.z.x * m.x.y * m.y.w) - (m.x.w * m.y.y * m.z.x) - (m.y.w * m.z.y * m.x.x) - (m.z.w * m.x.y * m.y.x),
 		 (m.x.x * m.y.y * m.z.z) + (m.y.x * m.z.y * m.x.z) + (m.z.x * m.x.y * m.y.z) - (m.x.z * m.y.y * m.z.x) - (m.y.z * m.z.y * m.x.x) - (m.z.z * m.x.y * m.y.x)}
	};
}

// cofactor of a matrix
mat4 cofactM(mat4 m) {
	return (mat4){
		{m.x.x, -1 * m.x.y, m.x.z, -1 * m.x.w},
		{-1 * m.y.x, m.y.y, -1 * m.y.z, m.y.w},
		{m.z.x, -1 * m.z.y, m.z.z, -1 * m.z.w},
		{-1 * m.w.x, m.w.y, -1 * m.w.z, m.w.w}
	};
}

// determinant of a matrix
GLfloat deterM(mat4 m) {
	mat4 minor = minorM(m);
	return (m.x.x * minor.x.x) - (m.x.y * minor.x.y) + (m.x.z * minor.x.z) - (m.x.w * minor.x.w);
}

// create the translation matrix
mat4 transl(GLfloat a1, GLfloat a2, GLfloat a3) {

	return (mat4){
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{a1, a2, a3, 1}
	};
}

// create the opposite translation matrix
mat4 transl_inv(GLfloat a1, GLfloat a2, GLfloat a3) {
	// same thing as invM(transl())

	return (mat4){
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{-a1, -a2, -a3, 1}
	};
}

// create the scale matrix
mat4 scale(GLfloat b1, GLfloat b2, GLfloat b3) {

	return (mat4){
		{b1, 0, 0, 0},
		{0, b2, 0, 0},
		{0, 0, b3, 0},
		{0, 0, 0, 1}
	};
}

// create the inverse of a scale matrix
mat4 scale_inv(GLfloat b1, GLfloat b2, GLfloat b3) {
	//  same as invM(scale())

	return (mat4){
		{1 / b1, 0, 0, 0},
		{0, 1 / b2, 0, 0},
		{0, 0, 1 / b3, 0},
		{0, 0, 0, 1}
	};
}

// create the matrix to rotate about x
mat4 rot_x(GLfloat th) {
	// th (theta) is in radians

	return (mat4){
		{1, 0, 0, 0},
		{0, cos(th), sin(th), 0},
		{0, -sin(th), cos(th), 0},
		{0, 0, 0, 1}
	};
}

// create the matrix to rotate about y
mat4 rot_y(GLfloat th) {
	// th (theta) is in radians

	return (mat4){
		{cos(th), 0, -sin(th), 0},
		{0, 1, 0, 0},
		{sin(th), 0, cos(th), 0},
		{0, 0, 0, 1}
	};
}

// create the matrix to rotate about z
mat4 rot_z(GLfloat th) {
	// th (theta) is in radians

	return (mat4){
		{cos(th), sin(th), 0, 0},
		{-sin(th), cos(th), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
}

// create the matrix to rotate about an arbitrary axis and point
mat4 rot_a(vec4 v, vec4 p, GLfloat th) {
	// th (theta) values are in radians

	v = normV(v);

	float d = sqrt(pow(v.y, 2) + pow(v.z, 2));
	float sinThX = v.y / d;
	float cosThX = v.z / d;
	float sinThY = v.x;
	float cosThY = d;

	mat4 rotXPos = (mat4){
		{1, 0, 0, 0},
		{0, cosThX, sinThX, 0},
		{0, -sinThX, cosThX, 0},
		{0, 0, 0, 1}
	};

	mat4 rotXNeg = (mat4){
		{1, 0, 0, 0},
		{0, cosThX, -sinThX, 0},
		{0, sinThX, cosThX, 0},
		{0, 0, 0, 1}
	};

	mat4 rotYPos = (mat4) {
		{cosThY, 0, -sinThY, 0},
		{0, 1, 0, 0},
		{sinThY, 0, cosThY, 0},
		{0, 0, 0, 1}
	};

	mat4 rotYNeg = (mat4) {
		{cosThY, 0, sinThY, 0},
		{0, 1, 0, 0},
		{-sinThY, 0, cosThY, 0},
		{0, 0, 0, 1}
	};

	return mulMM(transl(p.x, p.y, p.z), mulMM(rotXNeg, mulMM(rotYPos, mulMM(rot_z(th), mulMM(rotYNeg, mulMM(rotXPos, transl_inv(p.x, p.y, p.z)))))));

	/*
	// I feel like this should have worked, it almost did but any rotation about -z was about z instead..
	// Maybe the standard library functions just aren't perfect? Who knows, but hardcoding the rotation matrices rather than calculating them works out.
	v = normV(v);
	float thX = asin(v.y / sqrt(pow(v.y, 2) + pow(v.z, 2)));
	float thY = asin(v.x);

	return mulMM(transl(p.x, p.y, p.z), mulMM(rot_x(-thX), mulMM(rot_y(thY), mulMM(rot_z(th), mulMM(rot_y(-thY), mulMM(rot_x(thX), transl_inv(p.x, p.y, p.z)))))));
	*/
}

// create the matrix to rotate about an arbitrary axis and the origin
mat4 rot_ao(vec4 v, GLfloat th) {
	// th (theta) is in radians
	return rot_a(v, (vec4){0, 0, 0, 1}, th);
}

// returns the identity matrix
mat4 identM() {
	return (mat4){
	    {1, 0, 0, 0},
	    {0, 1, 0, 0},
	    {0, 0, 1, 0},
	    {0, 0, 0, 1}
	};
}

// returns an orthoagonal projection matrix
mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {
	mat4 translation_matrix = transl((right + left) / -2.0, (top + bottom) / -2.0, (near + far) / -2.0);
	mat4 scale_matrix = scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (near - far));
	return mulMM(scale_matrix, translation_matrix);
}

// returns an perspective projection matrix in the form of a frustum
mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {

	mat4 shear_matrix = (mat4) {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{(left + right) / (-2 * near), (bottom + top) / (-2 * near), 1, 0},
		{0, 0, 0, 1}
	};
	mat4 scale_matrix = (mat4) {
		{(-2 * near) / (right - left), 0, 0, 0},
		{0, (-2 * near) / (top - bottom), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	mat4 fit_ccv_matrix = (mat4) {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, (near + far) / (far - near), -1},
		{0, 0, (-2 * near * far) / (far - near), 0}
	};

	//return fit_ccv_matrix;
	return mulMM(fit_ccv_matrix, mulMM(scale_matrix, shear_matrix));
}

// returns a perspective projection matrix
mat4 perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
	// fovy is given in degrees, aspect is the ratio of width:height

	GLfloat height = tan(fovy * 3.14159265 / 180.0) * near;
	GLfloat width = aspect * height;

	printf("height: %f, width: %f\n", height, width);
	printf("left: %f right: %f bottom: %f top: %f near: %f far: %f\n", -width/2, width/2, 0.0, height, near, far);

	return frustum(-width/2, width/2, 0, height, near, far);
}

// returns a model view matrix
mat4 look_at(vec4 eye, vec4 at, vec4 up) {

	mat4 translation_matrix = transl(-eye.x, -eye.y, -eye.z);

	vec4 z = normV(subVV(eye, at));
	if (vec4sAreEqual(crossVV(up, z), (vec4){0, 0, 0, 0})) { // if look direction and up direction are parallel, offset look direction very slightly
		//z = normV((vec4) {z.x + .001, z.y + .001, z.z + .001, 0});
		up = normV((vec4) {up.x + .001, up.y + .001, up.z + .001, 0});
	}
	vec4 x = normV(crossVV(up, z));
	vec4 y = normV(crossVV(z, x));

	/*
	printf("x:\n");
	printV(x);
	printf("y:\n");
	printV(y);
	printf("z:\n");
	printV(z);
	*/

	mat4 rotation_matrix = (mat4) {
		{x.x, y.x, z.x, 0},
		{x.y, y.y, z.y, 0},
		{x.z, y.z, z.z, 0},
		{0, 0, 0, 1}
	};

	return mulMM(rotation_matrix, translation_matrix);
}
