#version 120

attribute vec4 position;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * position;
}