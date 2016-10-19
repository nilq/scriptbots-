#ifndef WORLD_H
#define WORLD_H

#include "../agent/agent.h"
#include "../util/settings.h"

#include <vector>

class World {
    public:
        World();
        ~World();

        void update();
        void reset();

        //void draw(View *view);

        bool is_closed() const;
        void set_closed(bool closed);

        //std::pair<int, int> num_herb_carn() const;

        int num_agents() const;
        int epoch() const;

    private:
        void process_input();
        void process_output();
        void process_brain();

        void reproduce(int i, float mr, float mr2);

        void add_crossover(int n);
        void add_random(int n);

        int mod_counter;
        int epoch_counter;
        int id_counter;

        std::vector<Agent> agents;

        int f_width;
        int f_height;

        int f_x;
        int f_y;

        float food[settings::WIDTH / settings::CZ][settings::HEIGHT / settings::CZ];

        bool closed;
};

#endif
