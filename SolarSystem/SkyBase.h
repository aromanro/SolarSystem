
#pragma once

#include "Program.h"
#include "Texture.h"
#include "VertexBufferRenderable.h"

#include <glm.hpp>

namespace OpenGL {

    class SkyBase :
        public Program
    {
    public:
        SkyBase();
        ~SkyBase() override;

        virtual void Draw(const glm::mat4& mat) = 0;

        virtual bool SetShaders() = 0;
    protected:
        GLint transformMatLoc;
    };

}

