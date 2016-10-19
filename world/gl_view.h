#ifndef GL_VIEW_H
#define GL_VIEW_H

#include "view.h"
#include "world.h"

class GLView;

extern GLView *GL_VIEW;

void gl_changeSize(int w, int h);
void gl_handleIdle();
void gl_renderScene();

class GLView : public View {
    public:
        GLView(World *w);

        virtual ~GLView();

        virtual void draw_agent(const Agent &a);
        virtual void draw_food(int x, int y, float q);

        void set_world(World *w);

        void changeSize(int w, int h);
        void handleIdle();
        void renderScene();

    private:
        World *world;

        bool is_paused;
        bool shall_draw;

        int speed;

        bool draw_food;

        char buf[100];
        char buf2[10];

        int mod_counter;
        int last_update;

        int frames;
};

#endif
