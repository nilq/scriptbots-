#include "world.h"

#include <ctime>

#include "../util/settings.h"
#include "../util/utils.h"
#include "../util/vmath.h"

using namespace std;

World::World() :
    mod_counter   (0),
    epoch_counter (0),
    id_counter    (0),
    f_width       (settings::WIDTH / settings::CZ),
    f_height      (settings::HEIGHT / settings::CZ),
    closed        (false)

    {

    add_random(settings::NUM_BOTS);

    for (int x = 0; x < f_width; x++) {
        for (int y = 0; y < f_height; y++) {
            food[x][y] = 0;
        }
    }
}

void World::update() {
    mod_counter++;

    if (mod_counter % 100 == 0) {
        for (int i = 0; i < agents.size(); i++) {
            agents[i].age++;
        }
    }

    if (mod_counter % 10000 == 0) {
        mod_counter = 0;
        epoch_counter++;
    }

    if (mod_counter % settings::FOOD_ADD_FREQ == 0) {
        f_x = randi(0, f_width);
        f_y = randi(0, f_height);

        food[f_x][f_y] = settings::FOOD_MAX;
    }

    process_input();
    process_brain();
    process_output();

    for (int i = 0; i < agents.size(); i++) {
        Agent *agent = &agents[i];

        float base_loss = 0.0002 + 0.0001 * (abs(agent -> w1) + abs(agent -> w2)) / 2;

        if (agent -> w1 + agent -> w2 < 0.2) {
            base_loss = 0;
        }

        base_loss += 0.00005 * agent -> sound_mul;

        if (agent -> boost) {
            agent -> health -= base_loss * settings::BOOST_SIZE_MULT * 2;

        } else {
            agent -> health -= base_loss;
        }
    }

    for (int i = 0; i < agents.size(); i++) {
        Agent *agent_i = &agents[i];

        if (agent_i -> health <= 0) {
            int num_around = 0;

            for (int j = 0; j < agents.size(); j++) {
                if (j == i) {
                    continue;
                }

                if ((agent_i -> position - agent_i -> position).length() < settings::FOOD_DISTRIBUTION_RADIUS) {
                    num_around++;
                }
            }

            if (num_around > 0) {
                for (int j = 0; j < agents.size(); j++) {
                    if (j == i) {
                        continue;
                    }

                    Agent *agent_j = &agents[j];

                    if (agents[j].herbivore < 0.7) {
                        if ((agent_i -> position - agent_j -> position).length() < settings::FOOD_DISTRIBUTION_RADIUS) {
                            agent_j -> health      += 3 * (1 - agent_j -> herbivore) * (1 - agent_j -> herbivore) / num_around;
                            agent_j -> rep_counter -= 2 * (1 - agent_j -> herbivore) * (1 - agent_j -> herbivore) / num_around;

                            if (agent_j -> health > 2) {
                                agent_j -> health = 2;
                            }

                            agent_j -> init_event(30, 1, 1, 1);
                        }
                    }
                }
            }
        }
    }

    vector<Agent>::iterator it = agents.begin();

    do {
        if (it -> health <= 0) {
            it = agents.erase(it);

        } else {
            it++;
        }
    } while (it != agents.end());

    for (int i = 0; i < agents.size(); i++) {

        Agent *agent = &agents[i];

        if (agent -> rep_counter <= 0 && agent -> health > 0.65) {
            reproduce(i, agent -> mutrate1, agent -> mutrate2);

            agent -> rep_counter = agent -> herbivore * randf(settings::REP_RATE - 0.1, settings::REP_RATE + 0.1) + (1 - agent -> herbivore) * randf(settings::REP_RATE - 0.1, settings::REP_RATE + 0.1);
        }
    }

    if (!closed) {
        if (agents.size() < settings::NUM_BOTS) {
            add_random(1);
        }

        if (mod_counter % 200 == 0) {
            if (randf(0, 1) < 0.5) {
                add_random(1);
            } else {
                add_crossover(1);
            }
        }
    }
}

