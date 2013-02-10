//
//  main.cpp
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/15/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <vector>
#include <string>
#include "objLoader.h"
#include "ShaderLoader.h"

using namespace std;

const float WINDOW_WIDTH = 800.0f;
const float WINDOW_HEIGHT = 600.0f;

GLuint vbo;
GLuint shaderProgram;

string strVertexShader("vertex.vs");
string strFragmentShader("fragment.fs");
char *OBJ_FILE = "chev.obj";

ShaderLoader *shaderLoader = new ShaderLoader();
objLoader *objData = new objLoader();

float lightColor[] = { 1.0f, 1.0f, 1.0f };
float lightPosition[] = { 0.0f, 0.0f, 0.0f };

int numVerts = 0;

//float *vertArray;
vector<float> *vertices;

void initializeVertexBuffer() {
    vertices = new vector<float>();
    glGenBuffers(1, &vbo);
    
    for(int i = 0; i < objData->faceCount; i++)	{
		obj_face *face = objData->faceList[i];
        
		for(int j = 0; j < face->vertex_count; j++) {
            for (int k = 0; k < 3; k++) {
                double vert = objData->vertexList[face->vertex_index[j]]->e[k];
                vertices->push_back(vert);
            }
            
            vertices->push_back(1.0f);
		}
	}
    
    float vertArray[vertices->size()];
    
    for (int i = 0; i < vertices->size(); i++) {
        vertArray[i] = vertices->at(i);
    }
    
    numVerts = (int) vertices->size() / 4;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertArray), vertArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    delete vertices;
}

void initializeProgram() {
    vector<GLuint> shaderList;
    
    shaderList.push_back(shaderLoader->createShader(GL_VERTEX_SHADER, strVertexShader));
    shaderList.push_back(shaderLoader->createShader(GL_FRAGMENT_SHADER, strFragmentShader));
    
    shaderProgram = shaderLoader->createProgram(shaderList);
    
    for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void setView(int w, int h) {
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, (float) w / (float) h, 0.5f, 100);
    
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-2.5f, 2.5f, 5.0f,        // eye
              -2.5f, 2.5f, -1.0f,        // center
              0.0f, 1.0f, 0.0f);        // up
}

void reshape (int w, int h) {
    setView(w, h);
}

void display() {
    GLint attrib;
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setView(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    
    glUseProgram(shaderProgram);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    attrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_QUADS, 0, numVerts);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glDisableVertexAttribArray(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUseProgram(0);
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST | GL_LIGHTING | GL_TEXTURE_2D);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
    
    objData->load(OBJ_FILE);
    
    initializeVertexBuffer();
    initializeProgram();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL");
	glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
	init();
    
	glutMainLoop();
    
    delete shaderLoader;
    delete objData;
    
    return 0;
}