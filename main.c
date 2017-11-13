#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#ifdef __APPLE__

#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif

/* Dimenzije prozora */
static int window_width, window_height;

static bool key_up = false;
static bool key_down = false;
static bool key_left = false;
static bool key_right = false;

static float pom = 0;
static float down = 0;
static float angle = 0;

/* Deklaracije callback funkcija. */
static void on_keyboard(int key, int x, int y);

static void on_keyboard_up(int key, int x, int y);

static void on_update(int val);

static void on_reshape(int width, int height);

static void on_display(void);

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

/* funkcija osvezavanja igrice (pomeranje objekata - logika igrice) */
static void on_update(int val) {
    if (key_up) {
        pom += sinf(angle * (float) M_PI / 180.0f) * 0.1;
        down += cosf(angle * (float) M_PI / 180.0f) * 0.1;
    } else if (key_down) {
        pom -= sinf(angle * (float) M_PI / 180.0f) * 0.1;
        down -= cosf(angle * (float) M_PI / 180.0f) * 0.1;
    }
    if (key_right) {
        angle -= 10;
    } else if (key_left) {
        angle += 10;
    }
    glutTimerFunc(16, on_update, 0);
    glutPostRedisplay();
}

static void on_reshape(int width, int height) {
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
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
            0, 5, -0.1,
            0, 0, 0,
            0, 1, 0
    );

    /*
     * Kreira se kocka i primenjuje se geometrijska transformacija na
     * istu.
     */
    glColor3f(0, 0, 1);
    glTranslatef(pom, .5, down);
    glRotatef(angle, 0, 1, 0);
    glScalef(1, 1, 1);
    glutSolidCube(1);

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
