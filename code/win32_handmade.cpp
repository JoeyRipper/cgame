
#include <windows.h>

// Creates and handles window
LRESULT CALLBACK
MainWindowCallback(
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
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
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
            static DWORD Operation = WHITENESS; // creates persistent local varibale
            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
            // Alternates coloredness of 'Operation' every time this case exists. Creates epilepsy effect for debugging.
            if (Operation == WHITENESS)
            {
                Operation = BLACKNESS;
            }
            else
            {
                Operation = WHITENESS;
            }
            EndPaint(Window, &Paint); 

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
    WindowClass.lpfnWndProc = MainWindowCallback;
    
    // Instance for the window procedure
    WindowClass.hInstance = Instance;
    
    // Sets the window icon
//    WindowClass.hIcon;

    // Sets the class name; LSTR casts as typedef in windows
    WindowClass.lpszClassName = L"C_gameWindowClass";
    

    if (RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            L"C game",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0
        );
        if (WindowHandle)
        {
            MSG Message;
            for (;;)
            {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
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