#ifdef __APPLE__

#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "texture.h"
#include "model.h"


/* Dimenzije prozora */
static int window_width, window_height;

static bool key_up = false;
static bool key_down = false;
static bool key_left = false;
static bool key_right = false;
static bool moved = false;
static bool on_start = true;
static bool cam_front = false;

static float posX = 0;
static float posZ = 0;
static float angle = 0;
static float camera_angle = 180;

static float vX = 0;
static float vZ = 0;

static MODEL model;

static GLuint car_texture;
static GLuint wheel_texture;
static GLuint road_texture;
static GLuint grass_texture;
static GLuint start_texture;

static float rotX = 0;
static float rotY = 0;

static VEKTOR3 tacke_puta[30000];
static int br_tacaka_puta = 0;

static long start_time;
static long najbolji_krug = -1;

/* Deklaracije callback funkcija. */
static void on_keyboard(int key, int x, int y);

static void on_keyboard_up(int key, int x, int y);

static void on_update(int val);

static void on_reshape(int width, int height);

static void on_display(void);

static void write_text(float x, float y, const char *s);

static void generate_road();

int main(int argc, char **argv) {

    srand((unsigned int) time(NULL));

    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(on_keyboard);
    glutSpecialUpFunc(on_keyboard_up);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.52, 0.8, 0.98, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    load_model("car.model", &model);
    car_texture = loadBMP_custom("car.bmp");
    wheel_texture = loadBMP_custom("wheels.bmp");
    road_texture = loadBMP_custom("asphalt.bmp");
    grass_texture = loadBMP_custom("grass.bmp");
    start_texture = loadBMP_custom("start.bmp");

    generate_road();

    posX = (tacke_puta[br_tacaka_puta - 4].x + tacke_puta[br_tacaka_puta - 3].x) / 2;
    posZ = (tacke_puta[br_tacaka_puta - 4].z + tacke_puta[br_tacaka_puta - 3].z) / 2;

    /* Program ulazi u glavnu petlju. */
    glutTimerFunc(0, on_update, 0);
    glutMainLoop();

    return 0;
}

static void on_keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            key_left = true;
            break;
        case GLUT_KEY_RIGHT:
            key_right = true;
            break;
        case GLUT_KEY_DOWN:
            key_down = true;
            break;
        case GLUT_KEY_UP:
            key_up = true;
            if (!moved) {
                moved = true;
                start_time = time(NULL);
            }
            break;
        case 'c':
        case 'C':
            cam_front = !cam_front;
            break;
        case 27:
            exit(1);
            break;
    }

}

static void on_keyboard_up(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            key_left = false;
            break;
        case GLUT_KEY_RIGHT:
            key_right = false;
            break;
        case GLUT_KEY_DOWN:
            key_down = false;
            break;
        case GLUT_KEY_UP:
            key_up = false;
            break;
    }
}

