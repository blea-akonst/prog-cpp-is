#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

const unsigned long maxWordSize = 1;

void PrintColor(const string &color, ostream &outputStream = cout) {
    string result(maxWordSize, ' '), strColor;

    unsigned long lengthField = result.size() + 1, step = (lengthField - color.size()) / 2;

    strColor = color;
    copy(strColor.begin(), strColor.end(), result.begin() + step);

    outputStream << result << " ";
}

class Timer {
public:

    Timer() {
        start = chrono::high_resolution_clock::now();
        cout << endl;
    }

    ~Timer() {
        end = chrono::high_resolution_clock::now();
        chrono::duration<float> duration = end - start;

        int durationMinutes = (int) duration.count() / 60;
        int durationSeconds = (int) ((duration.count() / 60) * ((float) 6 / 10) * 100 - 3) % 60;
        string resultDuration = "";
        if (durationMinutes != 0)
            resultDuration += to_string(durationMinutes) + " мин. ";

        cout << "\n\nВремя выполнения программы: " << resultDuration << setprecision(0) << durationSeconds << " сек.";
    }

private:
    chrono::time_point<chrono::steady_clock> start, end;
};

int setRGBColor(string color) {
    int result = 0;

    if (color[0] == 'G' || color[0] == 'g')
        result = 0x32CD32;
    else if (color[0] == 'B' || color[0] == 'b')
        result = 0x0000FF;
    else if (color[0] == 'Y' || color[0] == 'y')
        result = 0xFFD700;
    else if (color[0] == 'W' || color[0] == 'w')
        result = 0xFFFFFF;
    else if (color[0] == 'O' || color[0] == 'o')
        result = 0xFF8C00;
    else if (color[0] == 'R' || color[0] == 'r')
        result = 0xFF0000;

    return result;
}
