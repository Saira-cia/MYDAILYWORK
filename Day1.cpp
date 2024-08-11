#include <windows.h>
#include <string>

// Global variables
HWND hDisplay;  // Handle to the display (edit control)
std::string currentInput = "";
std::string previousInput = "";
char lastOperator = 0;

// Function to perform calculation
double Calculate(double leftOperand, double rightOperand, char oper) {
    switch (oper) {
        case '+': return leftOperand + rightOperand;
        case '-': return leftOperand - rightOperand;
        case '*': return leftOperand * rightOperand;
        case '/': return leftOperand / rightOperand;
    }
    return 0.0;
}

// Callback function to process messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Create display (edit control)
            hDisplay = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_RIGHT,
                                    10, 10, 260, 40, hwnd, NULL, NULL, NULL);

            // Create buttons
            const char *buttons[] = {
                "7", "8", "9", "/",
                "4", "5", "6", "*",
                "1", "2", "3", "-",
                "0", "C", "=", "+"
            };
            int x = 10, y = 60;
            for (int i = 0; i < 16; i++) {
                CreateWindow("BUTTON", buttons[i], WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                             x, y, 60, 40, hwnd, (HMENU)(i + 1), GetModuleHandle(NULL), NULL);
                x += 70;
                if ((i + 1) % 4 == 0) {
                    x = 10;
                    y += 50;
                }
            }
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (wmId >= 1 && wmId <= 16) {
                char buttonText[2];
                GetWindowText((HWND)lParam, buttonText, 2);

                if (isdigit(buttonText[0])) {
                    // If it's a digit, append to the current input
                    currentInput += buttonText;
                    SetWindowText(hDisplay, currentInput.c_str());
                } else if (buttonText[0] == 'C') {
                    // Clear the inputs
                    currentInput.clear();
                    previousInput.clear();
                    lastOperator = 0;
                    SetWindowText(hDisplay, "");
                } else if (buttonText[0] == '=') {
                    if (!currentInput.empty() && !previousInput.empty()) {
                        // Perform calculation
                        double leftOperand = std::stod(previousInput);
                        double rightOperand = std::stod(currentInput);
                        double result = Calculate(leftOperand, rightOperand, lastOperator);
                        currentInput = std::to_string(result);
                        SetWindowText(hDisplay, currentInput.c_str());
                        previousInput.clear();
                        lastOperator = 0;
                    }
                } else {
                    // It's an operator (+, -, *, /)
                    if (!currentInput.empty()) {
                        if (!previousInput.empty() && lastOperator) {
                            // Perform intermediate calculation
                            double leftOperand = std::stod(previousInput);
                            double rightOperand = std::stod(currentInput);
                            double result = Calculate(leftOperand, rightOperand, lastOperator);
                            previousInput = std::to_string(result);
                        } else {
                            previousInput = currentInput;
                        }
                        currentInput.clear();
                        lastOperator = buttonText[0];
                    }
                }
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return 0;
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char CLASS_NAME[] = "CalculatorWindowClass";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Simple Calculator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}