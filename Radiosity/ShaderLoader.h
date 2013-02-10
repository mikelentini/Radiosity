//
//  ShaderLoader.h
//  OpenGL Learning
//
//  Created by Mike Lentini on 1/15/13.
//  Copyright (c) 2013 Mike Lentini. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <OpenGL/gl.h>
#include <vector>

using namespace std;

class ShaderLoader {
    public:
        const char* readShaderFile(string inFile);
        GLuint createShader(GLenum eShaderType, const string &strShaderFile);
        GLuint createProgram(vector<GLuint> shaderList);
};