void World::process_input() {
    float pi8  = M_PI / 8 / 2;
    float pi38 = pi8 * 3;

    for (int i = 0; i < agents.size(); i++) {
        Agent *a = &agents[i];

        a -> inp[11] = clamp(a -> health / 2);

        int cx = (int)a -> position.x / settings::CZ;
        int cy = (int)a -> position.y / settings::CZ;

        float p1, r1, g1, b1;
        float p2, r2, g2, b2;
        float p3, r3, g3, b3;

        p1 = 0; r1 = 0; g1 = 0; b1 = 0;
        p2 = 0; r2 = 0; g2 = 0; b2 = 0;
        p3 = 0; r3 = 0; g3 = 0; b3 = 0;

        float so_accum = 0; // sound
        float sm_accum = 0; // smell
        float he_accum = 0; // hearing

        float blood = 0;

        for (int j = 0; j < agents.size(); j++) {
            if (j == i) {
                continue;
            }

            Agent *a2 = &agents[j];

            if (a -> position.x < a2 -> position.x - settings::DIST || a -> position.x > a2 -> position.x + settings::DIST) {
                if (a -> position.y < a2 -> position.y - settings::DIST || a -> position.y > a2 -> position.y + settings::DIST) {
                    continue;
                }
            }

            float d = (a -> position - a2 -> position).length();

            if (d < settings::DIST) {
                sm_accum += 0.3 * (settings::DIST - d) / settings::DIST;
                so_accum += 0.4 * (settings::DIST - d) / settings::DIST * (max(fabs(a2 -> w1), fabs(a2 -> w2)));

                he_accum += a2 -> sound_mul * (settings::DIST - d) / settings::DIST;

                float ang = (a2 -> position - a -> position).get_angle();

                float l_eye_angle = a -> angle - pi8;
                float r_eye_angle = a -> angle + pi8;

                float back_angle  = a -> angle + M_PI;
                float forw_angle  = a -> angle;

                if (l_eye_angle < -M_PI) l_eye_angle += 2 * M_PI;
                if (r_eye_angle > M_PI)  r_eye_angle -= 2 * M_PI;
                if (back_angle > M_PI)   back_angle  -= 2 * M_PI;

                float diff1 = l_eye_angle - ang;

                if (fabs(diff1) > M_PI) {
                    diff1 = 2 * M_PI - fabs(diff1);
                }

                diff1 = fabs(diff1);

                float diff2 = r_eye_angle - ang;

                if (fabs(diff2) > M_PI) {
                    diff2 = 2 * M_PI - fabs(diff2);
                }

                float diff3 = back_angle - ang;

                if (fabs(diff3) > M_PI) {
                    diff3 = 2 * M_PI - fabs(diff3);
                }

                float diff4 = forw_angle - ang;

                if (fabs(forw_angle) > M_PI) {
                    diff4 = 2 * M_PI - fabs(forw_angle);
                }

                diff4 = fabs(diff4);

                if (diff1 < pi38) {
                    float mul = settings::EYE_SENS * ((pi38 - diff1) / pi38) * ((settings::DIST - d) / settings::DIST);

                    p1 += mul * (d / settings::DIST);

                    r1 += mul * a2 -> r;
                    g1 += mul * a2 -> g;
                    b1 += mul * a2 -> b;
                }

                if (diff2 < pi38) {
                    float mul = settings::EYE_SENS * ((pi38 - diff1) / pi38) * ((settings::DIST - d) / settings::DIST);

                    p2 = mul * (d / settings::DIST);

                    r2 += mul * a2 -> r;
                    g2 += mul * a2 -> g;
                    b2 += mul * a2 -> b;
                }

                if (diff3 < pi38) {
                    float mul = settings::EYE_SENS * ((pi38 - diff1) / pi38) * ((settings::DIST - d) / settings::DIST);

                    p3 = mul * (d / settings::DIST);

                    r3 += mul * a2 -> r;
                    g3 += mul * a2 -> g;
                    b3 += mul * a2 -> b;
                }

                if (diff4 < pi38) {
                    float mul = settings::BLOOD_SENS * ((pi38 - diff1) / pi38) * ((settings::DIST - d) / settings::DIST);

                    blood += mul * (1 - a2 -> health / 2);
                }
            }
        }

        a -> inp[0] = clamp(p1);
        a -> inp[1] = clamp(r1);
        a -> inp[2] = clamp(g1);
        a -> inp[3] = clamp(b1);
        a -> inp[5] = clamp(p2);
        a -> inp[6] = clamp(r2);
        a -> inp[7] = clamp(g2);
        a -> inp[8] = clamp(b2);

        a -> inp[9]  = clamp(so_accum);
        a -> inp[10] = clamp(sm_accum);

        a -> inp[12] = clamp(p3);
        a -> inp[13] = clamp(r3);
        a -> inp[14] = clamp(g3);
        a -> inp[15] = clamp(b3);

        a -> inp[16] = abs(sin(mod_counter / a -> clock_f1));
        a -> inp[17] = abs(sin(mod_counter / a -> clock_f2));

        a -> inp[18]= clamp(he_accum);
        a -> inp[19]= clamp(blood);
    }
}

