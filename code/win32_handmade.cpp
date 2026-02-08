#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <Xinput.h>

// term - definition
#define internal static
#define local_persist static
#define global_variable static

// common typedefs
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

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

struct win32_window_dimension
{
    int Width;
    int Height;
};

// global - likely to change in the future.
// statics always initialize to 0
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;


#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
    // TODO: log
    HMODULE XInputLibrary = LoadLibrary(TEXT("xinput1_4.dll"));
    if (XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if (!XInputGetState) {XInputGetState = XInputGetStateStub;}

        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if (!XInputSetState) {XInputSetState = XInputSetStateStub;}
    }
    else
    {
        // TODO: log
    }
}

internal win32_window_dimension
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
Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer *Buffer)
{
    StretchDIBits(DeviceContext,
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK
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

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;

            // gets the value of the value of the 30th bit
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
            if (IsDown == WasDown)
            {
                break;
            }

            if (VKCode == 'W')
            {

            }
            else if (VKCode == 'A')
            {

            }
            else if (VKCode == 'S')
            {

            }
            else if (VKCode == 'D')
            {

            }
            else if (VKCode == 'E')
            {

            }
            else if (VKCode == 'Q')
            {

            }
            else if (VKCode == VK_SPACE)
            {

            }
            else if (VKCode == VK_UP)
            {

            }
            else if (VKCode == VK_DOWN)
            {

            }
            else if (VKCode == VK_LEFT)
            {

            }
            else if (VKCode == VK_RIGHT)
            {

            }
            else if (VKCode == VK_ESCAPE)
            {
                OutputDebugStringA("ESC: ");
                if (IsDown)
                {
                    OutputDebugStringA("IsDown ");
                }
                if (WasDown)
                {
                    OutputDebugStringA("WasDown");
                }
                OutputDebugStringA("\n");
            }
            
            // Ensure Alt + F4 Works
            bool AltKeyWasDown = ((LParam & (1 << 29)) != 0);
            if ((VKCode == VK_F4) && AltKeyWasDown)
            {
                GlobalRunning = false;
            }
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
            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer);
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
    Win32LoadXInput();

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
            // Init Xaudio2 obj
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

                for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ControllerIndex++)
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        // NOTE: Controller is connected
                        // TODO: See if ControllerState.dwPacketNumber increments too fast
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                        
                        bool Up =               (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down =             (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left =             (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right =            (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start =            (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back =             (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftThumb =        (Pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                        bool RightThumb =       (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                        bool LeftShoulder =     (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder =    (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton =          (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton =          (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton =          (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton =          (Pad->wButtons & XINPUT_GAMEPAD_Y);
                        
                        int16 LStickX = Pad->sThumbLX;
                        int16 LStickY = Pad->sThumbLY;
                        int16 RStickX = Pad->sThumbRX;
                        int16 RStickY = Pad->sThumbRY;

                        uint8 LTrigger = Pad->bLeftTrigger;
                        uint8 RTrigger = Pad->bRightTrigger;

                        XOffset += LStickX >> 12;
                        YOffset -= LStickY >> 12;
                    }
                    else
                    {
                        // NOTE: Controller not connected 
                    }
                }

//                 XINPUT_VIBRATION Vibration;
//                Vibration.wLeftMotorSpeed = 60000;
//                Vibration.wRightMotorSpeed = 60000;
//                XInputSetState(0, &Vibration);

                RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
//                XOffset++;
//                YOffset += 2;

                HDC DeviceContext = GetDC(Window);

                win32_window_dimension Dimension = win32_GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer);

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