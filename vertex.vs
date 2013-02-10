#version 120

attribute vec4 position;
attribute vec3 normal;

void main() {
    vec3 lightDir, newNormal;
	vec4 diffuse, ambient, globalAmbient;
	float NdotL;
    
	newNormal = normalize(gl_NormalMatrix * normal);
	lightDir = normalize(vec3(gl_LightSource[0].position - position));
	NdotL = max(dot(newNormal, lightDir), 0.0);
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	gl_FrontColor =  NdotL * diffuse + globalAmbient + ambient;
    gl_BackColor = gl_FrontColor;
    
	gl_Position = gl_ModelViewProjectionMatrix * position;
}