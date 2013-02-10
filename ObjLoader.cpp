//
//  ObjLoader.cpp
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/16/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#include "ObjLoader.h"

using namespace std;

ObjLoader::ObjLoader(string filename) {
    this->filename = filename;
    FILE *file = fopen(filename.c_str(), "r");
    char lineHeader[128];
    
    if(file == NULL){
        cout << "Could not open file: " << this->filename << endl;
        exit(1);
    }
    
    // load all vertices and normals
    while (true) {
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        
        if (strcmp(lineHeader, "v") == 0) {
            Vector3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            
            this->vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vn") == 0) {
            Vector3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            
            this->normals.push_back(normal);
        }
    }
}

ObjModel* ObjLoader::loadObj(string modelName, bool multipleModels) {
    vector<Vector3> vertices;
    vector<Vector3> normals;
    vector<Face> faces;
    
    FILE *file = fopen(this->filename.c_str(), "r");
    
    char lineHeader[128];
    
    if (multipleModels) {
        // go to the model in the obj file
        while (true) {
            int res = fscanf(file, "%s", lineHeader);
            
            if (res == EOF) {
                cout << "Could not find model: " << modelName << endl;
                exit(1);
            } else if (strcmp(lineHeader, "o") == 0) {
                char currModel[128];
                fscanf(file, "%s\n", currModel);
                
                if (strcmp(modelName.c_str(), currModel) == 0) {
                    break;
                }
            }
        }
    }
    
    // load in the faces of the model
    while (true) {
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        
        if (strcmp(lineHeader, "f") == 0) {
            Face face;
            fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n",
                   &face.vertexIndex[0], &face.normalIndex[0],
                   &face.vertexIndex[1], &face.normalIndex[1],
                   &face.vertexIndex[2], &face.normalIndex[2],
                   &face.vertexIndex[3], &face.normalIndex[3]);
            
            face.vertexIndex[0] -= 1;
            face.vertexIndex[1] -= 1;
            face.vertexIndex[2] -= 1;
            face.vertexIndex[3] -= 1;
            
            face.normalIndex[0] -= 1;
            face.normalIndex[1] -= 1;
            face.normalIndex[2] -= 1;
            face.normalIndex[3] -= 1;
            
            faces.push_back(face);
        /*} else if (strcmp(lineHeader, "mtllib") == 0) {
            // TODO: add material file loading
            fscanf(file, "%s\n", lineHeader);
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            // TODO: add material association
            fscanf(file, "%s\n", lineHeader);
        } else if (strcmp(lineHeader, "s") == 0) {
            // TODO: add smoothing groups?
            fscanf(file, "%s\n", lineHeader);*/
        } else if (strcmp(lineHeader, "o") == 0) {
            break;
        }
    }
    
    return new ObjModel(faces);
}

ObjModel* ObjLoader::loadObj(string modelName) {
    return this->loadObj(modelName, false);
}