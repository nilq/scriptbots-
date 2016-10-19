#include "dwraon_brain.h"

using namespace std;

Box::Box() {
    w.resize(CONNS, 0);
    id.resize(CONNS, 0);
    notted.resize(CONNS, 0);

    for (int i = 0; i < CONNS; i++) {
        w[i]  = randf(0.1, 2);
        id[i] = randi(0, BRAIN_SIZE);

        if (randf(0, 1) < 0.2) {
            // 20% of brian at least should connect to input.
            id[i] = randi(0, INPUT_SIZE);
        }

        notted[i] = randf(0, 1) < 0.5;
    }

    type = (randf(0, 1) > 0.5) ? 0 : 1;
    kp   = randf(0.8, 1);
    bias = randf(-1, 1);

    out = 0;
    target = 0;
}

DWRAONBrain::DWRAONBrain() {
    for (int i = 0; i < BRAIN_SIZE; i++) {
        Box a;

        boxes.push_back(a);

        boxes[i].out    = a.out;
        boxes[i].target = a.target;
        boxes[i].type   = a.type;
        boxes[i].bias   = a.bias;

        for (int j = 0; j < CONNS; j++) {
            boxes[i].notted[j] = a.notted[j];
            boxes[i].w[j]      = a.w[j];
            boxes[i].id[j]     = a.id[j];

            if (randf(0, 1) < 0.05) {
                boxes[i].id[j] = 0;
            }

            if (randf(0, 1) < 0.05) {
                boxes[i].id[j] = 5;
            }

            if (randf(0, 1) < 0.05) {
                boxes[i].id[j] = 12;
            }

            if (randf(0, 1) < 0.05) {
                boxes[i].id[j] = 4;
            }

            if (i < BRAIN_SIZE / 2) {
                boxes[i].id[j] = randi(0, INPUT_SIZE);
            }
        }
    }
}

DWRAONBrain::DWRAONBrain(const DWRAONBrain &other) {
    boxes = other.boxes;
}

DWRAONBrain &DWRAONBrain::operator=(const DWRAONBrain &other) {

    if (this != &other) {
        boxes = other.boxes;
    }

    return *this;
}

void DWRAONBrain::tick(vector<float> &in, vector<float> &out) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        boxes[i].out = in[i];
    }

    for (int i = INPUT_SIZE; i < BRAIN_SIZE; i++) {

        Box* box = &boxes[i];

        if (box -> type == 0) { // AND
            float res = 1;

            for (int j = 0; j < CONNS; j++) {
                int id = box -> id[j];
                float n = boxes[id].out;

                if (box -> notted[j]) {
                    n = 1 - n;
                }

                res = res * n;
            }

            res *= box -> bias;

            box -> target = res;

        } else { // OR
            float res = 0;

            for (int j = 0; j < CONNS; j++) {
                int id = box -> id[j];
                float n = boxes[id].out;

                if (box -> notted[j]) {
                    n = 1 - n;
                }

                res += n * box -> w[j];
            }

            res += box -> bias;
            box -> target = res;
        }

        clamp(box -> target);
    }

    for (int i = INPUT_SIZE; i < BRAIN_SIZE; i++) {
        Box* box = &boxes[i];

        box -> out += (box -> target - box -> out) * box -> kp;
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        out[i] = boxes[BRAIN_SIZE - 1 - i].out;
    }
}

void DWRAONBrain::mutate(float mr, float mr2) {
    for (int j = 0; j < BRAIN_SIZE; j++) {
        if (randf(0, 1) < mr * 3) {
            boxes[j].bias += randn(0, mr2);
        }

        if (randf(0, 1) < mr * 3) {
            int rc = randi(0, CONNS);

            boxes[j].w[rc] += randn(0, mr2);

            if (boxes[j].w[rc] < 0.01) {
                boxes[j].w[rc] = 0.0;
            }
        }

        if (randf(0, 1) < mr) {
            int rc = randi(0, CONNS);
            int ri = randi(0, BRAIN_SIZE);

            boxes[j].id[rc] = ri;
        }

        if (randf(0, 1) < mr) {
            int rc = randi(0, CONNS);

            boxes[j].notted[rc] = !boxes[j].notted[rc];
        }

        if (randf(0, 1) < mr) {
            boxes[j].type = 1 - boxes[j].type;
        }
    }
}

DWRAONBrain DWRAONBrain::cross_over(const DWRAONBrain &other) {
    DWRAONBrain new_brain(*this);

    for (int i = 0; i < new_brain.boxes.size(); i++) {
        new_brain.boxes[i].bias = randf(0, 1) < 0.5 ? this -> boxes[i].bias : other.boxes[i].bias;
        new_brain.boxes[i].kp = randf(0, 1) < 0.5 ? this -> boxes[i].kp : other.boxes[i].kp;
        new_brain.boxes[i].type = randf(0, 1) < 0.5 ? this -> boxes[i].type : other.boxes[i].type;

        for (int j = 0; j < new_brain.boxes.size(); j++) {
            new_brain.boxes[i].id[j] = randf(0, 1) < 0.5 ? this -> boxes[i].id[j] : other.boxes[i].id[j];
            new_brain.boxes[i].notted[j] = randf(0, 1) < 0.5 ? this -> boxes[i].notted[j] : other.boxes[i].notted[j];
            new_brain.boxes[i].w[j] = randf(0, 1) < 0.5 ? this -> boxes[i].w[j] : other.boxes[i].w[j];
        }
    }

    return new_brain;
}
