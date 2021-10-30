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

        bool SetShaders();

        // vertex shader parameters
        GLint matLocation;
        GLint modelMatLocation;
        GLint transpInvModelMatLocation;

        // fragment shader uniform parameters

        GLint ambientColorLocation;  //Ka
        GLint diffuseColorLocation;  //Kd
        GLint specularColorLocation; //Ks

        GLint exponentLocation;

        GLint useAmbientTextureLocation;
        GLint useDiffuseTextureLocation;
        GLint useSpecularTextureLocation;
        GLint useExponentTextureLocation;
        GLint useBumpTextureLocation;

        GLint ambientTextureLocation; // map_Ka
        GLint diffuseTextureLocation; // map_Kd
        GLint specularTextureLocation; // map_Ks

        GLint exponentTextureLocation; // map_Ns
        GLint bumpTextureLocation; // map_bump or bump
    protected:
        void getUniformsLocations();
        bool SetupVertexShader();
        bool SetupFragmentShader();
    };

}
