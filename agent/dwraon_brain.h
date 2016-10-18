#ifndef DWRAONBRAIN_H
#define DWRAONBRAIN_H

//#include "settings.h"
#include "../util/utils.h"

#include <vector>

#define INPUT_SIZE 20
#define OUTPUT_SIZE 9

#define BRAIN_SIZE 100
#define CONNS 3

class Box {
    public:
        Box();

        int type;
        float kp; // damping strength

        std::vector<float> w; // weights of connected boxes
        std::vector<int> id;  // indexes of connected boxes in 'boxes[]'

        std::vector<char> notted;

        float bias;

        float target;
        float out;
};

class DWRAONBrain {
    public:
        std::vector<Box> boxes;

        DWRAONBrain();
        DWRAONBrain(const DWRAONBrain &other);

        virtual DWRAONBrain &operator=(const DWRAONBrain &other);

        void tick(std::vector<float> &in, std::vector<float> &out);
        void mutate(float mr, float mr2);

        DWRAONBrain cross_over(const DWRAONBrain &other);
};

#endif
