#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "Dwmapi.lib")
#include <windows.h>
#include <d3d9.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>  

int s_width = 800;
int s_height = 600;
#define CENTERX (GetSystemMetrics(SM_CXSCREEN)/2)-(s_width/2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN)/2)-(s_height/2)
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;  
HWND hWnd;
const MARGINS  margin = {0,0,s_width,s_height};
LPD3DXFONT pFont;  


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;     // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = s_width;    // set the width of the buffer
    d3dpp.BackBufferHeight =s_height;    // set the height of the buffer

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

	D3DXCreateFont(d3ddev, 15, 0, 550, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
				   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);

}

void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, char *fmt, ...)
{
    RECT FontPos = { x, y, x + 120, y + 16 };
    char buf[1024] = {'\0'};
    va_list va_alist;

    va_start(va_alist, fmt);
    vsprintf_s(buf, fmt, va_alist);
    va_end(va_alist);
    g_pFont->DrawText(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void render(int x, int y, bool minimized, char *filename = "NULL")
{
    // clear the window alpha
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0, 0, 0), 1.0f, 0);
    d3ddev->BeginScene();    // begins the 3D scene

	char *logtxt = new char();
	logtxt = "";

	if (strcmp(filename,"NULL")==0)
		DrawString(x-400,y-150,D3DCOLOR_ARGB(255,255,255,255),pFont,"Test rendering string :D");
	else
	{

		std::string line;
		std::ifstream f (filename);
		if (f.is_open())
		{
			while (f.good())
			{
			  std::getline(f, line);
			}
			f.close();
		}
		else 
		{  
			std::cout << "Unable to open file"; 
		}

		DrawString(x-400,y-150,D3DCOLOR_ARGB(255,255,255,255),pFont,&line[0]);

	}
    d3ddev->EndScene();    // ends the 3D scene
    d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}



char* ReadINI(char* szSection, char* szKey, const char* szDefaultValue)
{
	char* szResult = new char[255];
	memset(szResult, 0x00, 255);
	GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, 255, ".\\conf.ini"); 
	return szResult;
}


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	char * value = ReadINI("CONFIGURATION","WIN_NAME","none");
    RECT rc;

	HWND newhwnd = FindWindow(NULL,value);
	
	if(newhwnd!=NULL)
	{
		GetWindowRect(newhwnd,&rc);
		s_width=  rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
		ExitProcess(0);
	}

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)RGB(0,0,0);
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(0,
                          "WindowClass",
                          "",
                          WS_EX_TOPMOST | WS_POPUP  ,
                          rc.left, rc.top,
                          s_width, s_height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

	SetWindowLong(hWnd, GWL_EXSTYLE,(int)GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED |WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

    ShowWindow(hWnd, nCmdShow);


    initD3D(hWnd);
    MSG msg;
	::SetWindowPos(FindWindow(NULL,value) ,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    int x = 0;
	while(TRUE)
    {

		::SetWindowPos(hWnd ,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

		if(!FindWindow(NULL,value))
			ExitProcess(0);
		if (x < 100)
			render(s_width, s_height, false);
		else
			render(s_width, s_height, false, "chatlog.txt");
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		x++;

        if(msg.message == WM_QUIT)
            exit(0);
	}

    return msg.wParam;
}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
		/**case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			// shrink the application to the notification area
			// ...
			render(true);
		}
    break;**/
		case WM_PAINT:
		{	
			DwmExtendFrameIntoClientArea(hWnd, &margin);  
        }
	break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
	break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}  