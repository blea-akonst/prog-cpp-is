#include <iostream>
#include <vector>
#include <unistd.h> // для sleep и flush
#include "additional.h"
#include "visualSettings.h"
#include "visualCube.h"
#include "visualFunctions.h"

using namespace std;

#define Plane vector<vector<MiniCube*>>
#define RightCenter RightPlane[1][1]->RightColor()
#define LeftCenter LeftPlane[1][1]->LeftColor()
#define UpCenter UpPlane[1][1]->UpColor()
#define DownCenter DownPlane[1][1]->DownColor()
#define FrontCenter FrontPlane[1][1]->FrontColor()
#define BackCenter BackPlane[1][1]->BackColor()

ifstream inputStream("input.txt");
ofstream outputStream("output.txt");

static int CubeCounter = 0, AmountOfTests = 10000;
static vector<string> solvingErrors;

class RubikCube {
private:

    // Кубик, из которого состоит Кубик Рубика
    class MiniCube {
    private:
        // Грани кубика. Видимые грани имеют цвет, а невидимые - чёрные
        string up = "black", down = "black", left = "black", right = "black", front = "black", back = "black";
    public:
        // Конструкторы
        MiniCube() = default;

        explicit MiniCube(string &up, string &down, string &left, string &right, string &front, string &back) :
                up(up), down(down), left(left), right(right), front(front), back(back) {}

        // Геттеры
        [[nodiscard]] string UpColor() const { return this->up; }

        [[nodiscard]] string DownColor() const { return this->down; }

        [[nodiscard]] string LeftColor() const { return this->left; }

        [[nodiscard]] string RightColor() const { return this->right; }

        [[nodiscard]] string FrontColor() const { return this->front; }

        [[nodiscard]] string BackColor() const { return this->back; }

        // Сеттеры
        void setUpColor(string color) { this->up = move(color); }

        void setDownColor(string color) { this->down = move(color); }

        void setLeftColor(string color) { this->left = move(color); }

        void setRightColor(string color) { this->right = move(color); }

        void setFrontColor(string color) { this->front = move(color); }

        void setBackColor(string color) { this->back = move(color); }
    };

    unsigned int FirstStepCounter = 0, SecondStepCounter = 0, ThirdStepCounter = 0,
            FourthStepCounter = 0, FifthStepCounter = 0, SixthStepCounter = 0, SeventhStepCounter = 0;

    unsigned int RotatesCounter = 0;

    // В arr мы храним "слои" нашего кубика, в каждом из которых есть 3 вектора по 3 мини-кубика
    vector<vector<vector<MiniCube>>> arr;

    // Грани кубика
    Plane UpPlane;
    Plane DownPlane;
    Plane LeftPlane;
    Plane RightPlane;
    Plane FrontPlane;
    Plane BackPlane;

    void FillPlaneArr(Plane &curPlane) {
        for (int i = 0; i < 3; ++i) {
            vector<MiniCube *> tempArr(3);
            for (int j = 0; j < 3; ++j)
                tempArr[j] = &arr[i][2][j];
            curPlane.push_back(tempArr);
        }
    }

    // Распределение кубиков из трёхмерного вектора по граням
    void PushInPlaneVector() {
        FillPlaneArr(UpPlane);
        FillPlaneArr(DownPlane);
        FillPlaneArr(LeftPlane);
        FillPlaneArr(RightPlane);
        FillPlaneArr(FrontPlane);
        FillPlaneArr(BackPlane);
    }


    /* для визуализации */
    bool isVisualCubeUsed[3][3][3];
    // 27 частей
    VisualCube tmp[3][3], visualColors[3][3][3];
    // храним угол поворота каждой грани
    int rotateAngle[6];
    // размер кубика-рубика
    GLfloat visualSize;

public:

    unsigned int getRotatesCounter() { return this->RotatesCounter; }

    [[nodiscard]] bool CheckIfCubeCorrect() const {
        bool ifUpAndDownCentersCorrect =
                ((UpCenter == "W" || UpCenter == "white") && (DownCenter == "Y" || DownCenter == "yellow")) ||
                ((UpCenter == "Y" || UpCenter == "yellow") && (DownCenter == "W" || DownCenter == "white"));

        bool ifOtherCentersCorrect =
                ((LeftCenter == "O" || LeftCenter == "orange") && (FrontCenter == "G" || FrontCenter == "green") &&
                (RightCenter == "R" || RightCenter == "red") && (BackCenter == "B" || BackCenter == "blue")) ||
                ((LeftCenter == "B" || LeftCenter == "blue") && (FrontCenter == "O" || FrontCenter == "orange") &&
                (RightCenter == "G" || RightCenter == "green") && (BackCenter == "R" || BackCenter == "red")) ||
                ((LeftCenter == "R" || LeftCenter == "red") && (FrontCenter == "B" || FrontCenter == "blue") &&
                (RightCenter == "O" || RightCenter == "orange") && (BackCenter == "G" || BackCenter == "green")) ||
                ((LeftCenter == "G" || LeftCenter == "green") && (FrontCenter == "R" || FrontCenter == "red") &&
                (RightCenter == "B" || RightCenter == "blue") && (BackCenter == "O" || BackCenter == "orange"));

        bool result = ifUpAndDownCentersCorrect && ifOtherCentersCorrect;

        return result;
    }

    // Конструктор
    RubikCube() {
        arr.resize(3, vector<vector<MiniCube >>(3, vector<MiniCube>(3)));
        PushInPlaneVector();
        CreateRubikCube();
    }

    ~RubikCube() = default;

    // Вывод статистики по сборке кубика
    void PrintCubeInfo(ostream &outputTemp = cout) {
        if (CheckIfCubeCorrect() && FifthStepCounter != 0) {
            outputTemp << "\n\n";
            outputTemp << "+---------------------------------------------------+\n";
            outputTemp << "|           Количество итераций по шагам:           |\n";
            outputTemp << "+---------------------------------------------------+\n\n";
            outputTemp << "1й шаг) " << FirstStepCounter << " итераций" << endl;
            outputTemp << "2й шаг) " << SecondStepCounter << " итераций" << endl;
            outputTemp << "3й шаг) " << ThirdStepCounter << " итераций" << endl;
            outputTemp << "4й шаг) " << FourthStepCounter << " итераций" << endl;
            outputTemp << "5й шаг) " << FifthStepCounter << " итераций" << endl;
            outputTemp << "6й шаг) " << SixthStepCounter << " итераций" << endl;
            outputTemp << "7й шаг) " << SeventhStepCounter << " итераций" << endl;
            (isCubeCompleted()) ?
                    outputTemp << "\n\nВсего поворотов потребовалось для решения кубика: " << RotatesCounter << "\n\n" :
                    outputTemp << "\n\nСделано " << RotatesCounter << " поворотов, но кубик не собран\n\n";
        } else
            (isCubeCompleted()) ?
                outputTemp << "\n\nВсего поворотов потребовалось для решения кубика: " << RotatesCounter << "\n\n" :
                outputTemp << "\n\nСделано " << RotatesCounter << " поворотов, но кубик не собран\n\n";
    }

