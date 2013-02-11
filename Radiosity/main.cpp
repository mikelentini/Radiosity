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
vector<double*> *ambs;
vector<double*> *diffs;
vector<double*> *specs;
vector<double> *Nss;
vector<double> *transs;
vector<double> *Nis;

Vector3 black(0.0f, 0.0f, 0.0f);
Vector3 white(1.0f, 1.0f, 1.0f);

struct Patch {
    Vector3 excident;
    Vector3 incident;
    float reflectance;
    float emission;
    
    Vector3 normal;
    Vector3 color;
    Vector3 verts[4];
};

vector<Patch> *patches;
vector<Patch> *prevPatches;
int MAX_PASSES = 1;

void initializeVertexBuffer() {
    vertices = new vector<float>();
    normals = new vector<float>();
    ambs = new vector<double*>();
    diffs = new vector<double*>();
    specs = new vector<double*>();
    Nss = new vector<double>();
    transs = new vector<double>();
    Nis = new vector<double>();
    
    prevPatches = new vector<Patch>();
    patches = new vector<Patch>();
    
    glGenBuffers(1, &vbo);
    
    for(int i = 0; i < objData->faceCount; i++)	{
		obj_face *face = objData->faceList[i];
        int materialIndex = face->material_index;
        double *amb = objData->materialList[materialIndex]->amb;
        double *diff = objData->materialList[materialIndex]->diff;
        double *spec = objData->materialList[materialIndex]->spec;
        double Ns = objData->materialList[materialIndex]->shiny;
        double trans = objData->materialList[materialIndex]->trans;
        double Ni = objData->materialList[materialIndex]->refract_index;
        
        obj_vector *vert1 = objData->vertexList[face->vertex_index[0]];
        obj_vector *vert2 = objData->vertexList[face->vertex_index[1]];
        obj_vector *vert3 = objData->vertexList[face->vertex_index[2]];
        obj_vector *vert4 = objData->vertexList[face->vertex_index[3]];
        
        // add face as a patch to the patches and prevPatches vectors
        Patch patch;
        if (strcmp(objData->materialList[materialIndex]->name, "light") == 0) {
            patch = {
                Vector3(white),
                Vector3(white),
                (float) Ns,
                1.0f,
                Vector3(objData->normalList[face->normal_index[0]]->e[0],
                        objData->normalList[face->normal_index[0]]->e[1],
                        objData->normalList[face->normal_index[0]]->e[1]),
                Vector3(amb[0], amb[1], amb[2]),
                {
                    Vector3(vert1->e[0], vert1->e[1], vert1->e[2]),
                    Vector3(vert2->e[0], vert2->e[1], vert2->e[2]),
                    Vector3(vert3->e[0], vert3->e[1], vert3->e[2]),
                    Vector3(vert4->e[0], vert4->e[1], vert4->e[2])
                }
            };
        } else {
            patch = {
                Vector3(black),
                Vector3(black),
                (float) Ns,
                1.0f,
                Vector3(objData->normalList[face->normal_index[0]]->e[0],
                        objData->normalList[face->normal_index[0]]->e[1],
                        objData->normalList[face->normal_index[0]]->e[1]),
                Vector3(amb[0], amb[1], amb[2]),
                {
                    Vector3(vert1->e[0], vert1->e[1], vert1->e[2]),
                    Vector3(vert2->e[0], vert2->e[1], vert2->e[2]),
                    Vector3(vert3->e[0], vert3->e[1], vert3->e[2]),
                    Vector3(vert4->e[0], vert4->e[1], vert4->e[2])
                }
            };
        }
        
        patches->push_back(patch);
        prevPatches->push_back(patch);
        
		for(int j = 0; j < face->vertex_count; j++) {
            obj_vector *vert = objData->vertexList[face->vertex_index[j]];
            obj_vector *normal = objData->normalList[face->normal_index[j]];
            
            vertices->push_back(vert->e[0]);
            vertices->push_back(vert->e[1]);
            vertices->push_back(vert->e[2]);
            vertices->push_back(1.0f);
            
            normals->push_back(normal->e[0]);
            normals->push_back(normal->e[1]);
            normals->push_back(normal->e[2]);
            
            ambs->push_back(amb);
            diffs->push_back(diff);
            specs->push_back(spec);
            Nss->push_back(Ns);
            transs->push_back(trans);
            Nis->push_back(Ni);
		}
	}
    
    // do patch things
    for (int pass = 0; pass < MAX_PASSES; pass++) {
        for (int i = 0; i < patches->size(); i++) {
            Patch patch = patches->at(i);
            
            //render the scene from the point of view of this patch
            //patch.incident = sum of incident light in rendering
            //patch.excident = (patch.incident * patch.reflectance) + patch.emission
        }
    }
    
    long arraySize = vertices->size() + normals->size() + (ambs->size() * 3) + (diffs->size() * 3) +
        (specs->size() * 3) + Nss->size() + transs->size() + Nis->size();
    float vertArray[arraySize];
    
    /* { 1.0, 1.0, 1.0, 1.0     <- vert position
     *   1.0, 1.0, 1.0          <- normal
     *   1.0, 1.0, 1.0          <- amb
     *   1.0, 1.0, 1.0          <- diff
     *   1.0, 1.0, 1.0      	<- spec
     *   1.0, 1.0, 1.0 }        <- Ns, trans, Ni
     */
    for (int i = 0, j = 0, k = 0, l = 0; i < arraySize; i += 19, j += 4, k += 3, l++) {
        vertArray[i] = vertices->at(j);
        vertArray[i + 1] = vertices->at(j + 1);
        vertArray[i + 2] = vertices->at(j + 2);
        vertArray[i + 3] = vertices->at(j + 3);
        
        vertArray[i + 4] = normals->at(k);
        vertArray[i + 5] = normals->at(k + 1);
        vertArray[i + 6] = normals->at(k + 2);
        
        vertArray[i + 7] = ambs->at(l)[0];
        vertArray[i + 8] = ambs->at(l)[1];
        vertArray[i + 9] = ambs->at(l)[2];
        
        vertArray[i + 10] = diffs->at(l)[0];
        vertArray[i + 11] = diffs->at(l)[1];
        vertArray[i + 12] = diffs->at(l)[2];
        
        vertArray[i + 13] = specs->at(l)[0];
        vertArray[i + 14] = specs->at(l)[1];
        vertArray[i + 15] = specs->at(l)[2];
        
        vertArray[i + 16] = Nss->at(l);
        vertArray[i + 17] = transs->at(l);
        vertArray[i + 18] = Nis->at(l);
    }
    
    numVerts = (int) vertices->size() / 4;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertArray), vertArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    delete vertices;
    delete normals;
    delete ambs;
    delete specs;
    delete diffs;
    delete transs;
    delete Nis;
    delete Nss;
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
    gluPerspective(50, (float) w / (float) h, 0.5f, 100);
    
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-2.5f, 2.5f, 5.0f,        // eye
              -2.5f, 2.5f, -1.0f,       // center
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
    glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, 0);
    
    attrib = glGetAttribLocation(shaderProgram, "normal");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 4));
    
    attrib = glGetAttribLocation(shaderProgram, "amb");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 7));
    
    attrib = glGetAttribLocation(shaderProgram, "diff");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 10));
    
    attrib = glGetAttribLocation(shaderProgram, "spec");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 13));
    
    attrib = glGetAttribLocation(shaderProgram, "Ns");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 16));
    
    attrib = glGetAttribLocation(shaderProgram, "trans");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 17));
    
    attrib = glGetAttribLocation(shaderProgram, "Ni");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 19, (const GLvoid*)(sizeof(GL_FLOAT) * 18));
    
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glDrawArrays(GL_QUADS, 0, numVerts);
    //glDrawArrays(GL_POINTS, 0, numVerts);
    glPopMatrix();
    
    glDisableVertexAttribArray(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUseProgram(0);
    
    glutSwapBuffers();
    //glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, specColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
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