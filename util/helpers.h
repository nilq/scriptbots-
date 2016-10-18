#ifndef HELPERS
#define HELPERS

#include <stdlib.h>
#include <math.h>

inline float randf(float a, float b) {
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

inline float randi(int a, int b) {
    return (rand() % (b - a)) + a;
}

inline double randn(double mu, double sigma) {
    static bool shall_deviate   = false;
    static float stored_deviate;

    double polar, r_squared, v1, v2;

    if (shall_deviate) {
        shall_deviate = false;

        return stored_deviate * sigma + mu;

    } else {
        do {
            v1 = 2.0 * (double(rand()) / double(RAND_MAX));
            v2 = 2.0 * (double(rand()) / double(RAND_MAX));

            r_squared = v1 * v1 + v2 * v2;

        } while(r_squared >= 1.0 || r_squared == 0);

        polar = sqrt(-2.0 * log(r_squared) / r_squared)

        stored_deviate = v1 * polar;
        shall_deviate  = true;

        return v1 * polar * sigma + mu;
    }
}

inline float clamp(float a) {
    if (a < 0) {
        return 0;

    } else if (a > 1) {
        return 1;
    }

    return a;
}

#endif
