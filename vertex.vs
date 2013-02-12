#version 120

attribute vec4 position;
attribute vec3 color;

void main() {
    gl_FrontColor = vec4(color, 1.0);
    
	gl_Position = gl_ModelViewProjectionMatrix * position;
}