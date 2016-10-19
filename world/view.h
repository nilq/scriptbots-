#ifndef VIEW_H
#define VIEW_H

#include "../agent/agent.h"

class View {
    public:
        virtual void draw_agent(const Agent &a)       = 0;
        virtual void draw_food(int x, int y, float q) = 0;
};

#endif