void World::process_output() {
    for (int i = 0; i < agents.size(); i++) {
        Agent *a = &agents[i];

        a -> w1 = a -> out[0];
        a -> w2 = a -> out[1];

        a -> r = a -> out[2];
        a -> g = a -> out[3];
        a -> b = a -> out[4];

        a -> boost = a -> out[6] > 0.5;

        a -> sound_mul = a -> out[7];
        a -> give = a -> out[8];

        float g = a -> out[5];

        if (a -> spike_length < g) {
            a -> spike_length += settings::SPIKE_SPEED;
        } else if (a -> spike_length > g) {
            a -> spike_length = g;
        }
    }

    for (int i = 0; i < agents.size(); i++) {
        Agent *a = &agents[i];

        Vector2f v(settings::BOT_RADIUS / 2, 0);
        v.rotate(a -> angle + M_PI / 2);

        Vector2f w1p = a -> position + v;
        Vector2f w2p = a -> position - v;

        float bw1 = settings::BOT_SPEED * a -> w1;
        float bw2 = settings::BOT_SPEED * a -> w2;

        if (a -> boost) {
            bw1 *= settings::BOOST_SIZE_MULT;
            bw2 *= settings::BOOST_SIZE_MULT;
        }

        Vector2f vv = w2p - a -> position;
        vv.rotate(-bw1);

        a -> position = w2p - vv;
        a -> angle   -= bw1;

        if (a -> angle < -M_PI) {
            a -> angle = M_PI - (-M_PI - a -> angle);
        }

        vv = a -> position - w1p;
        vv.rotate(bw2);

        a -> position = w1p + vv;
        a -> angle   += bw2;

        if (a -> angle > M_PI) {
            a -> angle = -M_PI + (a -> angle - M_PI);
        }

        if (a -> position.x < 0) a -> position.x = settings::WIDTH + a -> position.x;
        if (a -> position.x >= settings::WIDTH) a -> position.x = a -> position.x - settings::WIDTH;

        if (a -> position.y < 0) a -> position.y = settings::HEIGHT + a -> position.y;
        if (a -> position.y >= settings::HEIGHT) a -> position.y = a -> position.y - settings::HEIGHT;
    }

    for (int i = 0; i < agents.size(); i++) {
        Agent *agent = &agents[i];

        int cx = (int)agent -> position.x / settings::CZ;
        int cy = (int)agent -> position.y / settings::CZ;

        float f = food[cx][cy];

        if (f > 0 && agent -> health < 2) {
            float itk = min(f, settings::FOOD_INTAKE);
            float speed_mul = (1 - (abs(agent -> w1) + abs(agent -> w2)) / 2) / 2 + 0.5;

            itk *= agent -> herbivore * agent -> herbivore * speed_mul;

            agent -> health += itk;
            agent -> rep_counter -= 3 * itk;

            food[cx][cy] -= min(f, settings::FOOD_WASTE);
        }
    }

    for (int i = 0; i < agents.size(); i++) {
        agents[i].d_food = 0;
    }

    for (int i = 0; i < agents.size(); i++) {
        Agent *a = &agents[i];

        if (a -> give > 0.5) {
            for (int j = 0; j < agents.size(); j++) {
                Agent *aj = &agents[j];
                float d = (a -> position - aj -> position).length();

                if (d < settings::FOOD_SHARING_DISTANCE) {
                    if (aj -> health < 2) {
                        aj -> health += settings::FOOD_TRANSFER;
                    }

                    a -> health  -= settings::FOOD_TRANSFER;
                    aj -> d_food += settings::FOOD_TRANSFER;
                    a -> d_food  -= settings::FOOD_TRANSFER;
                }
            }
        }
    }

    if (mod_counter % 2 == 0) {
        for (int i = 0; i < agents.size(); i++) {
            Agent *a = &agents[i];
            for (int j = 0; j < agents.size(); j++) {
                Agent *aj = &agents[i];

                if (j == i || a -> spike_length < 0.2 || a -> w1 < 0.3 || a -> w2 < 0.3) {
                    continue;
                }

                float d = (a -> position - aj -> position).length();

                if (d < 2 * settings::BOT_RADIUS) {
                    Vector2f v(1, 0);
                    v.rotate(a -> angle);

                    float diff = v.angle_between(aj -> position - a -> position);
                    if (fabs(diff) < M_PI / 8) {
                        float mult = 1;

                        if (a -> boost) {
                            mult = settings::BOOST_SIZE_MULT;
                        }

                        float dmg = settings::SPIKE_MULT *a -> spike_length * max(fabs(a -> w1), fabs(a -> w2)) * settings::BOOST_SIZE_MULT;

                        aj -> health -= dmg;

                        if (a -> health > 2) {
                            a -> health = 2;
                        }

                        a -> spike_length = 0;
                        a -> init_event(40 * dmg, 1, 1, 0);

                        Vector2f v2(1, 0);
                        v2.rotate(aj -> angle);

                        float a_diff = v.angle_between(v2);

                        if (fabs(a_diff) < M_PI / 2) {
                            aj -> spike_length = 0;
                        }
                    }
                }
            }
        }
    }
}

