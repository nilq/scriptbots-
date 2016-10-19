#include "world/gl_view.h"
#include "world/world.h"
#include <ctime>
#include "config.h"

#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

GLView *GL_VIEW = new GLView(0);
#include <stdio.h>

int main(int argc, char **argv) {
    srand(time(0));

    World* world = new World();
    GL_VIEW -> set_world(world);

    return 0;
}
