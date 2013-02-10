//
//  ObjModel.h
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/16/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#ifndef __OpenGL_Learning__ObjModel__
#define __OpenGL_Learning__ObjModel__

#include <iostream>
#include <vector>
#include "vecmath.h"

using namespace std;

struct Face {
    int vertexIndex[4];
    int normalIndex[4];
};

inline ostream& operator<<(ostream& os, const Face& f) {
    os << "vertices: " << f.vertexIndex[0] << ", " << f.vertexIndex[1] <<
        ", " << f.vertexIndex[2] << ", " << f.vertexIndex[3];
    return os;
}

class ObjModel {
public:
    ObjModel(vector<Face> faces);
    vector<Face> faces;
};

#endif /* defined(__OpenGL_Learning__ObjModel__) */
