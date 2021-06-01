#pragma once

#include "visualCube.h"

void processMenu(int action);

// меню с кнопками для действий
void createGLUTMenus() {
    glutCreateMenu(processMenu);

    glutAddMenuEntry("Read Cube from file", 1);
    glutAddMenuEntry("Print Cube in console", 2);
    glutAddMenuEntry("Print Cube in file", 3);
    glutAddMenuEntry("Shuffle", 4);
    glutAddMenuEntry("Find Solution", 5);
    glutAddMenuEntry("Print solving information in console", 6);
    glutAddMenuEntry("Solve Cube using only left and up rotates", 7);
    glutAddMenuEntry("Solve Cube using only right and up rotates", 8);
    glutAddMenuEntry("Create solved Cube", 9);
    glutAddMenuEntry("Start Cube-solver-MachineGun", 10);
    // glutAddMenuEntry("Set random colors", 11);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat fAspect = (GLfloat) width / (GLfloat) height;
    gluPerspective(60, fAspect, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