static bool on_road() {
    float a1, b1, c1;
    int broj_preseka = 0;

    a1 = 0;
    b1 = posX - 30000;
    c1 = a1 * posX + b1 * posZ;

    for (int i = 0; i < br_tacaka_puta / 2; i++) {
        float a2, b2, c2;

        // https://www.topcoder.com/community/data-science/data-science-tutorials/geometry-concepts-line-intersection-and-its-applications/

        // unutrasnje linije
        int ind1 = (i * 2) % br_tacaka_puta;
        int ind2 = ((i + 1) * 2) % br_tacaka_puta;
        a2 = tacke_puta[ind2].z - tacke_puta[ind1].z;
        b2 = tacke_puta[ind1].x - tacke_puta[ind2].x;
        c2 = a2 * tacke_puta[ind1].x + b2 * tacke_puta[ind1].z;

        float det1 = a1 * b2 - a2 * b1;
        if (fabsf(det1) > 0.0000001f) { // ako linije nisu paralelne
            float presek_x = (b2 * c1 - b1 * c2) / det1;
            float presek_z = (a1 * c2 - a2 * c1) / det1;

            if (presek_x > posX &&
                presek_x > fminf(tacke_puta[ind1].x, tacke_puta[ind2].x) &&
                presek_x < fmaxf(tacke_puta[ind1].x, tacke_puta[ind2].x) &&
                presek_z > fminf(tacke_puta[ind1].z, tacke_puta[ind2].z) &&
                presek_z < fmaxf(tacke_puta[ind1].z, tacke_puta[ind2].z)) {
                ++broj_preseka;
            }
        }

        // spoljne linije
        ind1 = (i * 2 + 1) % br_tacaka_puta;
        ind2 = ((i + 1) * 2 + 1) % br_tacaka_puta;
        a2 = tacke_puta[ind2].z - tacke_puta[ind1].z;
        b2 = tacke_puta[ind1].x - tacke_puta[ind2].x;
        c2 = a2 * tacke_puta[ind1].x + b2 * tacke_puta[ind1].z;

        det1 = a1 * b2 - a2 * b1;
        if (fabsf(det1) > 0.0000001f) { // ako linije nisu paralelne
            float presek_x = (b2 * c1 - b1 * c2) / det1;
            float presek_z = (a1 * c2 - a2 * c1) / det1;

            if (presek_x > posX &&
                presek_x > fminf(tacke_puta[ind1].x, tacke_puta[ind2].x) &&
                presek_x < fmaxf(tacke_puta[ind1].x, tacke_puta[ind2].x) &&
                presek_z > fminf(tacke_puta[ind1].z, tacke_puta[ind2].z) &&
                presek_z < fmaxf(tacke_puta[ind1].z, tacke_puta[ind2].z)) {
                ++broj_preseka;
            }
        }
    }

    return (broj_preseka % 2 == 1);
}

