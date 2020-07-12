
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
        virtual ~SkyBase();

        bool SetShaders();

    protected:
        GLint transformMatLoc;
    };

}

