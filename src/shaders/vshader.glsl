#version 120

attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec4 vNormal;
varying vec4 color;

uniform mat4 ctm; // Object Current Transformation Matrix
uniform mat4 model_view_matrix; // Model view matrix (look_at())
uniform mat4 projection_matrix; // Projection matrix (ortho() for now)
uniform vec4 light_position; // position of the light sphere
uniform int is_shadow; // determines if should draw shadow or not
uniform float plane_height; // current height of the plane to project onto

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

void main() {
	
	if(is_shadow == 0) {
		
		gl_Position = projection_matrix * model_view_matrix * ctm * vPosition;
		color = vColor;

		N = normalize(model_view_matrix * ctm * vNormal);
		vec4 L_temp = model_view_matrix * (light_position - ctm * vPosition);
		L = normalize(L_temp);
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = normalize(eye_point - (model_view_matrix * ctm * vPosition));
		distance = length(L_temp);

	} else {

		vec4 temp = ctm * vPosition;
		float x = light_position.x - ((light_position.y - plane_height) * ((light_position.x - temp.x) / (light_position.y - temp.y)));
		float z = light_position.z - ((light_position.y - plane_height) * ((light_position.z - temp.z) / (light_position.y - temp.y)));

		gl_Position = projection_matrix * model_view_matrix * vec4(x, plane_height, z, 1);
		color = vec4(0, 0, 0, 1);
	}
}