/* funkcija osvezavanja cele igrice (pomeranje objekata - logika igrice) */
static void on_update(int val) {
    /*
     * Logika fizike automobila, radjeno po uzoru na:
     * https://gamedev.stackexchange.com/questions/26845/i-am-looking-to-create-realistic-car-movement-using-vectors
     */

    // promenljive ubrzanja automobila - vektor (x,z)
    float accelerationX = 0.0f, accelerationZ = 0.0f;

    // konstante fizike
    float accelerationConstant = 0.02f; // konstanta ubrzanja automobila
    float forwardFrictionContstant = accelerationConstant / 2; // koeficijent otpora u pravcu ka kome je auto okrenut
    float sideFrictionConstant = 0.6f; // koeficijent otpora po strani automobila
    float maxSpeed = 2.0f; // maksimalna brzina automobila
    float turnSpeed = 8.0f; // brzina skretanja automobila

    if (key_up) {
        // ukoliko igrac drzi strelicu napred, pravimo ubrzanje u pravcu nosa automobila
        accelerationX = sinf(angle * (float) M_PI / 180.0f) * accelerationConstant;
        accelerationZ = cosf(angle * (float) M_PI / 180.0f) * accelerationConstant;
    } else if (key_down) {
        // ukoliko igrac drzi strelicu nazad, kocenje
        forwardFrictionContstant *= 5;
    }

    if (!on_road()) {
        forwardFrictionContstant *= 4;
    }

    // trenutna vrednost brzine (skalarna)
    float speed = sqrtf(vX * vX + vZ * vZ);

    // ukoliko igrac drzi strelicu na desno ili levo, skrecemo ugao automobila u tom pravcu
    // kao modifikaciju brzini skretanja koristimo linearnu formulu speed / maxSpeed
    // ona ce efektivno biti 0 kada automobil stoji, dok ce biti maksimalna (1) pri maksimalnoj brzini
    // time postizemo da automobil ima brze skretanje sto se brze krece i takodje da ne moze skretati u mestu
    if (key_right) {
        rotY -= 0.5f; //okretanje tockova
    } else if (key_left) {
        rotY += 0.5f;
    } else {
        rotY *= 0.8; //vracanje tockova ka napred nakon skretanja
    }

    //maksimalna okrenutost tockova
    if (rotY > 45) {
        rotY = 45;
    } else if (rotY < -45) {
        rotY = -45;
    }

    angle += turnSpeed * rotY / 45.0f * speed / maxSpeed;

    // racunamo otpor u pravcu kretanja ovom jednostavnom formulom
    // prosto uzimamo brzinu auta pomnozenu sa konstantom i oduzimamo je
    float forwardFrictionX = -vX * forwardFrictionContstant;
    float forwardFrictionZ = -vZ * forwardFrictionContstant;

    // racunamo otpor na stranu automobila
    // prvo racunamo vektor desne strane automobila, ako je automobil okrenut u pravcu angle
    // onda je desna strana na angle + 90
    float rightX = sinf((angle + 90.0f) * (float) M_PI / 180.0f);
    float rightZ = cosf((angle + 90.0f) * (float) M_PI / 180.0f);

    // kada imamo vektor desne strane automobila (rightX, rightZ), skalarni proizvod strane i trenutne brzine ce nam dati
    // vrednost brzine kretanja u stranu, to cemo pomnoziti sa samim vektorom strane i konstantom otpora za stranu
    float sideFrictionX = -rightX * (vX * rightX + vZ * rightZ) * sideFrictionConstant;
    float sideFrictionZ = -rightZ * (vX * rightX + vZ * rightZ) * sideFrictionConstant;

    // na trenutnu brzinu primenjujemo otpor koji smo prethodno izracunali
    vX += forwardFrictionX + sideFrictionX;
    vZ += forwardFrictionZ + sideFrictionZ;

    // ako se automobil ne krece maksimalnom brzinom, ubrzavamo ga trenutnim ubrzanjem
    if (speed < maxSpeed) {
        vX += accelerationX;
        vZ += accelerationZ;
    }

    // pomeramo poziciju automobila za njegovu trenutnu brzinu
    posX += vX;
    posZ += vZ;

    //rotacija tockova
    rotX += speed * 100;

    /*
     * Ukoliko je cam_front false, ugao kamere prati skretanje sa malim zakasnjenje
     * a ukoliko je cam_front true, onda iz nema potrebe za animacijom pracenja automobila
     * jer se posmatra iz uloge vozaca, tako da je ugao kamere, zapravo samo ugao automobila.
     */
    if (cam_front == false)
        camera_angle = camera_angle + (angle - camera_angle) * 0.02f;
    else
        camera_angle = angle;

    float startX = (tacke_puta[0].x + tacke_puta[1].x) / 2;
    float startZ = (tacke_puta[0].z + tacke_puta[1].z) / 2;

    if (sqrtf((startX - posX) * (startX - posX) + (startZ - posZ) * (startZ - posZ)) < 35.0f) {
        if (!on_start) {
            long vreme_kruga = time(NULL) - start_time;
            if (najbolji_krug == -1 || najbolji_krug > vreme_kruga) {
                najbolji_krug = vreme_kruga;
            }
        }
        start_time = time(NULL);
        on_start = true;
    } else {
        on_start = false;
    }

    glutTimerFunc(16, on_update, 0);
    glutPostRedisplay();
}

static void on_reshape(int width, int height) {
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

static void write_text(float x, float y, const char *s) {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRasterPos2f(x, y);

    int i = 0;
    for (i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);

    }
}

/*
 * Sablon za osvetljavanje je uzet sa casa vezbi 13_lightning
 */

static void setup_lightning() {

    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = {1, 1, 1, 0};

    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1};

    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = {0.7, 0.7, 0.7, 1};

    /* Spekularna boja svetla. */
    GLfloat light_specular[] = {0.9, 0.9, 0.9, 1};

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

}

