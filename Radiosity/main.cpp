//
//  main.cpp
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/15/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <vector>
#include <string>
#include "vecmath.h"
#include "objLoader.h"
#include "ShaderLoader.h"

using namespace std;

const float WINDOW_WIDTH = 600.0f;
const float WINDOW_HEIGHT = 600.0f;

GLuint vbo, normalVbo;
GLuint shaderProgram;

string strVertexShader("vertex.vs");
string strFragmentShader("fragment.fs");
char *OBJ_FILE = "cornell-and-friends2.obj";

ShaderLoader *shaderLoader = new ShaderLoader();
objLoader *objData = new objLoader();

float ambColor[] = { 0.5f, 0.5f, 0.5f };
float diffColor[] = { 0.75f, 0.75f, 0.75f };
float specColor[] = { 1.0f, 1.0f, 1.0f };
float lightPosition[] = { -2.5f, 2.5f, 5.0f };

int numVerts = 0;

//float *vertArray;
vector<float> *vertices;
vector<float> *normals;
vector<float*> *colors;

Vector3 black(0.0f, 0.0f, 0.0f);
Vector3 white(1.0f, 1.0f, 1.0f);

struct Patch {
    Vector3 excident;
    Vector3 incident;
    float reflectance;
    float emission;
    
    Vector3 color;
    Vector3 currentColor;
    Vector3 verts[4];
    Vector3 normals[4];
};

vector<Patch> *patches;
vector<Patch> *prevPatches;
int MAX_PASSES = 10;

char *trimWhitespace(char *str) {
    char *end;
    
    // Trim leading space
    while(isspace(*str)) str++;
    
    if(*str == 0)  // All spaces?
        return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    
    // Write new null terminator
    *(end+1) = 0;
    
    return str;
}

void initializeVertexBuffer() {
    vertices = new vector<float>();
    colors = new vector<float*>();
    
    prevPatches = new vector<Patch>();
    patches = new vector<Patch>();
    
    glGenBuffers(1, &vbo);
    
    for(int i = 0; i < objData->faceCount; i++)	{
		obj_face *face = objData->faceList[i];
        int materialIndex = face->material_index;
        double *diff = objData->materialList[materialIndex]->diff;
        double Ns = objData->materialList[materialIndex]->shiny / 200.0;
        
        obj_vector *vert1 = objData->vertexList[face->vertex_index[0]];
        obj_vector *vert2 = objData->vertexList[face->vertex_index[1]];
        obj_vector *vert3 = objData->vertexList[face->vertex_index[2]];
        obj_vector *vert4 = objData->vertexList[face->vertex_index[3]];
        
        obj_vector *norm1 = objData->normalList[face->normal_index[0]];
        obj_vector *norm2 = objData->normalList[face->normal_index[1]];
        obj_vector *norm3 = objData->normalList[face->normal_index[2]];
        obj_vector *norm4 = objData->normalList[face->normal_index[3]];
        
        // add face as a patch to the patches and prevPatches vectors
        Patch patch;
        trimWhitespace(objData->materialList[face->material_index]->name);
        if (strcmp(objData->materialList[materialIndex]->name, "light") == 0) {
            patch = {
                Vector3(white),
                Vector3(white),
                (float) Ns,
                1.0f,
                Vector3(diff[0], diff[1], diff[2]),
                Vector3(1.0f, 1.0f, 1.0f), 
                {
                    Vector3(vert1->e[0], vert1->e[1], vert1->e[2]),
                    Vector3(vert2->e[0], vert2->e[1], vert2->e[2]),
                    Vector3(vert3->e[0], vert3->e[1], vert3->e[2]),
                    Vector3(vert4->e[0], vert4->e[1], vert4->e[2])
                },
                {
                    Vector3(norm1->e[0], norm1->e[1], norm1->e[2]),
                    Vector3(norm2->e[0], norm2->e[1], norm2->e[2]),
                    Vector3(norm3->e[0], norm3->e[1], norm3->e[2]),
                    Vector3(norm4->e[0], norm4->e[1], norm4->e[2])
                }
            };
        } else {
            patch = {
                Vector3(black),
                Vector3(black),
                (float) Ns,
                0.2f,
                Vector3(diff[0], diff[1], diff[2]),
                Vector3(0.0f, 0.0f, 0.0f),
                {
                    Vector3(vert1->e[0], vert1->e[1], vert1->e[2]),
                    Vector3(vert2->e[0], vert2->e[1], vert2->e[2]),
                    Vector3(vert3->e[0], vert3->e[1], vert3->e[2]),
                    Vector3(vert4->e[0], vert4->e[1], vert4->e[2])
                },
                {
                    Vector3(norm1->e[0], norm1->e[1], norm1->e[2]),
                    Vector3(norm2->e[0], norm2->e[1], norm2->e[2]),
                    Vector3(norm3->e[0], norm3->e[1], norm3->e[2]),
                    Vector3(norm4->e[0], norm4->e[1], norm4->e[2])
                }
            };
        }
        
        patches->push_back(patch);
        
		for(int j = 0; j < face->vertex_count; j++) {
            obj_vector *vert = objData->vertexList[face->vertex_index[j]];
            
            vertices->push_back(vert->e[0]);
            vertices->push_back(vert->e[1]);
            vertices->push_back(vert->e[2]);
            vertices->push_back(1.0f);
            
            float *color = new float[3];
            if (strcmp(objData->materialList[face->material_index]->name, "light") == 0) {
                color[0] = 1.0f;
                color[1] = 1.0f;
                color[2] = 1.0f;
                colors->push_back(color);
            } else {
                color[0] = 0.0f;
                color[1] = 0.0f;
                color[2] = 0.0f;
                colors->push_back(color);
            }
		}
	}
    
    long arraySize = vertices->size() + (colors->size() * 3);
    float vertArray[arraySize];
    
    /* { 1.0, 1.0, 1.0, 1.0     <- vert position
     *   0.0, 0.0, 0.0 }        <- color
     */
    for (int i = 0, j = 0, l = 0; i < arraySize; i += 7, j += 4, l++) {
        vertArray[i] = vertices->at(j);
        vertArray[i + 1] = vertices->at(j + 1);
        vertArray[i + 2] = vertices->at(j + 2);
        vertArray[i + 3] = vertices->at(j + 3);
        
        vertArray[i + 4] = colors->at(l)[0];
        vertArray[i + 5] = colors->at(l)[1];
        vertArray[i + 6] = colors->at(l)[2];
    }
    
    numVerts = (int) vertices->size() / 4;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertArray), vertArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    delete vertices;
    delete normals;
    delete colors;
}

