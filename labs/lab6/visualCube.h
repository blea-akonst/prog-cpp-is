#pragma once

#include "visualSettings.h"

class VisualCube {
private:
    unsigned char RGBColor[3]; // glColor, glVertex и тд принмимают char*

public:
    // шесть граней куба - шесть цветов
    int color[6];
    // размер ребра
    GLfloat visualSize;

    void rotateORb() {
        int tmp = color[5];
        color[5] = color[3];
        color[3] = color[4];
        color[4] = color[2];
        color[2] = tmp;
    }

    void rotateRR() {
        int tmp = color[5];
        color[5] = color[3];
        color[3] = color[4];
        color[4] = color[2];
        color[2] = tmp;
    }

    void rotateGR() {
        int tmp = color[2];
        color[2] = color[1];
        color[1] = color[3];
        color[3] = color[0];
        color[0] = tmp;
    }

    void rotateGD() {
        int tmp = color[0];
        color[0] = color[4];
        color[4] = color[1];
        color[1] = color[5];
        color[5] = tmp;
    }

    void rotateGU() {
        int tmp = color[0];
        color[0] = color[4];
        color[4] = color[1];
        color[1] = color[5];
        color[5] = tmp;
    }

    void setColor(int i, int color) { this->color[i] = color; }

    // перевод из 0xFF0000 в RGB
    unsigned char *getRGB(int i) {
        // разбиваем color[i] на 3 составляющих
        // для 0xFF0000 RGB(FF, 00, 00) - красный цвет;
        RGBColor[0] = color[i] >> 16;
        RGBColor[1] = color[i] >> 8;
        RGBColor[2] = color[i];
        return RGBColor;
    }

    // отрисовка куба:
    // устанавливаем цвет и нормали
    void draw() {
        glPushMatrix();
        glBegin(GL_QUADS);

        // верх
        glColor3ubv(getRGB(0));
        glNormal3f(0, 0, 1);
        glVertex3f(visualSize, visualSize, visualSize);
        glVertex3f(0, visualSize, visualSize);
        glVertex3f(0, 0, visualSize);
        glVertex3f(visualSize, 0, visualSize);

        // низ
        glColor3ubv(getRGB(1));
        glNormal3f(0, 0, -1);
        glVertex3f(visualSize, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, visualSize, 0);
        glVertex3f(visualSize, visualSize, 0);

        // спереди
        glColor3ubv(getRGB(2));
        glNormal3f(0, -1, 0);
        glVertex3f(visualSize, 0, visualSize);
        glVertex3f(0, 0, visualSize);
        glVertex3f(0, 0, 0);
        glVertex3f(visualSize, 0, 0);

        // сзади
        glColor3ubv(getRGB(3));
        glNormal3f(0, 1, 0);
        glVertex3f(visualSize, visualSize, 0);
        glVertex3f(0, visualSize, 0);
        glVertex3f(0, visualSize, visualSize);
        glVertex3f(visualSize, visualSize, visualSize);

        // слева
        glColor3ubv(getRGB(4));
        glNormal3f(-1, 0, 0);
        glVertex3f(0, visualSize, visualSize);
        glVertex3f(0, visualSize, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, visualSize);

        // справа
        glColor3ubv(getRGB(5));
        glNormal3f(1, 0, 0);
        glVertex3f(visualSize, visualSize, 0);
        glVertex3f(visualSize, visualSize, visualSize);
        glVertex3f(visualSize, 0, visualSize);
        glVertex3f(visualSize, 0, 0);

        glEnd();
        glPopMatrix();
    }

    // отрисовка куба со смещением (x, y, z)
    void draw(double x, double y, double z) {
        glPushMatrix();
        glTranslated(x, y, z);
        draw();
        glPopMatrix();
    }

};
