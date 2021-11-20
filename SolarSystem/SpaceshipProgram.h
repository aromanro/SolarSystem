#pragma once
#include "Program.h"

#include "VertexBufferRenderable.h"
#include "SolarSystemBodies.h"

#include <glm.hpp>


namespace OpenGL {

    class SpaceshipProgram :
        public Program
    {
    public:
        SpaceshipProgram();
        virtual ~SpaceshipProgram();

        bool SetShaders(unsigned int nrLights);

        void SetupLights(BodyPropList& BodyProperties);

        struct Light {
            GLint lightDirPos = 0;
            GLint attenPos = 0;
            glm::dvec3 lightPos;
        };

        std::vector<Light> lights;

        unsigned int nrlights;

        // vertex shader parameters
        GLint matLocation = 0;
        GLint modelMatLocation = 0;
        GLint transpInvModelMatLocation = 0;

        // fragment shader uniform parameters

        GLint illuminationLocation = 0;

        GLint ambientColorLocation = 0;  //Ka
        GLint diffuseColorLocation = 0;  //Kd
        GLint specularColorLocation = 0; //Ks

        GLint exponentLocation = 0;

        GLint useAmbientTextureLocation = 0;
        GLint useDiffuseTextureLocation = 0;
        GLint useSpecularTextureLocation = 0;
        GLint useExponentTextureLocation = 0;
        GLint useBumpTextureLocation = 0;

        GLint ambientTextureLocation = 0; // map_Ka
        GLint diffuseTextureLocation = 0; // map_Kd
        GLint specularTextureLocation = 0; // map_Ks

        GLint exponentTextureLocation = 0; // map_Ns
        GLint bumpTextureLocation = 0; // map_bump or bump

        GLint viewPosLocation = 0;
    protected:
        void getUniformsLocations();
        bool SetupVertexShader();
        bool SetupFragmentShader();
    };

}