void initializeVertexBuffer(bool patchesSet) {
    if (patchesSet) {
        glDeleteBuffers(1, &vbo);
        glGenBuffers(1, &vbo);
        
        numVerts = (int) patches->size() * 4;
        long arraySize = numVerts * 4 + (numVerts * 3);
        float vertArray[arraySize];
        
        for (int i = 0, j = 0; i < arraySize; i += 28, j++) {
            Patch patch = patches->at(j);
            
            vertArray[i] = patch.verts[0].x;
            vertArray[i + 1] = patch.verts[0].y;
            vertArray[i + 2] = patch.verts[0].z;
            vertArray[i + 3] = 1.0f;
            vertArray[i + 4] = patch.currentColor.x;
            vertArray[i + 5] = patch.currentColor.y;
            vertArray[i + 6] = patch.currentColor.z;
            
            vertArray[i + 7] = patch.verts[1].x;
            vertArray[i + 8] = patch.verts[1].y;
            vertArray[i + 9] = patch.verts[1].z;
            vertArray[i + 10] = 1.0f;
            vertArray[i + 11] = patch.currentColor.x;
            vertArray[i + 12] = patch.currentColor.y;
            vertArray[i + 13] = patch.currentColor.z;
            
            vertArray[i + 14] = patch.verts[2].x;
            vertArray[i + 15] = patch.verts[2].y;
            vertArray[i + 16] = patch.verts[2].z;
            vertArray[i + 17] = 1.0f;
            vertArray[i + 18] = patch.currentColor.x;
            vertArray[i + 19] = patch.currentColor.y;
            vertArray[i + 20] = patch.currentColor.z;
            
            vertArray[i + 21] = patch.verts[3].x;
            vertArray[i + 22] = patch.verts[3].y;
            vertArray[i + 23] = patch.verts[3].z;
            vertArray[i + 24] = 1.0f;
            vertArray[i + 25] = patch.currentColor.x;
            vertArray[i + 26] = patch.currentColor.y;
            vertArray[i + 27] = patch.currentColor.z;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertArray), vertArray, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        initializeVertexBuffer();
    }
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
    gluPerspective(50, (float) w / (float) h, 0.5f, 10);
    
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-2.5f, 2.5f, 5.0f,        // eye
              -2.5f, 2.5f, -1.0f,       // center
              0.0f, 1.0f, 0.0f);        // up
}