static void setup_road_material() {

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = {1, 1, 1, 1};

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = {1, 1, 1, 1};

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = {0, 0, 0, 1};

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 1;

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

static void setup_red_material() {

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = {1, 0, 0, 1};

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = {1, 0, 0, 1};

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = {0, 0, 0, 1};

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 1;

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


static void setup_car_material() {

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = {0.6f, 0.6f, 0.6f, 1};

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = {1, 1, 1, 1};

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = {1, 1, 1, 1};

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 20;

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

static void setup_glass_material() {

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = {0.2, 0.2, 0.2, 0.2};

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = {0.8, 0.8, 0.8, 0.4};

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = {1, 1, 1, 1};

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 20;

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

static void on_display(void) {
    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se viewport. */
    glViewport(0, 0, window_width, window_height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            60,
            window_width / (float) window_height,
            0.3, 500);

    /* Podesava se tacka pogleda. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*
     * Dodato je gore u kodu (u on_keyboard funkciji) da na 'c' menja bool vrednost promenljive cam_front.
     * Ukoliko je cam_front false, camera gleda iza automobila, 3rd person, a ukoliko je true, gleda iz lica vozaca.
     * Zato su prethodni koeficijenti stavljeni u promenljive, koje se menjaju ukoliko je cam_front true ili false.
     * cam_pos je distanca kamere od centra automobila, pomerenog ka sofersajbni za 0.5
     * y_view je pozicija kamere po y osi, y_look je pozicija tacke gledanja po y osi.
     * Napomena: Koeficijenti su dobijeni iskljucivo eksperimentisanjem.
     */

    float cam_pos = 6;
    float y_view = 1.5f;
    float y_look = 0.5f;
    if (cam_front == true) {
        cam_pos = 0.8f;
        y_view = 0.4f;
        y_look = 0.25f;
    }
    gluLookAt(
            (posX + sinf(angle * (float) M_PI / 180.0f) * 0.5f) - sinf(camera_angle * (float) M_PI / 180.0f) * cam_pos,
            y_view,
            (posZ + cosf(angle * (float) M_PI / 180.0f) * 0.5f) - cosf(camera_angle * (float) M_PI / 180.0f) * cam_pos,
            (posX + sinf(angle * (float) M_PI / 180.0f) * 0.5f),
            y_look,
            (posZ + cosf(angle * (float) M_PI / 180.0f) * 0.5f),
            0, 1, 0
    );

    setup_lightning();

    setup_road_material();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, grass_texture);

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0, 1, 0);
    glTexCoord2f(-50, -50);
    glVertex3f(-4000, -1.205f, -4000);

    glNormal3f(0, 1, 0);
    glTexCoord2f(-50, 50);
    glVertex3f(-4000, -1.205f, 4000);

    glNormal3f(0, 1, 0);
    glTexCoord2f(50, -50);
    glVertex3f(4000, -1.205f, -4000);

    glNormal3f(0, 1, 0);
    glTexCoord2f(50, 50);
    glVertex3f(4000, -1.205f, 4000);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, start_texture);

    glBegin(GL_TRIANGLE_STRIP);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(tacke_puta[0].x, tacke_puta[0].y + 0.005f, tacke_puta[0].z);

    glNormal3f(0, 1, 0);
    glTexCoord2f(1, 0);
    glVertex3f(tacke_puta[1].x, tacke_puta[1].y + 0.005f, tacke_puta[1].z);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 1);
    glVertex3f(tacke_puta[2].x, tacke_puta[2].y + 0.005f, tacke_puta[2].z);

    glNormal3f(0, 1, 0);
    glTexCoord2f(1, 1);
    glVertex3f(tacke_puta[3].x, tacke_puta[3].y + 0.005f, tacke_puta[3].z);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, road_texture);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < br_tacaka_puta; i++) {
        glTexCoord2f(tacke_puta[i].x * 0.25f, tacke_puta[i].z * 0.25f);
        glNormal3f(0, 1, 0);
        glVertex3f(tacke_puta[i].x, tacke_puta[i].y, tacke_puta[i].z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);
    glNormal3f(0, 1, 0);
    // Unutrasnje bele trake pored puta
    for (int i = 0; i < br_tacaka_puta / 4; i++) {
        float nX, nZ, nX1, nZ1;
        nX = tacke_puta[i * 4].x - (tacke_puta[i * 4 + 1].x - tacke_puta[i * 4].x) * 0.1f;
        nZ = tacke_puta[i * 4].z - (tacke_puta[i * 4 + 1].z - tacke_puta[i * 4].z) * 0.1f;

        nX1 = tacke_puta[i * 4 + 2].x - (tacke_puta[i * 4 + 3].x - tacke_puta[i * 4 + 2].x) * 0.1f;
        nZ1 = tacke_puta[i * 4 + 2].z - (tacke_puta[i * 4 + 3].z - tacke_puta[i * 4 + 2].z) * 0.1f;

        glVertex3f(tacke_puta[i * 4].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4].z);
        glVertex3f(tacke_puta[i * 4 + 2].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 2].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);

        glVertex3f(tacke_puta[i * 4 + 2].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 2].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);
        glVertex3f(nX1, tacke_puta[i * 4].y + 0.005f, nZ1);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3f(0, 1, 0);
    // Spoljne bele trake pored puta
    for (int i = 0; i < br_tacaka_puta / 4; i++) {
        float nX, nZ, nX1, nZ1;
        nX = tacke_puta[i * 4].x + (tacke_puta[i * 4 + 1].x - tacke_puta[i * 4].x) * 1.1f;
        nZ = tacke_puta[i * 4].z + (tacke_puta[i * 4 + 1].z - tacke_puta[i * 4].z) * 1.1f;

        nX1 = tacke_puta[i * 4 + 2].x + (tacke_puta[i * 4 + 3].x - tacke_puta[i * 4 + 2].x) * 1.1f;
        nZ1 = tacke_puta[i * 4 + 2].z + (tacke_puta[i * 4 + 3].z - tacke_puta[i * 4 + 2].z) * 1.1f;

        glVertex3f(tacke_puta[i * 4 + 1].x, tacke_puta[i * 4 + 1].y + 0.005f, tacke_puta[i * 4 + 1].z);
        glVertex3f(tacke_puta[i * 4 + 3].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 3].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);

        glVertex3f(tacke_puta[i * 4 + 3].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 3].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);
        glVertex3f(nX1, tacke_puta[i * 4].y + 0.005f, nZ1);
    }
    glEnd();

    setup_red_material();

    glBegin(GL_TRIANGLES);
    glNormal3f(0, 1, 0);
    // Unutrasnje crvene trake pored puta
    for (int i = 0; i < br_tacaka_puta / 4; i++) {
        float nX, nZ, nX1, nZ1;
        nX = tacke_puta[i * 4 + 2].x - (tacke_puta[i * 4 + 3].x - tacke_puta[i * 4 + 2].x) * 0.1f;
        nZ = tacke_puta[i * 4 + 2].z - (tacke_puta[i * 4 + 3].z - tacke_puta[i * 4 + 2].z) * 0.1f;

        nX1 = tacke_puta[i * 4 + 4].x - (tacke_puta[i * 4 + 5].x - tacke_puta[i * 4 + 4].x) * 0.1f;
        nZ1 = tacke_puta[i * 4 + 4].z - (tacke_puta[i * 4 + 5].z - tacke_puta[i * 4 + 4].z) * 0.1f;

        glVertex3f(tacke_puta[i * 4 + 2].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 2].z);
        glVertex3f(tacke_puta[i * 4 + 4].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 4].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);

        glVertex3f(tacke_puta[i * 4 + 4].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 4].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);
        glVertex3f(nX1, tacke_puta[i * 4].y + 0.005f, nZ1);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3f(0, 1, 0);
    // Spoljasnje crvene trake pored puta
    for (int i = 0; i < br_tacaka_puta / 4; i++) {
        float nX, nZ, nX1, nZ1;
        nX = tacke_puta[i * 4 + 2].x + (tacke_puta[i * 4 + 3].x - tacke_puta[i * 4 + 2].x) * 1.1f;
        nZ = tacke_puta[i * 4 + 2].z + (tacke_puta[i * 4 + 3].z - tacke_puta[i * 4 + 2].z) * 1.1f;

        nX1 = tacke_puta[i * 4 + 4].x + (tacke_puta[i * 4 + 5].x - tacke_puta[i * 4 + 4].x) * 1.1f;
        nZ1 = tacke_puta[i * 4 + 4].z + (tacke_puta[i * 4 + 5].z - tacke_puta[i * 4 + 4].z) * 1.1f;

        glVertex3f(tacke_puta[i * 4 + 3].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 3].z);
        glVertex3f(tacke_puta[i * 4 + 5].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 5].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);

        glVertex3f(tacke_puta[i * 4 + 5].x, tacke_puta[i * 4].y + 0.005f, tacke_puta[i * 4 + 5].z);
        glVertex3f(nX, tacke_puta[i * 4].y + 0.005f, nZ);
        glVertex3f(nX1, tacke_puta[i * 4].y + 0.005f, nZ1);
    }
    glEnd();

    glEnable(GL_TEXTURE_2D);

    setup_car_material();

    /*
     * Kreira se kocka i primenjuje se geometrijska transformacija na
     * istu.
     */
    glTranslatef(posX, 0, posZ);
    glRotatef(angle + 180, 0, 1, 0);

    glBindTexture(GL_TEXTURE_2D, car_texture);

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model.tacaka; ++i) {
        // prednji tockovi
        if ((i >= model.pocetak_obj[16] && i < model.pocetak_obj[17])
            || (i >= model.pocetak_obj[8] && i < model.pocetak_obj[9])) {
            continue;
        }

        // zadnji tockovi
        if ((i >= model.pocetak_obj[9] && i < model.pocetak_obj[10])
            || (i >= model.pocetak_obj[11] && i < model.pocetak_obj[12])) {
            continue;
        }

        // stakla
        if (i < model.pocetak_obj[1]) {
            continue;
        }
        glTexCoord2f(model.tekstura[i].x, model.tekstura[i].y);
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, wheel_texture);

    glPushMatrix();
    glTranslatef(0.73f, -0.26f, -1.2f);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(-rotX, 1, 0, 0);
    glTranslatef(-0.73f, 0.26f, 1.2f);


    glBegin(GL_TRIANGLES);
    for (int i = model.pocetak_obj[16]; i < model.pocetak_obj[17]; i++) {
        // prednji desni tocak
        glTexCoord2f(model.tekstura[i].x, model.tekstura[i].y);
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);

    }
    glEnd();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.73f, -0.26f, -1.2f);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(-rotX, 1, 0, 0);
    glTranslatef(0.73f, 0.26f, 1.2f);

    glBegin(GL_TRIANGLES);
    for (int i = model.pocetak_obj[8]; i < model.pocetak_obj[9]; i++) {
        // prednji levi tocak
        glTexCoord2f(model.tekstura[i].x, model.tekstura[i].y);
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);

    }
    glEnd();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.73f, -0.26f, 1.14f);
    glRotatef(-rotX, 1, 0, 0);
    glTranslatef(-0.73f, 0.26f, -1.14f);

    glBegin(GL_TRIANGLES);
    for (int i = model.pocetak_obj[9]; i < model.pocetak_obj[10]; i++) {
        // zadnji desni tocak
        glTexCoord2f(model.tekstura[i].x, model.tekstura[i].y);
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);

    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.73f, -0.26f, 1.14f);
    glRotatef(-rotX, 1, 0, 0);
    glTranslatef(0.73f, 0.26f, -1.14f);

    glBegin(GL_TRIANGLES);
    for (int i = model.pocetak_obj[11]; i < model.pocetak_obj[12]; i++) {
        // zadnji levi tocak
        glTexCoord2f(model.tekstura[i].x, model.tekstura[i].y);
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);

    }
    glEnd();
    glPopMatrix();

    /*
     * ukljucivanje transparencije stakla, primer za transparentnost:
     * https://stackoverflow.com/questions/3125017/how-to-draw-transparent-polygon-in-opengl
     */
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

    glDisable(GL_TEXTURE_2D);

    setup_glass_material();

    glBegin(GL_TRIANGLES);
    glColor4f(1, 1, 1, 1);
    for (int i = 0; i < model.pocetak_obj[1]; ++i) {
        glNormal3f(model.normale[i].x, model.normale[i].y, model.normale[i].z);
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);
    }

    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    //racuna se trenutna brzina: sqrt(vX^2 + vZ^2)

    int speed = (int) (sqrtf(vX * vX + vZ * vZ) * 50 * 3 / 2);

    char buff[200];
    sprintf(buff, "Speed: %d km/h", speed);

    glColor3f(0, 0, 0);
    write_text(0, 0.9, buff);

    sprintf(buff, "Lap time: %d sec", time(NULL) - start_time);
    if (moved) {
        write_text(-0.9f, 0.9f, buff);
    }

    if (najbolji_krug != -1) {
        sprintf(buff, "Best lap: %d sec", najbolji_krug);
        write_text(-0.9f, 0.85f, buff);
    }

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

