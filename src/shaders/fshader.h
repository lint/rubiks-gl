
#ifndef _F_SHADER_H_
#define _F_SHADER_H_

// fshader.glsl as a string so that it can be included in the binary
static char * F_SHADER_STR = "#version 120\n"
"varying vec4 color;\n"
"\n"
"varying vec4 N;\n"
"varying vec4 L;\n"
"varying vec4 V;\n"
"varying float distance;\n"
"\n"
"vec4 ambient, diffuse, specular;\n"
"\n"
"void main()\n"
" {\n"
"	float shininess = 1000;\n"
"\n" 
"	vec4 NN = normalize(N);\n"
"	vec4 VV = normalize(V);\n"
"	vec4 LL = normalize(L);\n"
"\n" 
"	ambient = 0.2 * color;\n"
"	vec4 H = normalize(LL + VV);\n"
"	diffuse = max(dot(LL, NN), 0.0) * 0.8 * color;\n"
"	specular = pow(max(dot(NN, H), 0.0), shininess) * vec4(1.0, 1.0, 1.0, 1.0);\n"
"	//float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) + (attenuation_quadratic * distance * distance));\n"
"	float attenuation = 1;\n"
"\n"
"	gl_FragColor = ambient + attenuation * (diffuse + specular);\n"
"}\n";

#endif
