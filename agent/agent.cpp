#include "agent.h"

#include "../util/utils.h"
#include "../util/settings.h"

#include <stdio.h>
#include <iostream>
#include <string>

#include "dwraon_brain.h"

using namespace std;

Agent::Agent() {
    position = Vector2f(randf(0, settings::WIDTH), randf(0, settings::HEIGHT));

    health = 1.0 + randf(0, 0.1);
    angle  = randf(-M_PI, M_PI);

    r = 0;
    g = 0;
    b = 0;

    w1 = 0;
    w2 = 0;

    boost = false;

    spike_length = 0;

    age = 0;

    inp.resize(INPUT_SIZE, 0);
    out.resize(OUTPUT_SIZE, 0);

    herbivore = randf(0, 1);

    rep_counter = herbivore * randf(settings::REP_RATE - 0.1, settings::REP_RATE + 0.1) + (1 - herbivore) * randf(settings::REP_RATE - 0.1, settings::REP_RATE + 0.1);
    gen_counter = 0;

    hybrid = false;

    clock_f1 = randf(5, 100);
    clock_f2 = randf(5, 100);

    sound_mul = 1;

    indicator = 0;

    ir = 0;
    ig = 0;
    ib = 0;

    select_flag = 0;

    d_food = 0;
    give   = 0;

    id = 0;

    mutrate1 = 0.003;
    mutrate2 = 0.05;
}

void Agent::init_event(float size, float r, float g, float b) {
    indicator = size;

    ir = r;
    ig = g;
    ib = b;
}

void Agent::tick() {
    brain.tick(inp, out);
}

Agent Agent::reproduce(float mr, float mr2) {
    Agent baby;

    Vector2f behind(settings::BOT_RADIUS, 0);
    behind.rotate(-baby.angle);

    baby.position = this -> position + behind + Vector2f(randf(-settings::BOT_RADIUS * 2, settings::BOT_RADIUS * 2), randf(-settings::BOT_RADIUS * 2, settings::BOT_RADIUS * 2));

    if (baby.position.x < 0) baby.position.x = settings::WIDTH;
    if (baby.position.x > settings::WIDTH) baby.position.x = 0;

    if (baby.position.y < 0) baby.position.y = settings::HEIGHT;
    if (baby.position.y > settings::HEIGHT) baby.position.y = 0;

    baby.gen_counter = this -> gen_counter + 1;

    baby.mutrate1 = this -> mutrate1;
    baby.mutrate2 = this -> mutrate2;

    if (randf(0, 1) < 0.2) {
        baby.mutrate1 = randn(this -> mutrate1, settings::META_MUTRATE1);
    }

    if (randf(0, 1) < 0.2) {
        baby.mutrate2 = randn(this -> mutrate1, settings::META_MUTRATE2);
    }

    if (this -> mutrate1 < 0.001) this -> mutrate1 = 0.001;
    if (this -> mutrate2 < 0.02) this -> mutrate2 = 0.02;

    baby.herbivore = clamp(randn(this -> herbivore, mr2 * 4));

    if (randf(0, 1) < mr * 5) {
        baby.clock_f1 = randn(baby.clock_f1, mr2);
    }

    if (randf(0, 1) < mr * 5) {
        baby.clock_f2 = randn(baby.clock_f2, mr2);
    }

    if (baby.clock_f1 < 2) baby.clock_f1 = 2;
    if (baby.clock_f2 < 2) baby.clock_f2 = 2;

    baby.brain = this -> brain;
    baby.brain.mutate(mr, mr2);

    return baby;
}
