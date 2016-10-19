#include "gl_view.h"
#include "../util/config.h"

#include <ctime>

#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>

void gl_changeSize(int w, int h) {
    GL_VIEW -> changeSize(w, h);
}

void gl_handleIdle() {
    GL_VIEW -> handleIdle();
}

void gl_renderScene() {
    GL_VIEW -> renderScene();
}

void draw_text(float x, float y, void *font, const char *string, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    int len = (int)strlen(string);

    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVITICA_12, string[i]);
    }
}

void draw_circle(float x, float y, float r) {
    float n;

    for (int k = 0; k < 17; k++) {
        n = k * (M_PI / 8);

        glVertex3f(x + r * sin(n), y + r * cos(n), 0);
    }
}

GLView::GLView(World *s)
    : world(world)
      is_paused(false),
      shall_draw(true),
      speed(1),
      draw_food(true),
      mod_counter(0),
      frames(0),
      last_update(0) {}

GLView::~GLView() {}

void GLView::changeSize(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, settings::WIDTH, settings::HEIGHT, 0, 0, 1);
}

void GLView::handleIdle() {
    mod_counter++;

    int current_time = glutGet(GLUT_ELAPSED_TIME);

    frames++;
    if ((current_time - last_update) >= 1000) {
        sprintf(buf, "Nilq's Optimized Creatures, FPS: %d", frames);
        glutSetWindowTitle(buf);

        frames = 0;
        last_update = current_time;
    }

    if (speed <= 0 && shall_draw) {
        clock_t end_wait;
        float mul = -0.005 * (speed - 1);

        end_wait = clock() + mul * CLOCKS_PER_SEC;

        while (clock() < end_wait) {
            // get rekt CPU
        }
    }

    if (shall_draw) {
        if (speed > 0) {
            if (mod_counter % speed == 0) {
                renderScene();
            }

        } else {
            renderScene();
        }
    }
}

void GLView::renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    world -> draw(this);

    glPopMatrix();
    glutSwapBuffers();
}
