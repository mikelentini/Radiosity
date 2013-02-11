#version 120

attribute vec4 position;
attribute vec3 normal;
attribute vec3 amb;
attribute vec3 diff;
attribute vec3 spec;
attribute float Ni;
attribute float trans;
attribute float Ns;
varying float intensity;

void main() {
    vec3 lightDir, newNormal;
	vec4 diffuse, ambient, globalAmbient;
	float NdotL;
    
	newNormal = normalize(gl_NormalMatrix * normal);
	lightDir = normalize(vec3(gl_LightSource[0].position - position));
	NdotL = max(dot(newNormal, lightDir), 0.0);
	diffuse = vec4(diff, 1) * gl_LightSource[0].diffuse;
    
    ambient = vec4(amb, 1) * gl_LightSource[0].ambient;
    globalAmbient = gl_LightModel.ambient * vec4(amb, 1);
	gl_FrontColor =  NdotL * diffuse + globalAmbient + ambient;
    
	gl_Position = gl_ModelViewProjectionMatrix * position;
}