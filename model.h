//
// Created by Milica Kostic on 12/3/17.
//

#ifndef SPEEDRACING_MODEL_H
#define SPEEDRACING_MODEL_H

typedef struct {
    float x, y, z;
} VEKTOR3;

typedef struct {
    float x, y;
} KORD_TEKSTURE;

typedef struct {
    VEKTOR3 *pozicije;
    VEKTOR3 *normale;
    KORD_TEKSTURE *tekstura;
    int tacaka;
    int *pocetak_obj;
    int br_obj;
} MODEL;

void load_model(const char *file_name, MODEL *output);

#endif //SPEEDRACING_MODEL_H

