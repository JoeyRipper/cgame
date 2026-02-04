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

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

// global - likely to change in the future.
// statics always initialize to 0
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension
win32_GetWindowDimension(HWND Window)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    win32_window_dimension Result;
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return(Result);
};

internal void
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
    uint8 *Row = (uint8 *)Buffer->Memory;
    for (int Y = 0; Y < Buffer->Height; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0; X < Buffer->Width; X++)
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
            // initialized to 0x00000000 already; global_variable
            *Pixel++ = ((Red << 16) | (Green << 8) | Blue); // Register
        }

        Row += Buffer->Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    // if the pointer as a value: free it.
    if (Buffer->Memory)
    {
        // Windows version of free: pairs with VirtualAlloc()
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
    // VirtualAlloc is already initialized to 0
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer Buffer)
{
    StretchDIBits(DeviceContext,
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer.Width, Buffer.Height,
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
//            win32_window_dimension Dimension = win32_GetWindowDimension(Window);
//            Win32ResizeDIBSection(&GlobalBackbuffer, Dimension.Width, Dimension.Height);

            // OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            // TODO: Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

        case WM_CLOSE:
        {
            // TODO: Handle this with a message - Are you sure you want to leave?
            GlobalRunning = false;
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

            win32_window_dimension Dimension = win32_GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer);
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
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow)
 {
    WNDCLASS WindowClass = {0};

    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

    // makes the window resizable, and prevents constant passing of data for repetitive redrawing; may be legacy
    WindowClass.style = CS_OWNDC|CS_HREDRAW;

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
        HWND Window = CreateWindowEx(
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
            GlobalRunning = true;
            int XOffset = 0;
            int YOffset = 0;
            while (GlobalRunning)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
                XOffset++;
                YOffset += 2;

                HDC DeviceContext = GetDC(Window);

                win32_window_dimension Dimension = win32_GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer);

                ReleaseDC(Window, DeviceContext);
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