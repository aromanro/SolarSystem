#pragma once
#include "Program.h"

#include "VertexBufferRenderable.h"

#include <glm.hpp>


namespace OpenGL {

    class SpaceshipProgram :
        public Program
    {
    public:
        SpaceshipProgram();
        virtual ~SpaceshipProgram();

        bool SetupShaders();


        // vertex shader parameters
        GLint matLocation;
        GLint modelMatLocation;
        GLint transpInvModelMatLocation;

        // fragment shader uniform parameters

    protected:
        void getUniformsLocations();
        bool SetupVertexShader();
        bool SetupFragmentShader();
    };

}
