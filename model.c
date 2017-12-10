//
// Created by Milica Kostic on 12/3/17.
//


#include <stdio.h>
#include <stdlib.h>
#include "model.h"


/*
 * informacije kako ucitati .obj fajl su nadjene na linku
 * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
 * Po uzoru na to kakav je format fajla sam napisala funkcuju load_model za ucitavanje modela
 */
void load_model(const char *file_name, MODEL *output) {

    FILE *file = fopen(file_name, "r");

    VEKTOR3 *v3, *n3;
    KORD_TEKSTURE *t2;
    int v_count = 0, n_count = 0, t_count = 0;

    v3 = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    n3 = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    t2 = (KORD_TEKSTURE *) malloc(70000 * sizeof(KORD_TEKSTURE));

    output->pozicije = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    output->tekstura = (KORD_TEKSTURE *) malloc(70000 * sizeof(KORD_TEKSTURE));
    output->normale = (VEKTOR3 *) malloc(70000 * sizeof(VEKTOR3));
    output->tacaka = 0;
    output->pocetak_obj = (int *) malloc(50 * sizeof(int));
    output->br_obj = 0;

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

        if (buff[0] == 'o' && buff[1] == ' ') {
            output->pocetak_obj[output->br_obj++] = output->tacaka;
        }

    }

    free(v3);
    free(n3);
    free(t2);

    fclose(file);
}
