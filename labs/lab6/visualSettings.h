#pragma once

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
// #define GLEW_STATIC

enum cubeColors {
    yellow = 0xFFD700,
    blue = 0x0000FF,
    green = 0x32CD32,
    red = 0xFF0000,
    orange = 0xFF8C00,
    white = 0xFFFFFF
};

// цвета
unsigned int colors[6] = {green, blue, yellow, white, orange, red};

const int ROTATE_SPEED_STEP = 10, ROTATE_START_VALUE = 30;
GLfloat CUBE_SIZE = 12;

// проекции угла поворота на оси
int xRot = 25, yRot = -45;

// отдаление
GLfloat translateZ = -35;