//generise random broj i mapira ga u granice min i max
static int random_number(int min, int max) {
    return min + rand() % (max - min);
}

static float random__float_number(float min, float max) {
    return min + (float) (rand()) / (float) (RAND_MAX) * (max - min);
}

typedef struct {
    float x, y;
    float u, v; //vektor tangenti
} TACKA;

static VEKTOR3 hermitova(TACKA p, TACKA q, float t) {
    VEKTOR3 rezultat;

    rezultat.y = -1.2f;
    rezultat.x = (2 * t * t * t - 3 * t * t + 1) * p.x + (t * t * t - 2 * t * t + t) * p.u +
                 (-2 * t * t * t + 3 * t * t) * q.x + (t * t * t - t * t) * q.u;
    rezultat.z = (2 * t * t * t - 3 * t * t + 1) * p.y + (t * t * t - 2 * t * t + t) * p.v +
                 (-2 * t * t * t + 3 * t * t) * q.y + (t * t * t - t * t) * q.v;

    return rezultat;
}

static void generate_road() {

    TACKA niz_tacaka[300];

    TACKA niz_spoljnih_tacaka[300];
    /*
     * za crtanje kruga koristimo polarne koordinate x=qcosF i y=qsinF
     * pri cemu ce q random da se generise kako bi se dobio nepravilan krug
     * i ugao se random generise kako bismo dobili nepravilna rastojanja izmedju tacaka kruga
     */
    int i = 0;
    float ugao = 0;
    while (ugao < 360) {
        float q = random__float_number(16 * 100, 30 * 100);
        niz_tacaka[i].x = q * cosf(ugao * M_PI / 180.0f);
        niz_tacaka[i].y = q * sinf(ugao * M_PI / 180.0f);

        niz_spoljnih_tacaka[i].x = (q + 30) * cosf(ugao * M_PI / 180.0f);
        niz_spoljnih_tacaka[i].y = (q + 30) * sinf(ugao * M_PI / 180.0f);
        ugao += random_number(10, 20);

        i++;

    }

    /*
     * formula generisanja vektora uzeta iz profesorkine skripte sa predavanja (Catmull-Rom)
     */

    int j = 0;
    for (j = 0; j < i; j++) {
        niz_tacaka[j].u = (niz_tacaka[(j + 1) % i].x - niz_tacaka[(i + j - 1) % i].x) / 3.0f;
        niz_tacaka[j].v = (niz_tacaka[(j + 1) % i].y - niz_tacaka[(i + j - 1) % i].y) / 3.0f;

        niz_spoljnih_tacaka[j].u = (niz_tacaka[(j + 1) % i].x - niz_tacaka[(i + j - 1) % i].x) / 3.0f;
        niz_spoljnih_tacaka[j].v = (niz_tacaka[(j + 1) % i].y - niz_tacaka[(i + j - 1) % i].y) / 3.0f;
    }

    for (j = 0; j < i; j++) {
        float t = 0;
        while (t <= 1) {
            tacke_puta[br_tacaka_puta] = hermitova(niz_tacaka[j], niz_tacaka[(j + 1) % i], t);
            br_tacaka_puta++;
            tacke_puta[br_tacaka_puta] = hermitova(niz_spoljnih_tacaka[j], niz_spoljnih_tacaka[(j + 1) % i], t);
            br_tacaka_puta++;
            t += 0.05;

        }
    }

    tacke_puta[br_tacaka_puta] = tacke_puta[0];
    br_tacaka_puta++;
    tacke_puta[br_tacaka_puta] = tacke_puta[1];
    br_tacaka_puta++;
}