    // Функция для расскраски граней изначальными цветами
    void CreateRubikCube() {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                UpPlane[i][j]->setUpColor("W");
                DownPlane[i][j]->setDownColor("Y");
                LeftPlane[i][j]->setLeftColor("O");
                RightPlane[i][j]->setRightColor("R");
                FrontPlane[i][j]->setFrontColor("G");
                BackPlane[i][j]->setBackColor("B");
            }
    }

    // Повороты
    void RotateUpPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string right_0_0 = RightPlane[0][0]->RightColor(),
                right_0_1 = RightPlane[0][1]->RightColor(), right_0_2 = RightPlane[0][2]->RightColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "U ";

            string up_0_0 = UpPlane[0][0]->UpColor(),
                    up_1_0 = UpPlane[1][0]->UpColor();

            UpPlane[0][0]->setUpColor(UpPlane[2][0]->UpColor());
            UpPlane[1][0]->setUpColor(UpPlane[2][1]->UpColor());
            UpPlane[2][0]->setUpColor(UpPlane[2][2]->UpColor());
            UpPlane[2][1]->setUpColor(UpPlane[1][2]->UpColor());
            UpPlane[2][2]->setUpColor(UpPlane[0][2]->UpColor());
            UpPlane[1][2]->setUpColor(UpPlane[0][1]->UpColor());
            UpPlane[0][2]->setUpColor(up_0_0);
            UpPlane[0][1]->setUpColor(up_1_0);

            RightPlane[0][0]->setRightColor(BackPlane[0][0]->BackColor());
            RightPlane[0][1]->setRightColor(BackPlane[0][1]->BackColor());
            RightPlane[0][2]->setRightColor(BackPlane[0][2]->BackColor());

            BackPlane[0][0]->setBackColor(LeftPlane[0][0]->LeftColor());
            BackPlane[0][1]->setBackColor(LeftPlane[0][1]->LeftColor());
            BackPlane[0][2]->setBackColor(LeftPlane[0][2]->LeftColor());

            LeftPlane[0][0]->setLeftColor(FrontPlane[0][0]->FrontColor());
            LeftPlane[0][1]->setLeftColor(FrontPlane[0][1]->FrontColor());
            LeftPlane[0][2]->setLeftColor(FrontPlane[0][2]->FrontColor());

            FrontPlane[0][0]->setFrontColor(right_0_0);
            FrontPlane[0][1]->setFrontColor(right_0_1);
            FrontPlane[0][2]->setFrontColor(right_0_2);

            visualRotateMiniMachineGun(3, ROTATE_START_VALUE, -1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "U' ";

            string up_0_2 = UpPlane[0][2]->UpColor(),
                    up_1_2 = UpPlane[1][2]->UpColor();

            UpPlane[0][2]->setUpColor(UpPlane[2][2]->UpColor());
            UpPlane[1][2]->setUpColor(UpPlane[2][1]->UpColor());
            UpPlane[2][2]->setUpColor(UpPlane[2][0]->UpColor());
            UpPlane[2][1]->setUpColor(UpPlane[1][0]->UpColor());
            UpPlane[2][0]->setUpColor(UpPlane[0][0]->UpColor());
            UpPlane[1][0]->setUpColor(UpPlane[0][1]->UpColor());
            UpPlane[0][1]->setUpColor(up_1_2);
            UpPlane[0][0]->setUpColor(up_0_2);

            RightPlane[0][0]->setRightColor(FrontPlane[0][0]->FrontColor());
            RightPlane[0][1]->setRightColor(FrontPlane[0][1]->FrontColor());
            RightPlane[0][2]->setRightColor(FrontPlane[0][2]->FrontColor());

            FrontPlane[0][0]->setFrontColor(LeftPlane[0][0]->LeftColor());
            FrontPlane[0][1]->setFrontColor(LeftPlane[0][1]->LeftColor());
            FrontPlane[0][2]->setFrontColor(LeftPlane[0][2]->LeftColor());

            LeftPlane[0][0]->setLeftColor(BackPlane[0][0]->BackColor());
            LeftPlane[0][1]->setLeftColor(BackPlane[0][1]->BackColor());
            LeftPlane[0][2]->setLeftColor(BackPlane[0][2]->BackColor());

            BackPlane[0][0]->setBackColor(right_0_0);
            BackPlane[0][1]->setBackColor(right_0_1);
            BackPlane[0][2]->setBackColor(right_0_2);

            visualRotateMiniMachineGun(3, ROTATE_SPEED_STEP, 1);
        }
    }

    void RotateDownPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string right_2_0 = RightPlane[2][0]->RightColor(),
                right_2_1 = RightPlane[2][1]->RightColor(), right_2_2 = RightPlane[2][2]->RightColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "D ";

            string down_0_0 = DownPlane[0][0]->DownColor(),
                    down_1_0 = DownPlane[1][0]->DownColor();

            DownPlane[0][0]->setDownColor(DownPlane[2][0]->DownColor());
            DownPlane[1][0]->setDownColor(DownPlane[2][1]->DownColor());
            DownPlane[2][0]->setDownColor(DownPlane[2][2]->DownColor());
            DownPlane[2][1]->setDownColor(DownPlane[1][2]->DownColor());
            DownPlane[2][2]->setDownColor(DownPlane[0][2]->DownColor());
            DownPlane[1][2]->setDownColor(DownPlane[0][1]->DownColor());
            DownPlane[0][2]->setDownColor(down_0_0);
            DownPlane[0][1]->setDownColor(down_1_0);

            RightPlane[2][0]->setRightColor(FrontPlane[2][0]->FrontColor());
            RightPlane[2][1]->setRightColor(FrontPlane[2][1]->FrontColor());
            RightPlane[2][2]->setRightColor(FrontPlane[2][2]->FrontColor());

            FrontPlane[2][0]->setFrontColor(LeftPlane[2][0]->LeftColor());
            FrontPlane[2][1]->setFrontColor(LeftPlane[2][1]->LeftColor());
            FrontPlane[2][2]->setFrontColor(LeftPlane[2][2]->LeftColor());

            LeftPlane[2][0]->setLeftColor(BackPlane[2][0]->BackColor());
            LeftPlane[2][1]->setLeftColor(BackPlane[2][1]->BackColor());
            LeftPlane[2][2]->setLeftColor(BackPlane[2][2]->BackColor());

            BackPlane[2][0]->setBackColor(right_2_0);
            BackPlane[2][1]->setBackColor(right_2_1);
            BackPlane[2][2]->setBackColor(right_2_2);

            visualRotateMiniMachineGun(2, ROTATE_SPEED_STEP, 1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "D' ";

            string down_0_2 = DownPlane[0][2]->DownColor(),
                    down_1_2 = DownPlane[1][2]->DownColor();

            DownPlane[0][2]->setDownColor(DownPlane[2][2]->DownColor());
            DownPlane[1][2]->setDownColor(DownPlane[2][1]->DownColor());
            DownPlane[2][2]->setDownColor(DownPlane[2][0]->DownColor());
            DownPlane[2][1]->setDownColor(DownPlane[1][0]->DownColor());
            DownPlane[2][0]->setDownColor(DownPlane[0][0]->DownColor());
            DownPlane[1][0]->setDownColor(DownPlane[0][1]->DownColor());
            DownPlane[0][1]->setDownColor(down_1_2);
            DownPlane[0][0]->setDownColor(down_0_2);

            RightPlane[2][0]->setRightColor(BackPlane[2][0]->BackColor());
            RightPlane[2][1]->setRightColor(BackPlane[2][1]->BackColor());
            RightPlane[2][2]->setRightColor(BackPlane[2][2]->BackColor());

            BackPlane[2][0]->setBackColor(LeftPlane[2][0]->LeftColor());
            BackPlane[2][1]->setBackColor(LeftPlane[2][1]->LeftColor());
            BackPlane[2][2]->setBackColor(LeftPlane[2][2]->LeftColor());

            LeftPlane[2][0]->setLeftColor(FrontPlane[2][0]->FrontColor());
            LeftPlane[2][1]->setLeftColor(FrontPlane[2][1]->FrontColor());
            LeftPlane[2][2]->setLeftColor(FrontPlane[2][2]->FrontColor());

            FrontPlane[2][0]->setFrontColor(right_2_0);
            FrontPlane[2][1]->setFrontColor(right_2_1);
            FrontPlane[2][2]->setFrontColor(right_2_2);

            visualRotateMiniMachineGun(2, ROTATE_SPEED_STEP, -1);
        }
    }

    void RotateLeftPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string front_0_0 = FrontPlane[0][0]->FrontColor(),
                front_1_0 = FrontPlane[1][0]->FrontColor(), front_2_0 = FrontPlane[2][0]->FrontColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "L ";

            string left_0_0 = LeftPlane[0][0]->LeftColor(),
                    left_1_0 = LeftPlane[1][0]->LeftColor();

            LeftPlane[0][0]->setLeftColor(LeftPlane[2][0]->LeftColor());
            LeftPlane[1][0]->setLeftColor(LeftPlane[2][1]->LeftColor());
            LeftPlane[2][0]->setLeftColor(LeftPlane[2][2]->LeftColor());
            LeftPlane[2][1]->setLeftColor(LeftPlane[1][2]->LeftColor());
            LeftPlane[2][2]->setLeftColor(LeftPlane[0][2]->LeftColor());
            LeftPlane[1][2]->setLeftColor(LeftPlane[0][1]->LeftColor());
            LeftPlane[0][2]->setLeftColor(left_0_0);
            LeftPlane[0][1]->setLeftColor(left_1_0);

            FrontPlane[0][0]->setFrontColor(UpPlane[0][0]->UpColor());
            FrontPlane[1][0]->setFrontColor(UpPlane[1][0]->UpColor());
            FrontPlane[2][0]->setFrontColor(UpPlane[2][0]->UpColor());

            UpPlane[0][0]->setUpColor(BackPlane[2][2]->BackColor());
            UpPlane[1][0]->setUpColor(BackPlane[1][2]->BackColor());
            UpPlane[2][0]->setUpColor(BackPlane[0][2]->BackColor());

            BackPlane[0][2]->setBackColor(DownPlane[2][0]->DownColor());
            BackPlane[1][2]->setBackColor(DownPlane[1][0]->DownColor());
            BackPlane[2][2]->setBackColor(DownPlane[0][0]->DownColor());

            DownPlane[0][0]->setDownColor(front_0_0);
            DownPlane[1][0]->setDownColor(front_1_0);
            DownPlane[2][0]->setDownColor(front_2_0);

            visualRotateMiniMachineGun(4, ROTATE_SPEED_STEP, 1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "L' ";

            string left_0_0 = LeftPlane[0][0]->LeftColor(),
                    left_0_1 = LeftPlane[0][1]->LeftColor();

            LeftPlane[0][0]->setLeftColor(LeftPlane[0][2]->LeftColor());
            LeftPlane[0][1]->setLeftColor(LeftPlane[1][2]->LeftColor());
            LeftPlane[0][2]->setLeftColor(LeftPlane[2][2]->LeftColor());
            LeftPlane[1][2]->setLeftColor(LeftPlane[2][1]->LeftColor());
            LeftPlane[2][2]->setLeftColor(LeftPlane[2][0]->LeftColor());
            LeftPlane[2][1]->setLeftColor(LeftPlane[1][0]->LeftColor());
            LeftPlane[2][0]->setLeftColor(left_0_0);
            LeftPlane[1][0]->setLeftColor(left_0_1);

            FrontPlane[0][0]->setFrontColor(DownPlane[0][0]->DownColor());
            FrontPlane[1][0]->setFrontColor(DownPlane[1][0]->DownColor());
            FrontPlane[2][0]->setFrontColor(DownPlane[2][0]->DownColor());

            DownPlane[0][0]->setDownColor(BackPlane[2][2]->BackColor());
            DownPlane[1][0]->setDownColor(BackPlane[1][2]->BackColor());
            DownPlane[2][0]->setDownColor(BackPlane[0][2]->BackColor());

            BackPlane[0][2]->setBackColor(UpPlane[2][0]->UpColor());
            BackPlane[1][2]->setBackColor(UpPlane[1][0]->UpColor());
            BackPlane[2][2]->setBackColor(UpPlane[0][0]->UpColor());

            UpPlane[0][0]->setUpColor(front_0_0);
            UpPlane[1][0]->setUpColor(front_1_0);
            UpPlane[2][0]->setUpColor(front_2_0);

            visualRotateMiniMachineGun(4, ROTATE_SPEED_STEP, -1);
        }
    }

    void RotateRightPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string front_0_2 = FrontPlane[0][2]->FrontColor(),
                front_1_2 = FrontPlane[1][2]->FrontColor(), front_2_2 = FrontPlane[2][2]->FrontColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "R ";

            string right_0_0 = RightPlane[0][0]->RightColor(),
                    right_1_0 = RightPlane[1][0]->RightColor();

            RightPlane[0][0]->setRightColor(RightPlane[2][0]->RightColor());
            RightPlane[1][0]->setRightColor(RightPlane[2][1]->RightColor());
            RightPlane[2][0]->setRightColor(RightPlane[2][2]->RightColor());
            RightPlane[2][1]->setRightColor(RightPlane[1][2]->RightColor());
            RightPlane[2][2]->setRightColor(RightPlane[0][2]->RightColor());
            RightPlane[1][2]->setRightColor(RightPlane[0][1]->RightColor());
            RightPlane[0][2]->setRightColor(right_0_0);
            RightPlane[0][1]->setRightColor(right_1_0);

            FrontPlane[0][2]->setFrontColor(DownPlane[0][2]->DownColor());
            FrontPlane[1][2]->setFrontColor(DownPlane[1][2]->DownColor());
            FrontPlane[2][2]->setFrontColor(DownPlane[2][2]->DownColor());

            DownPlane[0][2]->setDownColor(BackPlane[2][0]->BackColor());
            DownPlane[1][2]->setDownColor(BackPlane[1][0]->BackColor());
            DownPlane[2][2]->setDownColor(BackPlane[0][0]->BackColor());

            BackPlane[0][0]->setBackColor(UpPlane[2][2]->UpColor());
            BackPlane[1][0]->setBackColor(UpPlane[1][2]->UpColor());
            BackPlane[2][0]->setBackColor(UpPlane[0][2]->UpColor());

            UpPlane[0][2]->setUpColor(front_0_2);
            UpPlane[1][2]->setUpColor(front_1_2);
            UpPlane[2][2]->setUpColor(front_2_2);

            visualRotateMiniMachineGun(5, ROTATE_SPEED_STEP, -1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "R' ";

            string right_0_1 = RightPlane[0][1]->RightColor(),
                    right_0_2 = RightPlane[0][2]->RightColor();

            RightPlane[0][1]->setRightColor(RightPlane[1][2]->RightColor());
            RightPlane[0][2]->setRightColor(RightPlane[2][2]->RightColor());
            RightPlane[1][2]->setRightColor(RightPlane[2][1]->RightColor());
            RightPlane[2][2]->setRightColor(RightPlane[2][0]->RightColor());
            RightPlane[2][1]->setRightColor(RightPlane[1][0]->RightColor());
            RightPlane[2][0]->setRightColor(RightPlane[0][0]->RightColor());
            RightPlane[1][0]->setRightColor(right_0_1);
            RightPlane[0][0]->setRightColor(right_0_2);

            FrontPlane[0][2]->setFrontColor(UpPlane[0][2]->UpColor());
            FrontPlane[1][2]->setFrontColor(UpPlane[1][2]->UpColor());
            FrontPlane[2][2]->setFrontColor(UpPlane[2][2]->UpColor());

            UpPlane[0][2]->setUpColor(BackPlane[2][0]->BackColor());
            UpPlane[1][2]->setUpColor(BackPlane[1][0]->BackColor());
            UpPlane[2][2]->setUpColor(BackPlane[0][0]->BackColor());

            BackPlane[0][0]->setBackColor(DownPlane[2][2]->DownColor());
            BackPlane[1][0]->setBackColor(DownPlane[1][2]->DownColor());
            BackPlane[2][0]->setBackColor(DownPlane[0][2]->DownColor());

            DownPlane[0][2]->setDownColor(front_0_2);
            DownPlane[1][2]->setDownColor(front_1_2);
            DownPlane[2][2]->setDownColor(front_2_2);

            visualRotateMiniMachineGun(5, ROTATE_SPEED_STEP, 1);
        }
    }

    void RotateFrontPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string up_2_0 = UpPlane[2][0]->UpColor(),
                up_2_1 = UpPlane[2][1]->UpColor(), up_2_2 = UpPlane[2][2]->UpColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "F ";

            string front_0_0 = FrontPlane[0][0]->FrontColor(),
                    front_1_0 = FrontPlane[1][0]->FrontColor();

            FrontPlane[0][0]->setFrontColor(FrontPlane[2][0]->FrontColor());
            FrontPlane[1][0]->setFrontColor(FrontPlane[2][1]->FrontColor());
            FrontPlane[2][0]->setFrontColor(FrontPlane[2][2]->FrontColor());
            FrontPlane[2][1]->setFrontColor(FrontPlane[1][2]->FrontColor());
            FrontPlane[2][2]->setFrontColor(FrontPlane[0][2]->FrontColor());
            FrontPlane[1][2]->setFrontColor(FrontPlane[0][1]->FrontColor());
            FrontPlane[0][2]->setFrontColor(front_0_0);
            FrontPlane[0][1]->setFrontColor(front_1_0);

            UpPlane[2][0]->setUpColor(LeftPlane[2][2]->LeftColor());
            UpPlane[2][1]->setUpColor(LeftPlane[1][2]->LeftColor());
            UpPlane[2][2]->setUpColor(LeftPlane[0][2]->LeftColor());

            LeftPlane[0][2]->setLeftColor(DownPlane[0][0]->DownColor());
            LeftPlane[1][2]->setLeftColor(DownPlane[0][1]->DownColor());
            LeftPlane[2][2]->setLeftColor(DownPlane[0][2]->DownColor());

            DownPlane[0][0]->setDownColor(RightPlane[2][0]->RightColor());
            DownPlane[0][1]->setDownColor(RightPlane[1][0]->RightColor());
            DownPlane[0][2]->setDownColor(RightPlane[0][0]->RightColor());

            RightPlane[0][0]->setRightColor(up_2_0);
            RightPlane[1][0]->setRightColor(up_2_1);
            RightPlane[2][0]->setRightColor(up_2_2);

            visualRotateMiniMachineGun(1, ROTATE_SPEED_STEP, -1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "F' ";

            string front_0_0 = FrontPlane[0][0]->FrontColor(),
                    front_0_1 = FrontPlane[0][1]->FrontColor();

            FrontPlane[0][0]->setFrontColor(FrontPlane[0][2]->FrontColor());
            FrontPlane[0][1]->setFrontColor(FrontPlane[1][2]->FrontColor());
            FrontPlane[0][2]->setFrontColor(FrontPlane[2][2]->FrontColor());
            FrontPlane[1][2]->setFrontColor(FrontPlane[2][1]->FrontColor());
            FrontPlane[2][2]->setFrontColor(FrontPlane[2][0]->FrontColor());
            FrontPlane[2][1]->setFrontColor(FrontPlane[1][0]->FrontColor());
            FrontPlane[2][0]->setFrontColor(front_0_0);
            FrontPlane[1][0]->setFrontColor(front_0_1);

            UpPlane[2][0]->setUpColor(RightPlane[0][0]->RightColor());
            UpPlane[2][1]->setUpColor(RightPlane[1][0]->RightColor());
            UpPlane[2][2]->setUpColor(RightPlane[2][0]->RightColor());

            RightPlane[0][0]->setRightColor(DownPlane[0][2]->DownColor());
            RightPlane[1][0]->setRightColor(DownPlane[0][1]->DownColor());
            RightPlane[2][0]->setRightColor(DownPlane[0][0]->DownColor());

            DownPlane[0][0]->setDownColor(LeftPlane[0][2]->LeftColor());
            DownPlane[0][1]->setDownColor(LeftPlane[1][2]->LeftColor());
            DownPlane[0][2]->setDownColor(LeftPlane[2][2]->LeftColor());

            LeftPlane[0][2]->setLeftColor(up_2_2);
            LeftPlane[1][2]->setLeftColor(up_2_1);
            LeftPlane[2][2]->setLeftColor(up_2_0);

            visualRotateMiniMachineGun(1, ROTATE_SPEED_STEP, 1);
        }
    }

    void RotateBackPlane(const char degree, bool isWriteToConsole = true) {
        RotatesCounter++;
        string up_0_0 = UpPlane[0][0]->UpColor(),
                up_0_1 = UpPlane[0][1]->UpColor(), up_0_2 = UpPlane[0][2]->UpColor();

        if (degree == '+') {
            if (isWriteToConsole)
                cout << "B ";

            string back_0_0 = BackPlane[0][0]->BackColor(),
                    back_1_0 = BackPlane[1][0]->BackColor();

            BackPlane[0][0]->setBackColor(BackPlane[2][0]->BackColor());
            BackPlane[1][0]->setBackColor(BackPlane[2][1]->BackColor());
            BackPlane[2][0]->setBackColor(BackPlane[2][2]->BackColor());
            BackPlane[2][1]->setBackColor(BackPlane[1][2]->BackColor());
            BackPlane[2][2]->setBackColor(BackPlane[0][2]->BackColor());
            BackPlane[1][2]->setBackColor(BackPlane[0][1]->BackColor());
            BackPlane[0][2]->setBackColor(back_0_0);
            BackPlane[0][1]->setBackColor(back_1_0);

            UpPlane[0][0]->setUpColor(RightPlane[0][2]->RightColor());
            UpPlane[0][1]->setUpColor(RightPlane[1][2]->RightColor());
            UpPlane[0][2]->setUpColor(RightPlane[2][2]->RightColor());

            RightPlane[0][2]->setRightColor(DownPlane[2][2]->DownColor());
            RightPlane[1][2]->setRightColor(DownPlane[2][1]->DownColor());
            RightPlane[2][2]->setRightColor(DownPlane[2][0]->DownColor());

            DownPlane[2][0]->setDownColor(LeftPlane[0][0]->LeftColor());
            DownPlane[2][1]->setDownColor(LeftPlane[1][0]->LeftColor());
            DownPlane[2][2]->setDownColor(LeftPlane[2][0]->LeftColor());

            LeftPlane[2][0]->setLeftColor(up_0_0);
            LeftPlane[1][0]->setLeftColor(up_0_1);
            LeftPlane[0][0]->setLeftColor(up_0_2);

            visualRotateMiniMachineGun(0, ROTATE_SPEED_STEP, 1);

        } else if (degree == '-') {
            if (isWriteToConsole)
                cout << "B' ";

            string back_0_0 = BackPlane[0][0]->BackColor(),
                    back_0_1 = BackPlane[0][1]->BackColor();

            BackPlane[0][0]->setBackColor(BackPlane[0][2]->BackColor());
            BackPlane[0][1]->setBackColor(BackPlane[1][2]->BackColor());
            BackPlane[0][2]->setBackColor(BackPlane[2][2]->BackColor());
            BackPlane[1][2]->setBackColor(BackPlane[2][1]->BackColor());
            BackPlane[2][2]->setBackColor(BackPlane[2][0]->BackColor());
            BackPlane[2][1]->setBackColor(BackPlane[1][0]->BackColor());
            BackPlane[2][0]->setBackColor(back_0_0);
            BackPlane[1][0]->setBackColor(back_0_1);

            UpPlane[0][0]->setUpColor(LeftPlane[2][0]->LeftColor());
            UpPlane[0][1]->setUpColor(LeftPlane[1][0]->LeftColor());
            UpPlane[0][2]->setUpColor(LeftPlane[0][0]->LeftColor());

            LeftPlane[0][0]->setLeftColor(DownPlane[2][0]->DownColor());
            LeftPlane[1][0]->setLeftColor(DownPlane[2][1]->DownColor());
            LeftPlane[2][0]->setLeftColor(DownPlane[2][2]->DownColor());

            DownPlane[2][0]->setDownColor(RightPlane[2][2]->RightColor());
            DownPlane[2][1]->setDownColor(RightPlane[1][2]->RightColor());
            DownPlane[2][2]->setDownColor(RightPlane[0][2]->RightColor());

            RightPlane[0][2]->setRightColor(up_0_0);
            RightPlane[1][2]->setRightColor(up_0_1);
            RightPlane[2][2]->setRightColor(up_0_2);

            visualRotateMiniMachineGun(0, ROTATE_SPEED_STEP, -1);
        }
    }

    void RotateMachineGun(string commandsSeq, bool isWriteToConsole = true) {
        unsigned long size = commandsSeq.size();
        for (int i = 0; i < size; i += 2) {
            if (commandsSeq[i] == 'U') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateUpPlane('+', isWriteToConsole);
                else
                    RotateUpPlane('-', isWriteToConsole);
                continue;
            }

            if (commandsSeq[i] == 'D') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateDownPlane('+', isWriteToConsole);
                else
                    RotateDownPlane('-', isWriteToConsole);
                continue;
            }

            if (commandsSeq[i] == 'L') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateLeftPlane('+', isWriteToConsole);
                else
                    RotateLeftPlane('-', isWriteToConsole);
                continue;
            }

            if (commandsSeq[i] == 'R') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateRightPlane('+', isWriteToConsole);
                else
                    RotateRightPlane('-', isWriteToConsole);
                continue;
            }

            if (commandsSeq[i] == 'F') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateFrontPlane('+', isWriteToConsole);
                else
                    RotateFrontPlane('-', isWriteToConsole);
                continue;
            }

            if (commandsSeq[i] == 'B') {
                if (commandsSeq[i + 1] == '+' || commandsSeq[i + 1] == ' ')
                    RotateBackPlane('+', isWriteToConsole);
                else
                    RotateBackPlane('-', isWriteToConsole);
                continue;
            }
        }
    }

    // Алгоритмы
    void RightAlgorithm(bool isWriteToConsole = true) {
        this->RotateRightPlane('+', isWriteToConsole);
        this->RotateUpPlane('+', isWriteToConsole);
        this->RotateRightPlane('-', isWriteToConsole);
        this->RotateUpPlane('-', isWriteToConsole);
    }

    void LeftAlgorithm(bool isWriteToConsole = true) {
        this->RotateLeftPlane('+', isWriteToConsole);
        this->RotateUpPlane('+', isWriteToConsole);
        this->RotateLeftPlane('-', isWriteToConsole);
        this->RotateUpPlane('-', isWriteToConsole);
    }

    void UpAlgorithm(bool isWriteToConsole = true) {
        this->RotateUpPlane('+', isWriteToConsole);
        this->RotateLeftPlane('+', isWriteToConsole);
        this->RotateUpPlane('-', isWriteToConsole);
        this->RotateLeftPlane('-', isWriteToConsole);
    }

    void DownAlgorithm(bool isWriteToConsole = true) {
        this->RotateDownPlane('-', isWriteToConsole);
        this->RotateRightPlane('-', isWriteToConsole);
        this->RotateDownPlane('+', isWriteToConsole);
        this->RotateLeftPlane('+', isWriteToConsole);
    }

    void LeftUpSolver(bool isWriteToConsole = true) {
        if (isCubeCompleted()) {
            RotatesCounter = 0;
            do {
                RotateUpPlane('+', isWriteToConsole);
                RotateLeftPlane('-', isWriteToConsole);
            } while (!isCubeCompleted());
            cout << "\nКубик снова собран!\n";
        } else
            cout << "\nДанный алгоритм нельзя использовать, если кубик не собран! Ты что, дурачок?\n";
    }

    void RightUpSolver(bool isWriteToConsole = true) {
        if (isCubeCompleted()) {
            RotatesCounter = 0;
            do {
                RotateRightPlane('-', isWriteToConsole);
                RotateUpPlane('+', isWriteToConsole);
            } while (!isCubeCompleted());
            cout << "\nКубик снова собран!\n";
        } else
            cout << "\nДанный алгоритм нельзя использовать, если кубик не собран! Ты что дурачок?\n";
    }

    // Разборка кубика
    void Shuffle(bool isWriteToConsole = true, int countTurns = 10 + rand() % 5) {
        for (int i = 0; i < countTurns; ++i) {
            int temp = 1 + rand() % 4;
            switch (temp) {
                case (1):
                    RightAlgorithm(isWriteToConsole);
                    break;
                case (2):
                    LeftAlgorithm(isWriteToConsole);
                    break;
                case (3):
                    UpAlgorithm(isWriteToConsole);
                    break;
                case (4):
                    DownAlgorithm(isWriteToConsole);
                    break;
                default:
                    break;
            }
        }
        if (isCubeCompleted())
            Shuffle(isWriteToConsole);
    }

    // Проверка готовности стадий сборки кубика
    [[nodiscard]] bool isFirstStepCompleted() const {
        bool isDownCompleted = (DownPlane[1][0]->DownColor() == DownCenter) &&
                (DownPlane[1][2]->DownColor() == DownCenter) && (DownPlane[0][1]->DownColor() == DownCenter) &&
                (DownPlane[2][1]->DownColor() == DownCenter);

        bool isLeftCompleted = LeftCenter == LeftPlane[2][1]->LeftColor();
        bool isRightCompleted = RightCenter == RightPlane[2][1]->RightColor();
        bool isFrontCompleted = FrontCenter == FrontPlane[2][1]->FrontColor();
        bool isBackCompleted = BackCenter == BackPlane[2][1]->BackColor();

        bool isCompleted =
                isDownCompleted && isLeftCompleted && isRightCompleted && isFrontCompleted && isBackCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isSecondStepCompleted() const {

        bool isFirstCompleted = isFirstStepCompleted();

        bool isDownCompleted = (DownPlane[0][0]->DownColor() == DownCenter) &&
                (DownPlane[0][2]->DownColor() == DownCenter) && (DownPlane[2][0]->DownColor() == DownCenter) &&
                (DownPlane[2][2]->DownColor() == DownCenter);

        bool isLeftCompleted = LeftPlane[2][0]->LeftColor() == LeftCenter &&
                LeftPlane[2][2]->LeftColor() == LeftCenter;

        bool isRightCompleted = RightPlane[2][0]->RightColor() == RightCenter &&
                RightPlane[2][2]->RightColor() == RightCenter;

        bool isFrontCompleted = FrontPlane[2][0]->FrontColor() == FrontCenter &&
                FrontPlane[2][2]->FrontColor() == FrontCenter;

        bool isBackCompleted = BackPlane[2][0]->BackColor() == BackCenter &&
                BackPlane[2][2]->BackColor() == BackCenter;

        bool isCompleted =
                isFirstCompleted && isDownCompleted && isLeftCompleted && isRightCompleted && isFrontCompleted &&
                        isBackCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isThirdStepCompleted() const {

        bool isSecondCompleted = isSecondStepCompleted();

        bool isLeftCompleted = LeftPlane[1][0]->LeftColor() == LeftCenter &&
                LeftPlane[1][2]->LeftColor() == LeftCenter;

        bool isFrontCompleted = FrontPlane[1][0]->FrontColor() == FrontCenter &&
                FrontPlane[1][2]->FrontColor() == FrontCenter;

        bool isRightCompleted = RightPlane[1][0]->RightColor() == RightCenter &&
                RightPlane[1][2]->RightColor() == RightCenter;

        bool isBackCompleted = BackPlane[1][0]->BackColor() == BackCenter &&
                BackPlane[1][2]->BackColor() == BackCenter;

        bool isCompleted =
                isSecondCompleted && isLeftCompleted && isRightCompleted && isFrontCompleted && isBackCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isFourthStepCompleted() const {
        bool isThirdCompleted = isThirdStepCompleted();

        bool isUpCompleted =
                UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][0]->UpColor() == UpCenter &&
                        UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter;

        bool isCompleted = isThirdCompleted && isUpCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isFifthStepCompleted() const {
        bool isFourthCompleted = isFourthStepCompleted();

        bool isLeftCompleted = LeftPlane[0][1]->LeftColor() == LeftCenter;

        bool isRightCompleted = RightPlane[0][1]->RightColor() == RightCenter;

        bool isFrontCompleted = FrontPlane[0][1]->FrontColor() == FrontCenter;

        bool isBackCompleted = BackPlane[0][1]->BackColor() == BackCenter;

        bool isCompleted =
                isFourthCompleted && isLeftCompleted && isRightCompleted && isFrontCompleted && isBackCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isSixthStepCompleted() const {
        bool isFifthCompleted = isFifthStepCompleted();

        bool isLeftCornersCompleted = (LeftPlane[0][0]->LeftColor() == LeftCenter ||
                LeftPlane[0][0]->LeftColor() == BackCenter || LeftPlane[0][0]->LeftColor() == UpCenter) &&
                (LeftPlane[0][2]->LeftColor() == LeftCenter || LeftPlane[0][2]->LeftColor() == FrontCenter ||
                        LeftPlane[0][2]->LeftColor() == UpCenter);

        bool isBackCornersCompleted = (BackPlane[0][0]->BackColor() == BackCenter ||
                BackPlane[0][0]->BackColor() == RightCenter || BackPlane[0][0]->BackColor() == UpCenter) &&
                (BackPlane[0][2]->BackColor() == BackCenter || BackPlane[0][2]->BackColor() == LeftCenter ||
                        BackPlane[0][2]->BackColor() == UpCenter);

        bool isRightCornersCompleted = (RightPlane[0][0]->RightColor() == RightCenter ||
                RightPlane[0][0]->RightColor() == FrontCenter || RightPlane[0][0]->RightColor() == UpCenter) &&
                (RightPlane[0][2]->RightColor() == RightCenter || RightPlane[0][2]->RightColor() == BackCenter ||
                        RightPlane[0][2]->RightColor() == UpCenter);

        bool isFrontCornersCompleted = (FrontPlane[0][0]->FrontColor() == FrontCenter ||
                FrontPlane[0][0]->FrontColor() == LeftCenter || FrontPlane[0][0]->FrontColor() == UpCenter) &&
                (FrontPlane[0][2]->FrontColor() == FrontCenter || FrontPlane[0][2]->FrontColor() == RightCenter ||
                        FrontPlane[0][2]->FrontColor() == UpCenter);

        bool isUpCornersCompleted =
                UpPlane[0][0]->UpColor() != FrontCenter && UpPlane[0][0]->UpColor() != RightCenter &&
                        (UpPlane[0][0]->UpColor() == LeftCenter || UpPlane[0][0]->UpColor() == BackCenter ||
                                UpPlane[0][0]->UpColor() == UpCenter) &&
                        UpPlane[0][2]->UpColor() != FrontCenter && UpPlane[0][2]->UpColor() != LeftCenter &&
                        (UpPlane[0][2]->UpColor() == RightCenter || UpPlane[0][2]->UpColor() == BackCenter ||
                                UpPlane[0][2]->UpColor() == UpCenter) &&
                        UpPlane[2][0]->UpColor() != BackCenter && UpPlane[2][0]->UpColor() != RightCenter &&
                        (UpPlane[2][0]->UpColor() == LeftCenter || UpPlane[2][0]->UpColor() == FrontCenter ||
                                UpPlane[2][0]->UpColor() == UpCenter) &&
                        UpPlane[2][2]->UpColor() != BackCenter && UpPlane[2][2]->UpColor() != LeftCenter &&
                        (UpPlane[2][2]->UpColor() == RightCenter || UpPlane[2][2]->UpColor() == FrontCenter ||
                                UpPlane[2][2]->UpColor() == UpCenter);

        bool isCompleted =
                isFifthCompleted && isLeftCornersCompleted && isRightCornersCompleted && isFrontCornersCompleted &&
                        isBackCornersCompleted && isUpCornersCompleted;

        return isCompleted;
    }

    [[nodiscard]] bool isCubeCompleted() const {
        if (!CheckIfCubeCorrect())
            throw exception();

        bool isFifthCompleted = isFifthStepCompleted();

        bool isFrontCornersCompleted =
                FrontPlane[0][0]->FrontColor() == FrontCenter && FrontPlane[0][2]->FrontColor() == FrontCenter;

        bool isBackCornersCompleted =
                BackPlane[0][0]->BackColor() == BackCenter && BackPlane[0][2]->BackColor() == BackCenter;

        bool isLeftCornersCompleted =
                LeftPlane[0][0]->LeftColor() == LeftCenter && LeftPlane[0][2]->LeftColor() == LeftCenter;

        bool isRightCornersCompleted =
                RightPlane[0][0]->RightColor() == RightCenter && RightPlane[0][2]->RightColor() == RightCenter;

        bool isCompleted =
                isFifthCompleted && isFrontCornersCompleted && isBackCornersCompleted && isLeftCornersCompleted &&
                        isRightCornersCompleted;

        return isCompleted;

    }

    // Стадии сборки кубика
    void FirstStep(int count = 0, bool isWriteToConsole = true) {
        if (FirstStepCounter++ > 128) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (1й шаг)\n");
            throw exception();
        }

        if (!isFirstStepCompleted()) {
            if (FrontCenter == FrontPlane[0][1]->FrontColor() && UpPlane[2][1]->UpColor() == DownCenter) {
                RotateMachineGun("F F ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (RightCenter == RightPlane[0][1]->RightColor() && UpPlane[1][2]->UpColor() == DownCenter) {
                RotateMachineGun("R R ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (LeftCenter == LeftPlane[0][1]->LeftColor() && UpPlane[1][0]->UpColor() == DownCenter) {
                RotateMachineGun("L L ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (BackCenter == BackPlane[0][1]->BackColor() && UpPlane[0][1]->UpColor() == DownCenter) {
                RotateMachineGun("B B ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][1]->FrontColor() == DownCenter && UpPlane[2][1]->UpColor() == FrontCenter) {
                RotateMachineGun("U'R'F R ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (UpPlane[1][2]->UpColor() == RightCenter && RightPlane[0][1]->RightColor() == DownCenter) {
                RotateMachineGun("U'B'R B ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (LeftPlane[0][1]->LeftColor() == DownCenter && UpPlane[1][0]->UpColor() == LeftCenter) {
                RotateMachineGun("U'F'L F ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == DownCenter && UpPlane[0][1]->UpColor() == BackCenter) {
                RotateMachineGun("U'L'B L ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (FrontPlane[1][2]->FrontColor() == DownCenter) {
                RotateMachineGun("F'U'F ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (RightPlane[1][2]->RightColor() == DownCenter) {
                RotateMachineGun("R'U'R ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (LeftPlane[1][2]->LeftColor() == DownCenter) {
                RotateMachineGun("L'U'L ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (BackPlane[1][2]->BackColor() == DownCenter) {
                RotateMachineGun("B'U'B ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (FrontPlane[1][0]->FrontColor() == DownCenter) {
                RotateMachineGun("F U'F'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (RightPlane[1][0]->RightColor() == DownCenter) {
                RotateMachineGun("R U'R'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (LeftPlane[1][0]->LeftColor() == DownCenter) {
                RotateMachineGun("L U'L'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (BackPlane[1][0]->BackColor() == DownCenter) {
                RotateMachineGun("B U'B'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (FrontPlane[2][1]->FrontColor() == DownCenter) {
                RotateMachineGun("F F U'F F ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (RightPlane[2][1]->RightColor() == DownCenter) {
                RotateMachineGun("R R U'R R ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (LeftPlane[2][1]->LeftColor() == DownCenter) {
                RotateMachineGun("L L U'L L ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (BackPlane[2][1]->BackColor() == DownCenter) {
                RotateMachineGun("B B U'B B ", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (DownPlane[0][1]->DownColor() == DownCenter && FrontPlane[2][1]->FrontColor() != FrontCenter) {
                RotateMachineGun("F'F'U'F'F'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (DownPlane[1][2]->DownColor() == DownCenter && RightPlane[2][1]->RightColor() != RightCenter) {
                RotateMachineGun("R'R'U'R'R'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (DownPlane[1][0]->DownColor() == DownCenter && LeftPlane[2][1]->LeftColor() != LeftCenter) {
                RotateMachineGun("L'L'U'L'L'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }
            if (DownPlane[2][1]->DownColor() == DownCenter && BackPlane[2][1]->BackColor() != BackCenter) {
                RotateMachineGun("B'B'U'B'B'", isWriteToConsole);
                FirstStep(0, isWriteToConsole);
            }

            if (!isFirstStepCompleted() && count < 4) {
                RotateMachineGun("U ", isWriteToConsole);
                FirstStep(++count, isWriteToConsole);
            } else if (!isFirstStepCompleted() && count == 4) {
                Shuffle(isWriteToConsole, 3);
                FirstStep(0, isWriteToConsole);
            }
        }
    }

    void SecondStep(int count = 0, bool isWriteToConsole = true) {
        if (SecondStepCounter++ > 128) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (2й шаг)\n");
            throw exception();
        }

        if (!isSecondStepCompleted()) {
            if (FrontPlane[2][1]->FrontColor() == FrontCenter && FrontPlane[0][2]->FrontColor() == DownCenter &&
                    UpPlane[2][2]->UpColor() == FrontCenter && RightPlane[0][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter) {
                RotateMachineGun("F'U'F ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    UpPlane[2][0]->UpColor() == LeftCenter && LeftPlane[0][2]->LeftColor() == DownCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter) {
                RotateMachineGun("L'U'L ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][0]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    UpPlane[0][2]->UpColor() == RightCenter && RightPlane[0][2]->RightColor() == DownCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter) {
                RotateMachineGun("R'U'R ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][2]->BackColor() == DownCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    LeftPlane[0][0]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    UpPlane[0][0]->UpColor() == BackCenter) {
                RotateMachineGun("B'U'B ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    RightPlane[0][0]->RightColor() == DownCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    UpPlane[2][2]->UpColor() == RightCenter) {
                RotateMachineGun("R U R'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][0]->FrontColor() == DownCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    UpPlane[2][0]->UpColor() == FrontCenter && LeftPlane[0][2]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter) {
                RotateMachineGun("F U F'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][0]->BackColor() == DownCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    UpPlane[0][2]->UpColor() == BackCenter && RightPlane[0][2]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter) {
                RotateMachineGun("B U B'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    LeftPlane[0][0]->LeftColor() == DownCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    UpPlane[0][0]->UpColor() == LeftCenter) {
                RotateMachineGun("L U L'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][2]->FrontColor() == RightCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    RightPlane[0][0]->RightColor() == FrontCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    UpPlane[2][2]->UpColor() == DownCenter) {
                RotateMachineGun("R U'R'U U R U R'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][0]->FrontColor() == LeftCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    UpPlane[2][0]->UpColor() == DownCenter && RightPlane[0][2]->RightColor() == FrontCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter) {
                RotateMachineGun("F U'F'U U F U F'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][0]->BackColor() == RightCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    UpPlane[0][2]->UpColor() == DownCenter && RightPlane[0][2]->RightColor() == BackCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter) {
                RotateMachineGun("B U'B'U U B U B'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[0][2]->BackColor() == LeftCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    LeftPlane[0][0]->LeftColor() == BackCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    UpPlane[0][0]->UpColor() == DownCenter) {
                RotateMachineGun("L U'L'U U L U L'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[2][1]->FrontColor() == FrontCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    (FrontPlane[2][2]->FrontColor() == DownCenter || RightPlane[2][0]->RightColor() == DownCenter)) {
                RotateMachineGun("F'U'F ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (FrontPlane[2][1]->FrontColor() == FrontCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    (LeftPlane[2][2]->LeftColor() == DownCenter || FrontPlane[2][0]->FrontColor() == DownCenter)) {
                RotateMachineGun("L'U'L ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[2][1]->BackColor() == BackCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    (RightPlane[2][2]->RightColor() == DownCenter || BackPlane[2][0]->BackColor() == DownCenter)) {
                RotateMachineGun("R'U'R ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (BackPlane[2][1]->BackColor() == BackCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    (BackPlane[2][2]->BackColor() == DownCenter || LeftPlane[2][0]->LeftColor() == DownCenter)) {
                RotateMachineGun("B'U'B ", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
            if (LeftPlane[2][1]->LeftColor() == LeftCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    DownPlane[0][0]->DownColor() == DownCenter && DownPlane[0][1]->DownColor() == DownCenter &&
                    DownPlane[1][0]->DownColor() == DownCenter && DownPlane[1][2]->DownColor() == DownCenter &&
                    DownPlane[2][1]->DownColor() == DownCenter && (FrontPlane[2][0]->FrontColor() != FrontCenter ||
                    LeftPlane[2][2]->LeftColor() != LeftCenter)) {
                RotateMachineGun("F U'F'", isWriteToConsole);
                SecondStep(0, isWriteToConsole); // попробовать поставить XOR
            }
            if (RightPlane[2][1]->RightColor() == RightCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    DownPlane[0][1]->DownColor() == DownCenter && DownPlane[0][2]->DownColor() == DownCenter &&
                    DownPlane[1][0]->DownColor() == DownCenter && DownPlane[1][2]->DownColor() == DownCenter &&
                    DownPlane[2][1]->DownColor() == DownCenter && (FrontPlane[2][2]->FrontColor() != FrontCenter ||
                    RightPlane[2][0]->RightColor() != RightCenter)) {
                RotateMachineGun("R U'R'", isWriteToConsole);
                SecondStep(0, isWriteToConsole); // попробовать поставить XOR
            }
            if (BackPlane[2][1]->BackColor() == BackCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    DownPlane[2][2]->DownColor() == DownCenter && DownPlane[2][1]->DownColor() == DownCenter &&
                    DownPlane[1][2]->DownColor() == DownCenter && DownPlane[1][0]->DownColor() == DownCenter &&
                    DownPlane[0][1]->DownColor() == DownCenter && (BackPlane[2][0]->BackColor() != BackCenter ||
                    RightPlane[2][2]->RightColor() != RightCenter)) {
                RotateMachineGun("B U'B'", isWriteToConsole);
                SecondStep(0, isWriteToConsole); // попробовать поставить XOR
            }
            if (BackPlane[2][1]->BackColor() == BackCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    DownPlane[2][0]->DownColor() == DownCenter && DownPlane[2][1]->DownColor() == DownCenter &&
                    DownPlane[1][2]->DownColor() == DownCenter && DownPlane[1][0]->DownColor() == DownCenter &&
                    DownPlane[0][1]->DownColor() == DownCenter && (BackPlane[2][2]->BackColor() != BackCenter ||
                    LeftPlane[2][0]->LeftColor() != LeftCenter)) {
                RotateMachineGun("L U'L'", isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }

            if (!isSecondStepCompleted() && count < 4) {
                RotateMachineGun("U ", isWriteToConsole);
                SecondStep(++count, isWriteToConsole);
            } else if (!isSecondStepCompleted() && count == 4) {
                Shuffle(isWriteToConsole, 3);
                FirstStep(0, isWriteToConsole);
                SecondStep(0, isWriteToConsole);
            }
        }
    }

    void ThirdStep(int count = 0, bool isWriteToConsole = true) {
        if (ThirdStepCounter++ > 128) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (3й шаг)\n");
            throw exception();
        }

        if (!isThirdStepCompleted()) {
            if (FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[0][1]->FrontColor() == FrontCenter &&
                    LeftPlane[2][2]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][0]->LeftColor() == LeftCenter && UpPlane[2][1]->UpColor() == LeftCenter) {
                RotateMachineGun("U'L'U L U F U'F'", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[0][1]->RightColor() == RightCenter && UpPlane[1][2]->UpColor() == FrontCenter) {
                RotateMachineGun("U'F'U F U R U'R'", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && BackPlane[0][1]->BackColor() == BackCenter &&
                    RightPlane[2][2]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && UpPlane[0][1]->UpColor() == RightCenter) {
                RotateMachineGun("U'R'U R U B U'B'", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && LeftPlane[0][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][2]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][0]->LeftColor() == LeftCenter && UpPlane[1][0]->UpColor() == BackCenter) {
                RotateMachineGun("U'B'U B U L U'L'", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (RightPlane[2][2]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && FrontPlane[0][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && UpPlane[2][1]->UpColor() == RightCenter) {
                RotateMachineGun("U R U'R'U'F'U F ", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && LeftPlane[0][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][2]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][0]->LeftColor() == LeftCenter && UpPlane[1][0]->UpColor() == FrontCenter) {
                RotateMachineGun("U F U'F'U'L'U L ", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (LeftPlane[2][2]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][0]->LeftColor() == LeftCenter && BackPlane[0][1]->BackColor() == BackCenter &&
                    BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && UpPlane[0][1]->UpColor() == LeftCenter) {
                RotateMachineGun("U L U'L'U'B'U B ", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
            if (BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && RightPlane[0][1]->RightColor() == RightCenter &&
                    RightPlane[2][2]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && UpPlane[1][2]->UpColor() == BackCenter) {
                RotateMachineGun("U B U'B'U'R'U R ", isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }

            if (!isThirdStepCompleted() && count < 4) {
                RotateMachineGun("U ", isWriteToConsole);
                ThirdStep(++count, isWriteToConsole);
            } else if (count == 4 && !isThirdStepCompleted()) {
                if (FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                        FrontPlane[2][0]->FrontColor() == FrontCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                        RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                        (FrontPlane[1][2]->FrontColor() != FrontCenter || RightPlane[1][0]->RightColor() != RightCenter)) {
                    RotateMachineGun("U R U'R'U'F'U F ", isWriteToConsole);
                    ThirdStep(0, isWriteToConsole);
                } else if (
                        FrontPlane[2][2]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                                FrontPlane[2][0]->FrontColor() == FrontCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                                LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                                (FrontPlane[1][0]->FrontColor() != FrontCenter || LeftPlane[1][2]->LeftColor() != LeftCenter)) {
                    RotateMachineGun("U F U'F'U'L'U L ", isWriteToConsole);
                    ThirdStep(0, isWriteToConsole);
                } else if (
                        BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                                BackPlane[2][0]->BackColor() == BackCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                                LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                                (BackPlane[1][2]->BackColor() != BackCenter || LeftPlane[1][0]->LeftColor() != LeftCenter)) {
                    RotateMachineGun("U L U'L'U'B'U B ", isWriteToConsole);
                    ThirdStep(0, isWriteToConsole);
                } else if (
                        BackPlane[2][2]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                                BackPlane[2][0]->BackColor() == BackCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                                RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                                (BackPlane[1][0]->BackColor() != BackCenter || RightPlane[1][2]->RightColor() != RightCenter)) {
                    RotateMachineGun("U B U'B'U'R'U R ", isWriteToConsole);
                    ThirdStep(0, isWriteToConsole);
                }
            }

            if (!isThirdStepCompleted() && count == 4) {
                Shuffle(isWriteToConsole, 3);
                SecondStep(0, isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
            }
        }
    }

    void FourthStep(bool isWriteToConsole = true) {
        if (FourthStepCounter++ > 64) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (4й шаг)\n");
            throw exception();
        }

        if (!isFourthStepCompleted()) {
            if (FrontPlane[0][1]->FrontColor() == UpCenter && FrontPlane[1][0]->FrontColor() == FrontCenter &&
                    FrontPlane[1][2]->FrontColor() == FrontCenter && FrontPlane[2][0]->FrontColor() == FrontCenter &&
                    FrontPlane[2][1]->FrontColor() == FrontCenter && FrontPlane[2][2]->FrontColor() == FrontCenter &&
                    RightPlane[0][1]->RightColor() == UpCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][0]->UpColor() == UpCenter) {
                RotateMachineGun("F U R U'R'F'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (FrontPlane[0][1]->FrontColor() == UpCenter && FrontPlane[1][0]->FrontColor() == FrontCenter &&
                    FrontPlane[1][2]->FrontColor() == FrontCenter && FrontPlane[2][0]->FrontColor() == FrontCenter &&
                    FrontPlane[2][1]->FrontColor() == FrontCenter && FrontPlane[2][2]->FrontColor() == FrontCenter &&
                    LeftPlane[0][1]->LeftColor() == UpCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter) {
                RotateMachineGun("L U F U'F'L'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == UpCenter && BackPlane[1][0]->BackColor() == BackCenter &&
                    BackPlane[1][2]->BackColor() == BackCenter && BackPlane[2][0]->BackColor() == BackCenter &&
                    BackPlane[2][1]->BackColor() == BackCenter && BackPlane[2][2]->BackColor() == BackCenter &&
                    RightPlane[0][1]->RightColor() == UpCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("R U B U'B'R'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == UpCenter && BackPlane[1][0]->BackColor() == BackCenter &&
                    BackPlane[1][2]->BackColor() == BackCenter && BackPlane[2][0]->BackColor() == BackCenter &&
                    BackPlane[2][1]->BackColor() == BackCenter && BackPlane[2][2]->BackColor() == BackCenter &&
                    LeftPlane[0][1]->LeftColor() == UpCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("B U L U'L'B'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (FrontPlane[0][1]->FrontColor() == UpCenter && FrontPlane[1][0]->FrontColor() == FrontCenter &&
                    FrontPlane[1][2]->FrontColor() == FrontCenter && FrontPlane[2][0]->FrontColor() == FrontCenter &&
                    FrontPlane[2][1]->FrontColor() == FrontCenter && FrontPlane[2][2]->FrontColor() == FrontCenter &&
                    RightPlane[1][0]->RightColor() == RightCenter && RightPlane[1][2]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][2]->RightColor() == RightCenter && UpPlane[1][0]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && BackPlane[0][1]->BackColor() == UpCenter) {
                RotateMachineGun("F R U R'U'F'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && RightPlane[0][1]->RightColor() == UpCenter &&
                    RightPlane[1][0]->RightColor() == RightCenter && RightPlane[1][2]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][2]->RightColor() == RightCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter && LeftPlane[0][1]->LeftColor() == UpCenter) {
                RotateMachineGun("R B U B'U'R'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
            if (FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    FrontPlane[0][1]->FrontColor() == UpCenter && RightPlane[0][1]->RightColor() == UpCenter &&
                    LeftPlane[0][1]->LeftColor() == UpCenter && BackPlane[0][1]->BackColor() == UpCenter) {
                RotateMachineGun("F U R U'R'F'R B U B'U'R'", isWriteToConsole);
                FourthStep(isWriteToConsole);
            }

            if (!isFourthStepCompleted()) {
                Shuffle(isWriteToConsole, 3);
                ThirdStep(0, isWriteToConsole);
                FourthStep(isWriteToConsole);
            }
        }
    }

    void FifthStep(int count = 0, bool isWriteToConsole = true) {
        if (FifthStepCounter++ > 32) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (5й шаг)\n");
            throw exception();
        }

        if (!isFifthStepCompleted()) {
            if (FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    RightPlane[0][1]->RightColor() == FrontCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U L'U'U'L U L'U L ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
            if (FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    LeftPlane[0][1]->LeftColor() == FrontCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U B'U'U'B U B'U B ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
            if (FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    BackPlane[0][1]->BackColor() == FrontCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U R'U'U'R U R'U R U B'U'U'B U B'U B U R'U'U'R U R'U R ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
            if (BackPlane[1][0]->BackColor() == BackCenter && BackPlane[1][2]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][2]->BackColor() == BackCenter && BackPlane[0][1]->BackColor() == RightCenter &&
                    RightPlane[1][0]->RightColor() == RightCenter && RightPlane[1][2]->RightColor() == RightCenter &&
                    RightPlane[2][0]->RightColor() == RightCenter && RightPlane[2][1]->RightColor() == RightCenter &&
                    RightPlane[2][2]->RightColor() == RightCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U F'U'U'F U F'U F ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
            if (BackPlane[1][0]->BackColor() == BackCenter && BackPlane[1][2]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][2]->BackColor() == BackCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    LeftPlane[0][1]->LeftColor() == RightCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U B'U'U'B U B'U B U L'U'U'L U L'U L U B'U'U'B U B'U B ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
            if (BackPlane[1][0]->BackColor() == BackCenter && BackPlane[1][2]->BackColor() == BackCenter &&
                    BackPlane[2][0]->BackColor() == BackCenter && BackPlane[2][1]->BackColor() == BackCenter &&
                    BackPlane[2][2]->BackColor() == BackCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    LeftPlane[0][1]->LeftColor() == BackCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][0]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter) {
                RotateMachineGun("U R'U'U'R U R'U R ", isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }

            if (!isFifthStepCompleted() && count < 4) {
                FifthStep(++count, isWriteToConsole);
            } else if (!isFifthStepCompleted() && count == 4) {
                Shuffle(isWriteToConsole, 3);
                FourthStep(isWriteToConsole);
                FifthStep(0, isWriteToConsole);
            }
        }
    }

    void SixthStep(int count = 0, bool isWriteToConsole = true) {
        if (SixthStepCounter++ > 32) {
            solvingErrors.emplace_back("\nОшибка при сборке кубика номер " + to_string(CubeCounter) + " (6й шаг)\n");
            throw exception();
        }

        if (!isSixthStepCompleted()) {
            if (FrontPlane[0][1]->FrontColor() == FrontCenter && FrontPlane[1][0]->FrontColor() == FrontCenter &&
                    FrontPlane[1][2]->FrontColor() == FrontCenter && FrontPlane[2][0]->FrontColor() == FrontCenter &&
                    FrontPlane[2][1]->FrontColor() == FrontCenter && FrontPlane[2][2]->FrontColor() == FrontCenter &&
                    LeftPlane[0][1]->LeftColor() == LeftCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter &&
                    (LeftPlane[0][2]->LeftColor() == FrontCenter || LeftPlane[0][2]->LeftColor() == RightCenter ||
                            LeftPlane[0][2]->LeftColor() == UpCenter) && (FrontPlane[0][0]->FrontColor() == RightCenter ||
                    FrontPlane[0][0]->FrontColor() == UpCenter || FrontPlane[0][0]->FrontColor() == FrontCenter) &&
                    (UpPlane[2][0]->UpColor() == UpCenter || UpPlane[2][0]->UpColor() == RightCenter ||
                            UpPlane[2][0]->UpColor() == FrontCenter)) {

                RotateMachineGun("F'L'F R'F'L F R ", isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == BackCenter && BackPlane[1][0]->BackColor() == BackCenter &&
                    BackPlane[1][2]->BackColor() == BackCenter && BackPlane[2][0]->BackColor() == BackCenter &&
                    BackPlane[2][1]->BackColor() == BackCenter && BackPlane[2][2]->BackColor() == BackCenter &&
                    RightPlane[0][1]->RightColor() == RightCenter && RightPlane[1][0]->RightColor() == RightCenter &&
                    RightPlane[1][2]->RightColor() == RightCenter && RightPlane[2][0]->RightColor() == RightCenter &&
                    RightPlane[2][1]->RightColor() == RightCenter && RightPlane[2][2]->RightColor() == RightCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter &&
                    (RightPlane[0][2]->RightColor() == FrontCenter || RightPlane[0][2]->RightColor() == RightCenter ||
                            RightPlane[0][2]->RightColor() == UpCenter) &&
                    (BackPlane[0][0]->BackColor() == RightCenter || BackPlane[0][0]->BackColor() == UpCenter ||
                            BackPlane[0][0]->BackColor() == FrontCenter) &&
                    (UpPlane[0][2]->UpColor() == UpCenter || UpPlane[0][2]->UpColor() == RightCenter ||
                            UpPlane[0][2]->UpColor() == FrontCenter)) {
                RotateMachineGun("R'F'L'F R F'L F ", isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == BackCenter && BackPlane[1][0]->BackColor() == BackCenter &&
                    BackPlane[1][2]->BackColor() == BackCenter && BackPlane[2][0]->BackColor() == BackCenter &&
                    BackPlane[2][1]->BackColor() == BackCenter && BackPlane[2][2]->BackColor() == BackCenter &&
                    LeftPlane[0][1]->LeftColor() == LeftCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter &&
                    (BackPlane[0][2]->BackColor() == FrontCenter || BackPlane[0][2]->BackColor() == UpCenter ||
                            BackPlane[0][2]->BackColor() == RightCenter) &&
                    (LeftPlane[0][0]->LeftColor() == RightCenter || LeftPlane[0][0]->LeftColor() == UpCenter ||
                            LeftPlane[0][0]->LeftColor() == FrontCenter) &&
                    (UpPlane[0][0]->UpColor() == UpCenter || UpPlane[0][0]->UpColor() == RightCenter ||
                            UpPlane[0][0]->UpColor() == FrontCenter)) {
                RotateMachineGun("F'L'B'L F L'B L ", isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }
            if (FrontPlane[0][1]->FrontColor() == FrontCenter &&
                    FrontPlane[1][0]->FrontColor() == FrontCenter && FrontPlane[1][2]->FrontColor() == FrontCenter &&
                    FrontPlane[2][0]->FrontColor() == FrontCenter && FrontPlane[2][1]->FrontColor() == FrontCenter &&
                    FrontPlane[2][2]->FrontColor() == FrontCenter && LeftPlane[0][1]->LeftColor() == LeftCenter &&
                    LeftPlane[1][0]->LeftColor() == LeftCenter && LeftPlane[1][2]->LeftColor() == LeftCenter &&
                    LeftPlane[2][0]->LeftColor() == LeftCenter && LeftPlane[2][1]->LeftColor() == LeftCenter &&
                    LeftPlane[2][2]->LeftColor() == LeftCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][2]->UpColor() == UpCenter &&
                    UpPlane[2][1]->UpColor() == UpCenter &&
                    (LeftPlane[0][2]->LeftColor() == RightCenter || LeftPlane[0][2]->LeftColor() == BackCenter ||
                            LeftPlane[0][2]->LeftColor() == UpCenter) &&
                    (FrontPlane[0][0]->FrontColor() == BackCenter || FrontPlane[0][0]->FrontColor() == RightCenter ||
                            FrontPlane[0][0]->FrontColor() == UpCenter) &&
                    (UpPlane[2][0]->UpColor() == UpCenter || UpPlane[2][0]->UpColor() == RightCenter ||
                            UpPlane[2][0]->UpColor() == BackCenter)) {
                RotateMachineGun("B'R'B L'B'R B L ", isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }
            if (BackPlane[0][1]->BackColor() == BackCenter && BackPlane[1][0]->BackColor() == BackCenter &&
                    BackPlane[1][2]->BackColor() == BackCenter && BackPlane[2][0]->BackColor() == BackCenter &&
                    BackPlane[2][1]->BackColor() == BackCenter && BackPlane[2][2]->BackColor() == BackCenter &&
                    LeftPlane[0][1]->LeftColor() == LeftCenter && LeftPlane[1][0]->LeftColor() == LeftCenter &&
                    LeftPlane[1][2]->LeftColor() == LeftCenter && LeftPlane[2][0]->LeftColor() == LeftCenter &&
                    LeftPlane[2][1]->LeftColor() == LeftCenter && LeftPlane[2][2]->LeftColor() == LeftCenter &&
                    UpPlane[0][1]->UpColor() == UpCenter && UpPlane[0][1]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter &&
                    (BackPlane[0][2]->BackColor() == RightCenter || BackPlane[0][2]->BackColor() == BackCenter ||
                            BackPlane[0][2]->BackColor() == UpCenter) &&
                    (LeftPlane[0][0]->LeftColor() == BackCenter || LeftPlane[0][0]->LeftColor() == UpCenter ||
                            LeftPlane[0][0]->LeftColor() == RightCenter) &&
                    (UpPlane[0][0]->UpColor() == UpCenter || UpPlane[0][0]->UpColor() == BackCenter ||
                            UpPlane[0][0]->UpColor() == RightCenter)) {
                RotateMachineGun("L'B'R'B L B'R B ", isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }

            if (!isSixthStepCompleted() && count < 4) {
                SixthStep(++count, isWriteToConsole);
            } else if (!isSixthStepCompleted() && count == 4) {
                Shuffle(isWriteToConsole, 3);
                FifthStep(0, isWriteToConsole);
                SixthStep(0, isWriteToConsole);
            }

        }
    }

    void SeventhStep(int count = 0, bool isSequenceStarted = false, bool isWriteToConsole = true) {
        if (SeventhStepCounter++ > 16) {
            solvingErrors.emplace_back(
                    "\nКубик номер " + to_string(CubeCounter) + " не может быть собран, проверьте конфигурацию\n");
            throw exception();
        }

        if (!isCubeCompleted()) {
            if (UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][0]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter & UpPlane[2][1]->UpColor() == UpCenter &&
                    RightPlane[0][0]->RightColor() == UpCenter) {
                RotateMachineGun("F'R F R'F'R F R'", isWriteToConsole);
                if (isSequenceStarted)
                    RotateMachineGun("U ", isWriteToConsole);
                else isSequenceStarted = true;

            } else if (UpPlane[0][1]->UpColor() == UpCenter && UpPlane[1][0]->UpColor() == UpCenter &&
                    UpPlane[1][2]->UpColor() == UpCenter && UpPlane[2][1]->UpColor() == UpCenter &&
                    FrontPlane[0][2]->FrontColor() == UpCenter) {
                RotateMachineGun("R F'R'F R F'R'F ", isWriteToConsole);
                if (isSequenceStarted)
                    RotateMachineGun("U ", isWriteToConsole);
                else isSequenceStarted = true;
            }

            if (!isCubeCompleted()) {
                RotateMachineGun("U ", isWriteToConsole);
                SeventhStep(++count, isSequenceStarted, isWriteToConsole);
            }
        }
    }

    void FindSolution(bool isWriteToConsole = true) {
        if (!CheckIfCubeCorrect()) {
            cout << "\n\nКубик имеет неправильные центры, попробуйте ввести другой\n";
            throw exception();
        }

        if (isCubeCompleted())
            cout << "\n\nКубик уже собран!\n";

        else {
            try {
                if (isWriteToConsole)
                    cout << "\n\nШаги сборки:\n";
                RotatesCounter = 0;
                FirstStep(0, isWriteToConsole);
                SecondStep(0, isWriteToConsole);
                ThirdStep(0, isWriteToConsole);
                FourthStep(isWriteToConsole);
                FifthStep(0, isWriteToConsole);
                SixthStep(0, isWriteToConsole);
                SeventhStep(0, false, isWriteToConsole);
                if (isCubeCompleted() && isWriteToConsole)
                    cout << "\nКоличество шагов, потребовавшееся для сборки: " << getRotatesCounter() << "\n";
            } catch (...) {}
        }
    }

    //Ввод развёртки
    void ReadRubikCube(istream &streamIn = cin) {

        string color;

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                UpPlane[i][j]->setUpColor(color);
                visualColors[j][2][i].setColor(3, setRGBColor(color));
            }

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                LeftPlane[i][j]->setLeftColor(color);
                visualColors[0][2 - i][j].setColor(4, setRGBColor(color));
            }

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                FrontPlane[i][j]->setFrontColor(color);
                visualColors[j][2 - i][2].setColor(0, setRGBColor(color));
            }

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                RightPlane[i][j]->setRightColor(color);
                visualColors[2][2 - i][2 - j].setColor(5, setRGBColor(color));
            }

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                BackPlane[i][j]->setBackColor(color);
                visualColors[2 - j][2 - i][0].setColor(1, setRGBColor(color));
            }

        for (int i = 0, ik = 2; i < 3; ++i, --ik)
            for (int j = 0; j < 3; ++j) {
                streamIn >> color;
                DownPlane[i][j]->setDownColor(color);
                visualColors[j][0][ik].setColor(2, setRGBColor(color));
            }

    }

    // Вывод развёртки
    void PrintRubikCube(ostream &streamOut = cout) const {

        for (int i = 0; i < 3; ++i) {
            /* streamOut << "\t\t\t\t\t   | "; */ // for "orange", "white" etc.
            streamOut << "\t\t | ";
            for (int j = 0; j < 3; ++j)
                PrintColor(UpPlane[i][j]->UpColor(), streamOut);
            streamOut << "| \n";
        }

        streamOut << "\n | ";
        for (int j = 0; j < 3; ++j)
            PrintColor(LeftPlane[0][j]->LeftColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(FrontPlane[0][j]->FrontColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(RightPlane[0][j]->RightColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(BackPlane[0][j]->BackColor(), streamOut);
        streamOut << "| \n | ";

        for (int j = 0; j < 3; ++j)
            PrintColor(LeftPlane[1][j]->LeftColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(FrontPlane[1][j]->FrontColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(RightPlane[1][j]->RightColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(BackPlane[1][j]->BackColor(), streamOut);
        streamOut << "| \n | ";

        for (int j = 0; j < 3; ++j)
            PrintColor(LeftPlane[2][j]->LeftColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(FrontPlane[2][j]->FrontColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(RightPlane[2][j]->RightColor(), streamOut);
        streamOut << "| ";

        for (int j = 0; j < 3; ++j)
            PrintColor(BackPlane[2][j]->BackColor(), streamOut);
        streamOut << "| " << "\n\n";

        for (int i = 0; i < 3; ++i) {
            /* streamOut << "\t\t\t\t\t   | "; */ // for "orange", "white" etc.
            streamOut << "\t\t | ";
            for (int j = 0; j < 3; ++j)
                PrintColor(DownPlane[i][j]->DownColor(), streamOut);
            streamOut << "| \n";
        }
        streamOut << "\n";

    }

    // *** ВИЗУАЛИЗАЦИЯ ***
    // номер грани для поворота
    int currentPlane;

    void setVisualCube(GLfloat size, unsigned int *color) {
        memset(rotateAngle, 0, sizeof(rotateAngle));
        this->visualSize = size;
        currentPlane = -1;

        // верх
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                visualColors[i][j][2].setColor(0, color[0]);

        // низ
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                visualColors[i][j][0].setColor(1, color[1]);

        // спереди
        for (int k = 0; k < 3; ++k)
            for (int j = 0; j < 3; ++j)
                visualColors[j][0][k].setColor(2, color[2]);

        // сзади
        for (int k = 0; k < 3; ++k)
            for (int j = 0; j < 3; ++j)
                visualColors[j][2][k].setColor(3, color[3]);

        // слева
        for (int i = 0; i < 3; ++i)
            for (int k = 0; k < 3; ++k)
                visualColors[0][k][i].setColor(4, color[4]);

        // справа
        for (int i = 0; i < 3; ++i)
            for (int k = 0; k < 3; ++k)
                visualColors[2][k][i].setColor(5, color[5]);

        // размеры мелких деталей
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k)
                    visualColors[i][j][k].visualSize = (size / 3) * 0.95;

    }

    // отрисовывает кубик на экране
    void draw() {
        const GLfloat K = 0.25;
        // K - куб черного цвета, размер которого равен K*visualSize
        glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(((1 - K) / 2) * visualSize + K * visualSize / 2, ((1 - K) / 2) * visualSize + K * visualSize / 2, ((1 - K) / 2) * visualSize + K * visualSize / 2);
        glutSolidCube(visualSize * K);
        glPopMatrix();

        memset(isVisualCubeUsed, true, sizeof(isVisualCubeUsed));
        if (currentPlane != -1) {
            glPushMatrix();

            if (currentPlane == 0 || currentPlane == 1) {
                int k = (currentPlane & 1) * 2;
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        isVisualCubeUsed[i][j][k] = false;

                glTranslated(visualSize / 2, visualSize / 2, 0);   // сдвигаемся к центру
                glRotatef(rotateAngle[currentPlane], 0, 0, 1);   // крутим
                glTranslated(-visualSize / 2, -visualSize / 2, 0); // сдвигаемся обратно
                // рисуем
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        visualColors[i][j][k].draw(visualSize / 3 * i, visualSize / 3 * j, visualSize / 3 * k);

            } else if (currentPlane == 2 || currentPlane == 3) {
                int j = (currentPlane & 1) * 2;
                for (int i = 0; i < 3; ++i)
                    for (int k = 0; k < 3; ++k)
                        isVisualCubeUsed[i][j][k] = false;

                glTranslated(visualSize / 2, 0, visualSize / 2);
                glRotatef(rotateAngle[currentPlane], 0, 1, 0);
                glTranslated(-visualSize / 2, 0, -visualSize / 2);
                for (int i = 0; i < 3; ++i)
                    for (int k = 0; k < 3; ++k)
                        visualColors[i][j][k].draw(visualSize / 3 * i, visualSize / 3 * j, visualSize / 3 * k);

            } else if (currentPlane == 4 || currentPlane == 5) {
                int i = (currentPlane & 1) * 2;
                for (int j = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        isVisualCubeUsed[i][j][k] = false;

                glTranslated(0, visualSize / 2, visualSize / 2);
                glRotatef(rotateAngle[currentPlane], 1, 0, 0);
                glTranslated(0, -visualSize / 2, -visualSize / 2);
                for (int j = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        visualColors[i][j][k].draw(visualSize / 3 * i, visualSize / 3 * j, visualSize / 3 * k);
            }
            glPopMatrix();
        }

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k)
                    if (isVisualCubeUsed[i][j][k])
                        visualColors[i][j][k].draw(visualSize / 3 * i, visualSize / 3 * j, visualSize / 3 * k);
    }

    // проекции угла поворота на оси и отдаление
    const GLfloat CUBE_SIZE = 12;
    int xRot = 25, yRot = -45;
    GLfloat translateZ = -35;

    void display() {
        glPushMatrix();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3f(1, 0, 0);
        glTranslatef(0, 0, translateZ);
        glRotatef(xRot, 1, 0, 0);
        glRotatef(yRot, 0, 1, 0);
        glTranslatef(CUBE_SIZE / -2, CUBE_SIZE / -2, CUBE_SIZE / -2);
        draw();
        glPopMatrix();
        glutSwapBuffers();
    }

    void visualRotate90(int planeId, int degree) {
        // если sign = -1, значит крутим 3 раза
        if (degree == -1)
            degree = 3;

        while (degree--) {
            // BLUE
            if (planeId == 0) {
                // копируем повёрнутую на 90 градусов верхнюю/нижнюю грань
                // в массив tmp, затем грани присваиваем tmp
                // и не забываем повернуть каждую деталь этой грани
                for (int i = 0, k = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        tmp[j][2 - i] = visualColors[i][j][k];

                for (int i = 0, k = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j) {
                        tmp[i][j].rotateORb();
                        visualColors[i][j][k] = tmp[i][j];
                    }
            }
                //GREEN
            else if (planeId == 1) {
                for (int i = 0, k = 2; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        tmp[j][2 - i] = visualColors[i][j][k];

                for (int i = 0, k = 2; i < 3; ++i)
                    for (int j = 0; j < 3; ++j) {
                        tmp[i][j].rotateRR();
                        visualColors[i][j][k] = tmp[i][j];
                    }
            }
                //WHITE
            else if (planeId == 2) {
                for (int i = 0, j = 0; i < 3; ++i)
                    for (int k = 0; k < 3; ++k)
                        tmp[k][2 - i] = visualColors[i][j][k];

                for (int i = 0, j = 0; i < 3; ++i)
                    for (int k = 0; k < 3; ++k) {
                        tmp[i][k].rotateGD();
                        visualColors[i][j][k] = tmp[i][k];
                    }
            }
                // YELLOW
            else if (planeId == 3) {
                for (int i = 0, j = 2; i < 3; ++i)
                    for (int k = 0; k < 3; ++k)
                        tmp[k][2 - i] = visualColors[i][j][k];

                for (int i = 0, j = 2; i < 3; ++i)
                    for (int k = 0; k < 3; ++k) {
                        tmp[i][k].rotateGU();
                        visualColors[i][j][k] = tmp[i][k];
                    }
            }
                // GL
                // RED
            else if (planeId == 4) {
                for (int j = 0, i = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        tmp[k][2 - j] = visualColors[i][j][k];

                for (int j = 0, i = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k) {
                        tmp[j][k].rotateGR();
                        visualColors[i][j][k] = tmp[j][k];
                    }
            }
                // GR
                // ORANGE
            else if (planeId == 5) {
                for (int j = 0, i = 2; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        tmp[k][2 - j] = visualColors[i][j][k];

                for (int j = 0, i = 2; j < 3; ++j)
                    for (int k = 0; k < 3; ++k) {
                        tmp[j][k].rotateGR();
                        visualColors[i][j][k] = tmp[j][k];
                    }
            }
        }
    }

    void timer(int degree, int) {
        if (currentPlane != -1)
            display(), visualRotateMiniMachineGun(currentPlane, ROTATE_SPEED_STEP, degree);
    }

    void visualRotateMiniMachineGun(int idx, int angle, int degree) {
        if (currentPlane == -1 || currentPlane == idx) {
            if (degree == -1)
                rotateAngle[idx] -= angle;
            else if (degree == 1)
                rotateAngle[idx] += angle;

            if (rotateAngle[idx] % 90 != 0) {
                currentPlane = idx, timer(degree, 0);
            } else {
                // угол стал кратным 90, поварачиваем на массиве
                if ((rotateAngle[idx] < 0) ^ (currentPlane == 2 || currentPlane == 3))
                    visualRotate90(idx, 1);
                else
                    visualRotate90(idx, -1);

                rotateAngle[idx] = 0;
                currentPlane = -1;
            }
        }
    }

};

RubikCube Cube;

void display() {
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 0, 0);
    glTranslatef(0, 0, translateZ);
    glRotatef(xRot, 1, 0, 0);
    glRotatef(yRot, 0, 1, 0);
    glTranslatef(CUBE_SIZE / -2, CUBE_SIZE / -2, CUBE_SIZE / -2);
    Cube.draw();
    glPopMatrix();
    glutSwapBuffers();
}

void init() {
    // цвет фона
    glClearColor(0.75, 0.75, 0.75, 0.4);

    // освещение
    float matSpecular[] = {0.3, 0.3, 0.3, 0};
    float diffuseLight[] = {0.2, 0.2, 0.2, 1};
    float ambientLight[] = {0.9, 0.9, 0.9, 1};
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // инициализируем куб
    Cube.setVisualCube(CUBE_SIZE, colors);
}

// без трёх аргументов оно не работает
void specialKeys(int key, int, int) {

    if (key == GLUT_KEY_DOWN) {
        xRot -= 10;
        if (xRot >= 360)
            xRot -= 360;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_UP) {
        xRot += 10;
        if (xRot < 0)
            xRot += 360;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_RIGHT) {
        yRot -= 10;
        if (yRot >= 360)
            yRot -= 360;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_LEFT) {
        yRot += 10;
        if (yRot < 0)
            yRot += 360;
        glutPostRedisplay();
    }

    if (key == '1') {Cube.RotateUpPlane('+'), glutPostRedisplay();}

    if (key == '2') {Cube.RotateLeftPlane('+'), glutPostRedisplay();}

    if (key == '3') {Cube.RotateFrontPlane('+'), glutPostRedisplay();}

    if (key == '4') {Cube.RotateRightPlane('+'), glutPostRedisplay();}

    if (key == '5') {Cube.RotateBackPlane('+'), glutPostRedisplay();}

    if (key == '6') {Cube.RotateDownPlane('+'), glutPostRedisplay();}
}

void timer(int) {
    glutTimerFunc(5, timer, 0);
    if (Cube.currentPlane != -1)
        Cube.visualRotateMiniMachineGun(Cube.currentPlane, ROTATE_START_VALUE, -1);

    display();
}

static void SolveCubeArray(int amountOfTests, bool isWriteToConsole) {
    CubeCounter = 0;
    Timer SolvingTime;
    unsigned int solvesCounter = 0, maxRotatesCounter = 0, sumRotatesCounter = 0, minRotatesCounter = 1000, unSilvesCounter = 0;
    cout << endl;

    for (int i = 0; i < amountOfTests; ++i) {
        cout << "Прогресс: " << CubeCounter << "/" << amountOfTests << flush;
        CubeCounter++;
        RubikCube TestCube;
        TestCube.CreateRubikCube();
        TestCube.Shuffle(false);
        TestCube.FindSolution(isWriteToConsole);
        if (TestCube.isCubeCompleted()) {
            solvesCounter++;
            sumRotatesCounter += TestCube.getRotatesCounter();
            maxRotatesCounter = max(maxRotatesCounter, TestCube.getRotatesCounter());
            minRotatesCounter = min(minRotatesCounter, TestCube.getRotatesCounter());
        } else unSilvesCounter++;
        cout << "\b\b\b\b\b\b\b\b\b\b" << flush;
        unsigned long size = to_string(CubeCounter).size() + to_string(amountOfTests).size() + 1;
        while (size--)
            cout << "\b" << flush;
    }

    string result = "Сборка окончена, обработка результатов";
    unsigned  long resultSize = result.size() + 3;
    cout << result << flush;
    sleep(1);
    cout << "." << flush;
    sleep(1);
    cout << "." << flush;
    sleep(1);
    cout << "." << flush;
    sleep(1);
    while (resultSize-- > 0)
        cout << "\b" << flush;

    if (!solvingErrors.empty())
        cout << "Все ошибки при сборке " << amountOfTests << " кубиков:\n";
    for (auto &str : solvingErrors)
        cout << str;

    cout << "\nКубиков собрано: " << solvesCounter << "/" << amountOfTests << " - " << setprecision(5)
         << ((float) solvesCounter / (float) amountOfTests) * 100 << " %\n";
    cout << "\nМаксимальное количество поворотов: " << maxRotatesCounter;
    cout << "\nСреднее количество поворотов: " << sumRotatesCounter / amountOfTests;
    cout << "\nМинимальное количество поворотов: " << minRotatesCounter;
    cout << "\nПроцент ошибок при сборке: " << setprecision(5) << fixed
         << (float) (amountOfTests - solvesCounter) / (float) amountOfTests << "%";

    solvingErrors.clear();
}

// действия в меню кнопок в окне
void processMenu(int action) {
    try {
        switch (action) {
            case 1:
                // Читаем кубик и выводим изначальное состояние
                Cube.ReadRubikCube(inputStream);
                break;

            case 2:
                cout << "\nРазвёртка кубика:\n\n";
                Cube.PrintRubikCube();
                break;

            case 3:
                outputStream << "\nРазвёртка кубика:\n\n";
                Cube.PrintRubikCube(outputStream);
                break;

            case 4:
                // Читаем кубик и выводим изначальное состояние
                Cube.Shuffle(false);
                // Разбираем кубик и выводим текущее состояние
                cout << "\n\nСостояние кубика после разборки:\n\n";
                Cube.PrintRubikCube();
                break;

            case 5:
                // Собираем кубик и выводим шаги сборки в консоль, а сам кубик - в файл
                Cube.FindSolution();
                outputStream << "\n\nКубик после сборки:\n\n";
                Cube.PrintRubikCube(outputStream);
                break;

            case 6:
                Cube.PrintCubeInfo();
                break;

            case 7:
                Cube.LeftUpSolver(false);
                break;

            case 8:
                Cube.RightUpSolver(false);
                break;

            case 9:
                Cube.CreateRubikCube();
                Cube.setVisualCube(CUBE_SIZE, colors);
                break;

            case 10:
                // Делаем тест на n кубиках
                // до этого надо сделать хотя бы одно другое действие
                SolveCubeArray(AmountOfTests, false); // (10кк ~ 3 часа), (1кк ~ 20 минут), (100к ~ 1,5 минуты)
                break;

            case 11:
                // сделать рандомное заполнение цветов (ни разу не собралось)
                // Cube.setRandomColors();
                break;

            default:
                break;
        }

    } catch (...) {}
}

int main(int argc, char *argv[]) {
    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(480, 800);
    glutInitWindowPosition(1, 1);
    glutCreateWindow("Rubik's Cube");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(5, timer, 0);
    glutSpecialFunc(specialKeys);
    createGLUTMenus();
    glutMainLoop();

    return 0;
}