void World::process_brain() {
    #pragma omp parallel for
    for (int i = 0; i < agents.size(); i++) {
        agents[i].tick();
    }
}

void World::add_random(int n) {
    for (int i = 0; i < n; i++) {
        Agent a;
        a.id = id_counter;
        id_counter++;

        agents.push_back(a);
    }
}

void World::add_crossover(int n) {
    for (int i = 0; i < n; i++) {
        int i1 = randi(0, agents.size());
        int i2 = randi(0, agents.size());

        do {
            i2 = randi(0, agents.size());
        } while (i1 == i2);

        for (int i = 0; i < agents.size(); i++) {
            Agent *a = &agents[i];

            if (a -> age > agents[i1].age && randf(0, 1) < 0.1) {
                i1 = i;

            } else if (a -> age > agents[i2].age && randf(0, 1) < 0.1) {
                i2 = i;
            }
        }

        Agent *a1 = &agents[i1];
        Agent *a2 = &agents[i2];

        Agent baby = a1 -> cross_over(*a2);

        baby.id = id_counter;
        id_counter++;

        agents.push_back(baby);
    }
}

void World::reproduce(int ai, float mr, float mr2) {
    if (randf(0, 1) < 0.04) {
        mr *= randf(1, 10);
    }

    if (randf(0, 1) < 0.04) {
        mr2 *= randf(1, 10);
    }

    agents[ai].init_event(30, 0, 0.8, 0);

    for (int i = 0; i < settings::BABIES; i++) {
        Agent a2 = agents[ai].reproduce(mr, mr2);

        a2.id = id_counter;

        id_counter++;
        agents.push_back(a2);
    }
}

void World::reset() {
    agents.clear();
    add_random(settings::NUM_BOTS);
}

void World::set_closed(bool closed) {
    closed = closed;
}

bool World::is_closed() const {
    return closed;
}
