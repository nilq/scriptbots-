#include "world/gl_view.h"
#include "world/world.h"

#include "util/settings.h"
#include "config.h"

#include <ctime>

#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>

GLView *GLVIEW = new GLView(0);

int main(int argc, char **argv) {

    srand(time(0));

    World *world = new World();
    GLVIEW -> set_world(world);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(settings::WIDTH, settings::HEIGHT);
    glutCreateWindow("Temporary Title <- Right Here");

    glClearColor(0.9f, 0.9f, 1.0f, 0.0f);
    glutDisplayFunc(gl_renderScene);
    glutIdleFunc(gl_handleIdle);
    glutReshapeFunc(gl_changeSize);

    glutMainLoop();

    return 0;
}
