//
// Created by Milica Kostic on 12/3/17.
//

#include <stdio.h>
#include <stdlib.h>
#include "model.h"

void load_model(const char *file_name, MODEL *output) {

    FILE *file = fopen(file_name, "r");

    VEKTOR3 v3[70000], n3[70000];
    KORD_TEKSTURE t2[70000];
    int v_count = 0, n_count = 0, t_count = 0;

    output->pozicije = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    output->tekstura = (KORD_TEKSTURE *) malloc(70000 * sizeof(KORD_TEKSTURE));
    output->normale = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    output->tacaka = 0;

    char buff[256];
    while (fgets(buff, 256, file) != NULL) {
        if (buff[0] == 'v' && buff[1] == ' ') {
            sscanf(buff, "v %f %f %f", &v3[v_count].x, &v3[v_count].y, &v3[v_count].z);
            v_count++;
        }

        if (buff[0] == 'v' && buff[1] == 'n' && buff[2] == ' ') {
            sscanf(buff, "vn %f %f %f", &n3[n_count].x, &n3[n_count].y, &n3[n_count].z);
            n_count++;
        }

        if (buff[0] == 'v' && buff[1] == 't' && buff[2] == ' ') {
            sscanf(buff, "vt %f %f", &t2[t_count].x, &t2[t_count].y);
            t_count++;
        }

        if (buff[0] == 'f' && buff[1] == ' ') {
            int v_i1, t_i1, n_i1, v_i2, t_i2, n_i2, v_i3, t_i3, n_i3;
            sscanf(buff, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v_i1, &t_i1, &n_i1, &v_i2, &t_i2, &n_i2, &v_i3, &t_i3, &n_i3);

            output->pozicije[output->tacaka] = v3[v_i1 - 1];
            output->tekstura[output->tacaka] = t2[t_i1 - 1];
            output->normale[output->tacaka] = n3[n_i1 - 1];
            output->tacaka++;

            output->pozicije[output->tacaka] = v3[v_i2 - 1];
            output->tekstura[output->tacaka] = t2[t_i2 - 1];
            output->normale[output->tacaka] = n3[n_i2 - 1];
            output->tacaka++;

            output->pozicije[output->tacaka] = v3[v_i3 - 1];
            output->tekstura[output->tacaka] = t2[t_i3 - 1];
            output->normale[output->tacaka] = n3[n_i3 - 1];
            output->tacaka++;
        }

    }

    fclose(file);
}
