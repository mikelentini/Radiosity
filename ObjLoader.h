//
//  ObjLoader.h
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/16/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#ifndef __OpenGL_Learning__ObjLoader__
#define __OpenGL_Learning__ObjLoader__

#include <iostream>
#include <string>
#include <vector>
#include "vecmath.h"
#include "ObjModel.h"

using namespace std;

class ObjLoader {
public:
    ObjLoader(string filename);
    ObjModel* loadObj(string modelName);
    ObjModel* loadObj(string modelName, bool multipleModels);

    vector<Vector3> vertices;
    vector<Vector3> normals;

private:
    string filename;
};

#endif /* defined(__OpenGL_Learning__ObjLoader__) */
