#ifndef AGENT_H
#define AGENT_H

#include "dwraon_brain.h"
#include "../util/vmath.h"

#include <vector>
#include <string>

class Agent {
    public:
        Agent();

        void init_event(float size, float r, float g, float b);
        void tick();

        Agent reproduce(float mr, float mr2);
        Agent cross_over(const Agent &other);

        Vector2f position;

        float health;
        float angle;

        float r;
        float g;
        float b;

        float w1;
        float w2;

        bool boost;

        float spike_length;

        int age;

        std::vector<float> inp;
        std::vector<float> out;

        float herbivore;

        float rep_counter;
        int gen_counter;

        bool hybrid;

        float clock_f1;
        float clock_f2;

        float sound_mul;

        float indicator;

        float ir;
        float ig;
        float ib;

        int select_flag;

        float d_food;
        float give;

        int id;

        float mutrate1;
        float mutrate2;

        DWRAONBrain brain;
};

#endif
