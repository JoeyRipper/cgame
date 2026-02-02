#include <windows.h>
#include <stdint.h>

// term - definition
#define internal static
#define local_persist static
#define global_variable static

// common typedefs
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// global - likely to change in the future.
// statics always initialize to 0
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
// temp globals
global_variable int BitmapWidth;
global_variable int BytesPerPixel = 4;
global_variable int BitmapHeight;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;
    int Pitch = Width * BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0; X < BitmapWidth; X++)
        {
            // Pixel in memory: BB GG RR xx
            // 0x xxBBGGRR
            
            uint8 Blue = (X+XOffset);
            uint8 Green = (Y+YOffset);
            uint8 Red = 0; 

            /*
            Memory:    BB GG RR xx
            Register:  xx RR GG BB

            Pixel (32-bit)
            */

            // increments after operation
            *Pixel++ = ((Red << 16) | (Green << 8) | Blue); // Register
        }

        Row += Pitch;
    }
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Width * Height) * 4;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // TODO: Clear this to black
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top; 
    StretchDIBits(
        DeviceContext,
        0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS, SRCCOPY);
}

// Creates and handles window
LRESULT CALLBACK
Win32MainWindowCallback(
    HWND Window,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);

            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            // TODO: Handle this as an error - recreate window?
            Running = false;
        } break;

        case WM_CLOSE:
        {
            // TODO: Handle this with a message - Are you sure you want to leave?
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left; // Upper left hand corner of window
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left; // px num of right side of window - left side of window
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top; // px num of bottom - top of window

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            PatBlt(DeviceContext, X, Y, Width, Height, BLACKNESS);
            Win32UpdateWindow(DeviceContext, &ClientRect);
        } break;

        default:
        {
//            OutputDebugStringA("default");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return(Result);
}

// main program entry
int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CmdLine,
    int CmdShow)
{
    WNDCLASS WindowClass = {0};

    // makes the window resizable, and prevents constant passing of data for repetitive redrawing; may be legacy
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;

    // Window procedure
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    
    // Instance for the window procedure
    WindowClass.hInstance = Instance;
    
    // Sets the window icon
//    WindowClass.hIcon;

    // Sets the class name
    WindowClass.lpszClassName = TEXT("C_gameWindowClass");
    

    if (RegisterClass(&WindowClass))
    {
        HWND Window= CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            TEXT("C game"),
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);
        if (Window)
        {
            Running = true;
            int XOffset = 0;
            int YOffset = 0;
            while (Running)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                RenderWeirdGradient(XOffset, YOffset);
                
                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect);
                ReleaseDC(Window, DeviceContext);
                XOffset++;
            }
        }
        else
        {
            // TODO: Log
        }
    }
    else
    {
        // TODO: Log
    }

    return(0);
}