#version 120

varying vec4 color;

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

vec4 ambient, diffuse, specular;

void main()
{
	float shininess = 1000;

	vec4 NN = normalize(N);
	vec4 VV = normalize(V);
	vec4 LL = normalize(L);

	ambient = 0.2 * color;
	vec4 H = normalize(LL + VV);
	diffuse = max(dot(LL, NN), 0.0) * 0.8 * color;
	specular = pow(max(dot(NN, H), 0.0), shininess) * vec4(1.0, 1.0, 1.0, 1.0);
	//float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) + (attenuation_quadratic * distance * distance));
	float attenuation = 1;

	gl_FragColor = ambient + attenuation * (diffuse + specular);
}