void reshape (int w, int h) {
    setView(w, h);
}

void renderScene() {
    GLint attrib;
    
    glUseProgram(shaderProgram);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    attrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 7, 0);
    
    attrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 7, (const GLvoid*)(sizeof(GL_FLOAT) * 4));
    
    //glPushMatrix();
    glDrawArrays(GL_QUADS, 0, numVerts);
    //glPopMatrix();
    
    glDisableVertexAttribArray(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUseProgram(0);
}

bool render = true;
int pass = 0;
int patchIndex = 0;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setView(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    
    glRotatef(-90, 1, 0, 0);
    
    if (render) {
        // do patch things
        if (pass < MAX_PASSES) {
            if (patchIndex < patches->size()) {
                Patch *patch = &patches->at(patchIndex);
                
                Vector3 look(patch->normals[0]);
                Vector3 center(patch->verts[0]);
                
                look += center;
                
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(160, (float) glutGet(GLUT_WINDOW_WIDTH) / (float) glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 10);
                
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                gluLookAt(center.x, center.y, center.z,     // eye
                          look.x, look.y, look.z,           // center
                          0.0f, 1.0f, 0.0f);                // up
                
                renderScene();
                
                int colorValues = WINDOW_WIDTH * WINDOW_HEIGHT * 3;
                float *pixelsNormalized = new float[colorValues];
                glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelsNormalized);
                
                float *pixels = new float[colorValues];
                glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixels);
                
                Vector3 sum(0, 0, 0);
                for (int i = 0; i < colorValues; i += 3) {
                    sum += Vector3(pixels[i], pixels[i + 1], pixels[i + 2]);
                }
                
                Vector3 total(0, 0, 0);
                for (int i = 0; i < colorValues; i += 3) {
                    pixelsNormalized[i] = pixels[i] == 0 ? 0 : pixels[i] / sum.x;
                    pixelsNormalized[i + 1] = pixels[i + 1] == 0 ? 0 : pixels[i + 1] / sum.y;
                    pixelsNormalized[i + 2] = pixels[i + 2] == 0 ? 0 : pixels[i + 2] / sum.z;
                    
                    pixels[i] *= pixelsNormalized[i];
                    pixels[i + 1] *= pixelsNormalized[i + 1];
                    pixels[i + 2] *= pixelsNormalized[i + 2];
                    
                    Vector3 c(pixels[i], pixels[i + 1], pixels[i + 2]);
                    total += c;
                }
                
                delete pixels;
                delete pixelsNormalized;
                
                //render the scene from the point of view of this patch
                patch->incident = total;
                patch->excident = (patch->incident * patch->reflectance) + patch->emission;
                patch->excident = Vector3(patch->excident.x * patch->color.x,
                                          patch->excident.y * patch->color.y,
                                          patch->excident.z * patch->color.z);
                patch->currentColor = patch->excident;
                
                patchIndex++;
            }
            
            // put old patches into prevPatches
            if (patchIndex == patches->size()) {
                prevPatches->clear();
                
                for (int i = 0; i < patches->size(); i++) {
                    prevPatches->push_back(patches->at(i));
                }
                
                cout << "pass " << pass + 1 << " done" << endl;
                pass++;
                patchIndex = 0;
                initializeVertexBuffer(true);
            }
        } else if (pass == MAX_PASSES) {
            cout << "radiosity done" << endl;
            render = false;
            initializeVertexBuffer(true);
        }
    } else {
        renderScene();
    }
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, specColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    objData->load(OBJ_FILE);
    
    initializeVertexBuffer();
    initializeProgram();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Radiosity");
	glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
	init();
    
	glutMainLoop();
    
    delete shaderLoader;
    delete objData;
    
    return 0;
}