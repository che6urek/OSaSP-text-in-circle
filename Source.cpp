#include <objidl.h>
#include <gdiplus.h>
#include <math.h>
#include <windows.h>
#include <string>
#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace std;

#define MAX_LOADSTRING 100

#define FONT_SIZE 14
#define FONT_STYLE FontStyleBold
#define FONTS_COUNT 6
#define TYPES_COUNT 6
#define RADIUS_DELTA 15
#define DEFAULT_RADIUS (-115)
#define SPIRAL_RADIUS (-25)
#define SPIRAL_ANGLE 45
#define SPIRAL_OFFSET 6

const wchar_t* fontsList[FONTS_COUNT] = { L"Calibri", L"Anja Eliane",
    L"Lily of the Valley_Personal_Use", L"Arial", L"Courier New", L"Consolas" };
const int outAngles[29] = { 10, 9, 8, 8, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2};
const int inAngles[7] = { 10, 12, 15, 18, 24, 30, 45};

HINSTANCE hInst;
WCHAR title[MAX_LOADSTRING] = L"OSaSP2";
WCHAR windowClass[MAX_LOADSTRING] = L"OSaSP2Class";

RECT rect;
wstring text = L"";
int angle = 0;
int angleIndex = 0;
int radius = DEFAULT_RADIUS;
int type = 0;
int currentStyle = 0;
int offset = 0;

WCHAR fileName[MAX_PATH];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                ShowText(HWND hWnd, const WCHAR* wStr, int len);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) 
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return int(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = windowClass;

    return RegisterClassEx(&wcex); 
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; 

    HWND hWnd = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr); 

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GetClientRect(hWnd, &rect);

    switch (message)
    {
    case WM_PAINT:
    {
        angle = 0;
        radius = type == 2 | type == 3 ? SPIRAL_RADIUS : DEFAULT_RADIUS;
        angleIndex = 0;
        offset = 0;
        ShowText(hWnd, text.c_str(), wcslen(text.c_str()));
        break;
    }
    case WM_CHAR:
    {
        if (wParam != 8)
        {
            InvalidateRect(hWnd, nullptr, false);
            text += wParam;
            const WCHAR wStr = wParam;
            ShowText(hWnd, &wStr, 1);
        }
        else
            if (text.length() > 0)
            {
                text.pop_back();
                InvalidateRect(hWnd, nullptr, true);
            }
        break;
    }
    case WM_KEYDOWN:
        switch (wParam)
        {
            case VK_F1:
                currentStyle = ++currentStyle % FONTS_COUNT;
                InvalidateRect(hWnd, nullptr, true);
                break;
            case VK_F2:
                type = ++type % TYPES_COUNT;
                InvalidateRect(hWnd, nullptr, true);
                break;
            case VK_F3:
                text = L"";
                InvalidateRect(hWnd, nullptr, true);
                break;
            case VK_F4:
                text = wstring(226, 'W');
                InvalidateRect(hWnd, nullptr, true);
                break;
            case VK_F5:
                text = wstring(1030, '@');
                InvalidateRect(hWnd, nullptr, true);
                break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ShowText(HWND hWnd, const WCHAR* str, int len)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    GetClientRect(hWnd, &rect);
    graphics.TranslateTransform((rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);

    Font font(&FontFamily(fontsList[currentStyle]), FONT_SIZE, FONT_STYLE);
    SolidBrush brush(Color::Black);
    for (auto i = 0; i < len; i++)
    {
        switch (type)
        {
            case 0:
                angle += outAngles[0];
                if (angle > 360)
                    return;
                break;
            case 1:
                angle += outAngles[0];
                if (angle > 360)
                {
                    radius -= RADIUS_DELTA;
                    angle = outAngles[0];                
                }
                break;
            case 2:
            case 3:
                angle += SPIRAL_ANGLE;
                if (angle > 360 + offset)
                {
                    offset += type == 2 ? SPIRAL_OFFSET : -SPIRAL_OFFSET;
                    radius -= RADIUS_DELTA;
                    angle = SPIRAL_ANGLE + offset;
                }
                break;
            case 4:
                angle += outAngles[angleIndex];
                if (angle > 360)
                {
                    radius -= RADIUS_DELTA;
                    if (angleIndex < 28)
                        angleIndex++;
                    angle = outAngles[angleIndex];
                }
                break;
            case 5:
                angle += inAngles[angleIndex];
                if (angle > 360)
                {
                    if (angleIndex < 6)
                    {
                        angleIndex++;
                        radius += RADIUS_DELTA;
                    }
                    else
                        return;
                    angle = inAngles[angleIndex];
                }
                break;
            
        }
        graphics.RotateTransform(angle);
        graphics.DrawString(&str[i], 1, &font, PointF(0, radius), &brush);
        graphics.RotateTransform(-angle);
    }
    EndPaint(hWnd, &ps);
}