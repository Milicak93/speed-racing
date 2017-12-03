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
#include "model.h"


/* Dimenzije prozora */
static int window_width, window_height;

static bool key_up = false;
static bool key_down = false;
static bool key_left = false;
static bool key_right = false;

static float posX = 0;
static float posZ = 0;
static float angle = 0;

static float vX = 0;
static float vZ = 0;

static MODEL model;


/* Deklaracije callback funkcija. */
static void on_keyboard(int key, int x, int y);

static void on_keyboard_up(int key, int x, int y);

static void on_update(int val);

static void on_reshape(int width, int height);

static void on_display(void);

static void write_text(float x, float y, const char *s);

int main(int argc, char **argv) {
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(on_keyboard);
    glutSpecialUpFunc(on_keyboard_up);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    load_model("car.obj", &model);

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

/* funkcija osvezavanja cele igrice (pomeranje objekata - logika igrice) */
static void on_update(int val) {
    /*
     * Logika fizike automobila, radjeno po uzoru na:
     * https://gamedev.stackexchange.com/questions/26845/i-am-looking-to-create-realistic-car-movement-using-vectors
     */

    // promenljive ubrzanja automobila - vektor (x,z)
    float accelerationX = 0.0f, accelerationZ = 0.0f;

    // konstante fizike
    float accelerationConstant = 0.04f; // konstanta ubrzanja automobila
    float forwardFrictionContstant = accelerationConstant / 3; // koeficijent otpora u pravcu ka kome je auto okrenut
    float sideFrictionConstant = 0.6f; // koeficijent otpora po strani automobila
    float maxSpeed = 3.0f; // maksimalna brzina automobila
    float turnSpeed = 30.0f; // brzina skretanja automobila

    if (key_up) {
        // ukoliko igrac drzi strelicu napred, pravimo ubrzanje u pravcu nosa automobila
        accelerationX = sinf(angle * (float) M_PI / 180.0f) * accelerationConstant;
        accelerationZ = cosf(angle * (float) M_PI / 180.0f) * accelerationConstant;
    } else if (key_down) {
        // ukoliko igrac drzi strelicu nazad, pravimo ubrzanje u suprotnom pravcu od nosa automobila
        accelerationX = sinf(angle * (float) M_PI / 180.0f) * accelerationConstant;
        accelerationZ = cosf(angle * (float) M_PI / 180.0f) * accelerationConstant;
    }

    // trenutna vrednost brzine (skalarna)
    float speed = sqrtf(vX * vX + vZ * vZ);

    // ukoliko igrac drzi strelicu na desno ili levo, skrecemo ugao automobila u tom pravcu
    // kao modifikaciju brzini skretanja koristimo linearnu formulu speed / maxSpeed
    // ona ce efektivno biti 0 kada automobil stoji, dok ce biti maksimalna (1) pri maksimalnoj brzini
    // time postizemo da automobil ima brze skretanje sto se brze krece i takodje da ne moze skretati u mestu
    if (key_right) {
        angle -= turnSpeed * speed / maxSpeed;
    } else if (key_left) {
        angle += turnSpeed * speed / maxSpeed;
    }

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
            1, 500);

    /* Podesava se tacka pogleda. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            0, 10, -0.1,
            0, 0, 0,
            0, 1, 0
    );

    /*
     * Kreira se kocka i primenjuje se geometrijska transformacija na
     * istu.
     */
    glTranslatef(posX, .5, posZ);
    glRotatef(angle + 180, 0, 1, 0);
    glScalef(1, 1, 1);

    glBegin(GL_TRIANGLES);
    glColor3f(0, 0, 1);
    for (int i = 0; i < model.tacaka; ++i) {
        glVertex3f(model.pozicije[i].x, model.pozicije[i].y, model.pozicije[i].z);
    }
    glEnd();

    //racuna se trenutna brzina: sqrt(vX^2 + vZ^2)

    int speed = (int) (sqrtf(vX * vX + vZ * vZ) * 50);

    char buff[200];
    sprintf(buff, "Speed: %d ", speed);

    write_text(0, 0.9, buff);


    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
