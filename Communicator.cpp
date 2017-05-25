// Communicator.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Communicator.h"
#include <string>
#include <winsock2.h>
#pragma comment( lib, "Ws2_32.lib" )

#define MAX_LOADSTRING 100
#define ID_PRIMARY 99
#define ID_EDITCHILD 100
#define ID_WRITECHILD 101
#define ID_START_PORT 102
#define ID_CONNECT 203
#define ID_CONNECT_PORT 204
#define BUTTON_ID      1001
#define BUTTON_CONNECT_ID      1002
#define BUTTON_START_ID      1003

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool connected = false;
SOCKET sGlobal;
HWND hWndG;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void printToTextFieldSelf(HWND hWnd, std::wstring text);
void printToTextFieldRec(HWND hWnd, std::wstring text);
DWORD WINAPI odbieranie(void *argumenty);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COMMUNICATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COMMUNICATOR));

    MSG msg;

	SOCKET s;
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	closesocket(sGlobal);
	WSACleanup();
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COMMUNICATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COMMUNICATOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, CW_USEDEFAULT, 400, 500, nullptr, nullptr, hInstance, nullptr);
   RECT rect;
   int width;
   int height;
   if (GetWindowRect(hWnd, &rect))
   {
	   width = rect.right - rect.left;
	   height = rect.bottom - rect.top;
   }
   hWndG = hWnd;
   HWND hWndEdit = CreateWindowEx(0,
	   L"EDIT",
	   0,
	   WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
	   0,
	   height*0.7,
	   width*0.72,
	   height*0.18,
	   hWnd,
	   (HMENU)ID_EDITCHILD,
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   0);
   SendMessage(hWndEdit, EM_SETLIMITTEXT, 100, NULL);
   HWND hWndEdit2 = CreateWindowEx(0,
	   L"EDIT",
	   0,
	   WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
	   0,
	   0,
	   width*0.72,
	   height*0.7,
	   hWnd,
	   (HMENU)ID_WRITECHILD,
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   0);

   HWND hwndButton = CreateWindow(
	   L"BUTTON",  // Predefined class; Unicode assumed 
	   L"SEND",      // Button text 
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
	   width*0.75,         // x position 
	   height*0.75,         // y position 
	   80,        // Button width
	   40,        // Button height
	   hWnd,     // Parent window
	   (HMENU)BUTTON_ID,       // No menu.
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   NULL);
   HWND hwndButtonConnect = CreateWindow(
	   L"BUTTON",  // Predefined class; Unicode assumed 
	   L"CONNECT",      // Button text 
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
	   width*0.75,         // x position 
	   height*0.25,         // y position 
	   80,        // Button width
	   40,        // Button height
	   hWnd,     // Parent window
	   (HMENU)BUTTON_CONNECT_ID,       // No menu.
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   NULL);
   HWND hWndConnect = CreateWindowEx(0,
	   L"EDIT",
	   0,
	   WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
	   width*0.75,         // x position 
	   height*0.05,         // y position 
	   80,        // Button width
	   40,
	   hWnd,
	   (HMENU)ID_CONNECT,
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   0);
   HWND hWndConnectPort = CreateWindowEx(0,
	   L"EDIT",
	   0,
	   WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL | ES_NUMBER,
	   width*0.75,         // x position 
	   height*0.15,         // y position 
	   80,        // Button width
	   40,
	   hWnd,
	   (HMENU)ID_CONNECT_PORT,
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   0);
   HWND hwndButtonStart = CreateWindow(
	   L"BUTTON",  // Predefined class; Unicode assumed 
	   L"RECEIVE",      // Button text 
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
	   width*0.75,         // x position 
	   height*0.45,         // y position 
	   80,        // Button width
	   40,        // Button height
	   hWnd,     // Parent window
	   (HMENU)BUTTON_START_ID,       // No menu.
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   NULL);
   HWND hWndStart = CreateWindowEx(0,
	   L"EDIT",
	   0,
	   WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL | ES_NUMBER,
	   width*0.75,         // x position 
	   height*0.35,         // y position 
	   80,        // Button width
	   40,
	   hWnd,
	   (HMENU)ID_START_PORT,
	   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	   0);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdcedit;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case BUTTON_ID:
			{
				hdcedit = GetDC(hWnd);
				const HWND hWndEdit = GetDlgItem(hWnd, ID_EDITCHILD);
				const HWND hWndEdit2 = GetDlgItem(hWnd, ID_WRITECHILD);
				int n = GetWindowTextLength(hWndEdit);
				std::wstring text(n + 1, L'#');
				GetWindowText(hWndEdit, (LPWSTR)&text[0], n + 1);
				if (connected) {
					const WCHAR * sendText = text.c_str();
					send(sGlobal, (char *)sendText, wcslen(sendText) * sizeof(wchar_t), 0);
				}
				printToTextFieldSelf(hWnd, text);
				SetWindowText(hWndEdit, L"");
				UpdateWindow(hWnd);
			}
			break;
			    /*
			case BUTTON_CONNECT_ID:
			{
				const HWND connectButton = GetDlgItem(hWnd, BUTTON_CONNECT_ID);
				const HWND connectT = GetDlgItem(hWnd, ID_CONNECT);
				const HWND connectPort = GetDlgItem(hWnd, ID_CONNECT_PORT);		
				int n = GetWindowTextLength(connectPort);
				std::wstring input(n + 1, L'#');
				GetWindowText(connectPort, &input[0], n + 1);
				int port = std::stoi(input);
				n = GetWindowTextLength(connectT);
				std::string input2(n + 1, L'#');
				GetWindowTextA(connectT, &input2[0], n + 1);
				struct sockaddr_in sa;
				SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
				memset((void *)(&sa), 0, sizeof(sa));
				sa.sin_family = AF_INET;
				sa.sin_port = htons(port);				
				sa.sin_addr.s_addr = inet_addr(input2.c_str());
				int result;

				result = connect(s, (struct sockaddr FAR *) &sa, sizeof(sa));
				if (result != SOCKET_ERROR)
				{
					sGlobal = s;
					connected = true;
					ShowWindow(connectButton, SW_HIDE);
					ShowWindow(connectT, SW_HIDE);
					ShowWindow(connectPort, SW_HIDE);
					MessageBox(NULL, L"Connected", L"", 0);

				}
			}
			break; */
	
			case BUTTON_START_ID:
			{
				DWORD id;
				int port = 9000;
				const HWND startButton = GetDlgItem(hWnd, BUTTON_START_ID);
				const HWND startPort = GetDlgItem(hWnd, ID_START_PORT);
				ShowWindow(startButton, SW_HIDE);
				ShowWindow(startPort, SW_HIDE);
				int n = GetWindowTextLength(startPort);
				std::wstring input(n + 1, L'#');
				GetWindowText(startPort, &input[0], n + 1);					
				port = std::stoi(input);
				CreateThread(NULL, 0, odbieranie, (void*)port, 0, &id);
			}
			break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            EndPaint(hWnd, &ps);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void printToTextFieldSelf(HWND hWnd, std::wstring text)
{
	const HWND hWndEdit2 = GetDlgItem(hWnd, ID_WRITECHILD);
	int nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)L"You: ");
	nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)((LPSTR)&text[0]));
	nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)L"\n");
}

void printToTextFieldRec(HWND hWnd, std::wstring text)
{
	const HWND hWndEdit2 = GetDlgItem(hWnd, ID_WRITECHILD);
	int nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)L"Received: ");
	nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)((LPSTR)&text[0]));
	nLast = GetWindowTextLength(hWndEdit2);
	SendMessage(hWndEdit2, EM_SETSEL, (WPARAM)nLast, (LPARAM)nLast);
	SendMessage(hWndEdit2, EM_REPLACESEL, 0, (LPARAM)L"\n");
}
DWORD WINAPI odbieranie(void *argumenty)
{
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);	
	struct sockaddr_in sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons((int)argumenty);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);	
	int result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	result = listen(s, 1);
	SOCKET si;
	struct sockaddr_in sc;
	int lenc;	
	for (;;)
	{

		lenc = sizeof(sc);
		si = accept(s, (struct sockaddr FAR *) &sc, &lenc);
		char buf[1024] = "";


		while (true)
		{
			int len = recv(si, buf, 1024, 0);
			if (len <= 0)break;
			buf[len] = '\0';			
			std::wstring rectxt((const WCHAR *)buf);
			printToTextFieldRec(hWndG, rectxt);

		};
	}
}
