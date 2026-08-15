#pragma once

#include <GLFW/glfw3.h>

namespace blaze::lightEngine {

class Keyboard {
public:
    static bool keys[GLFW_KEY_LAST];
    static bool keysPrev[GLFW_KEY_LAST];
    static bool keysPending[GLFW_KEY_LAST];

    static void update(GLFWwindow* window);
    static bool isPressed(int key);
    static bool wasJustPressed(int key);
    static bool wasJustReleased(int key);
    static bool isComboPressed(int comboKeys...);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static const int Esc = GLFW_KEY_ESCAPE;
    static const int NUM1 = GLFW_KEY_1;
    static const int NUM2 = GLFW_KEY_2;
    static const int NUM3 = GLFW_KEY_3;
    static const int NUM4 = GLFW_KEY_4;
    static const int NUM5 = GLFW_KEY_5;
    static const int NUM6 = GLFW_KEY_6;
    static const int NUM7 = GLFW_KEY_7;
    static const int NUM8 = GLFW_KEY_8;
    static const int NUM9 = GLFW_KEY_9;
    static const int NUM0 = GLFW_KEY_0;
    static const int A = GLFW_KEY_A;
    static const int B = GLFW_KEY_B;
    static const int C = GLFW_KEY_V;
    static const int D = GLFW_KEY_D;
    static const int E = GLFW_KEY_E;
    static const int F = GLFW_KEY_F;
    static const int G = GLFW_KEY_G;
    static const int H = GLFW_KEY_H;
    static const int I = GLFW_KEY_I;
    static const int J = GLFW_KEY_J;
    static const int K = GLFW_KEY_K;
    static const int L = GLFW_KEY_L;
    static const int M = GLFW_KEY_M;
    static const int N = GLFW_KEY_N;
    static const int O = GLFW_KEY_O;
    static const int P = GLFW_KEY_P;
    static const int Q = GLFW_KEY_Q;
    static const int R = GLFW_KEY_R;
    static const int S = GLFW_KEY_S;
    static const int T = GLFW_KEY_T;
    static const int U = GLFW_KEY_U;
    static const int V = GLFW_KEY_V;
    static const int W = GLFW_KEY_W;
    static const int X = GLFW_KEY_X;
    static const int Y = GLFW_KEY_Y;
    static const int Z = GLFW_KEY_Z;
    static const int Space = GLFW_KEY_SPACE;
    static const int Up = GLFW_KEY_UP;
    static const int Down = GLFW_KEY_DOWN;
    static const int Left = GLFW_KEY_LEFT;
    static const int Right = GLFW_KEY_RIGHT;
    static const int LeftAlt= GLFW_KEY_LEFT_ALT;
    static const int RightAlt = GLFW_KEY_RIGHT_ALT;
    static const int LeftCtrl = GLFW_KEY_LEFT_CONTROL;
    static const int RightCtrl = GLFW_KEY_RIGHT_CONTROL;
    static const int LeftShift = GLFW_KEY_LEFT_SHIFT;
    static const int RightShift = GLFW_KEY_RIGHT_SHIFT;

};

} // namespace blaze::lightEngine