// Lab03.cpp : Defines the entry point for the application.
/*
	Trong chương trình này, ta sẽ cố gắng lưu lại bản vẽ sau khi bản vẽ. 
	Việc vẽ các đường thẳng tạm sẽ không được vẽ trực tiếp lên bản vẽ này.
	Ý tưởng: sử dụng một bitmap để lưu bản vẽ hiện hành. Việc vẽ các đường thẳng tạm sẽ được vẽ trực tiếp lên màn hình.
			khi kết thúc việc vẽ thì ta mới vẽ đường thẳng lên bitmap
	Đối với kỹ thuật này thì khi vẽ đường thẳng ta không cần vẽ đè đường
*/

#include "stdafx.h"
#include "Lab03.h"
#include "Windowsx.h"

#define MAX_LOADSTRING 100
#define ID_TIMER1	123123

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT p1, p2, p2_ancien;
HBITMAP hBitmap;
HWND hwndDialog;
BOOL enTrainDessin;
HBRUSH hbrBackground;
//====================================na=====================
TCHAR buff[1024];
//===========================================================

int mode; //mode de dessin==> 0 pour ligne, 1 pour libre
int size = 1;
int color1 = 0;
int color2 = 0;
int color3 = 0;
bool draw;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//================================================na===========
INT_PTR CALLBACK	Text(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//===========================================================

VOID				BackgroudColorDialog(HWND);

VOID				DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawCurveProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawEllipseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawRoundProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

VOID				DrawLine(HDC hdc);
VOID				FreeDraw(HDC hdc);
VOID				DrawCurve(HDC hdc);
VOID				DrawRectangle(HDC hdc);
VOID				DrawSquare(HDC hdc);
VOID				DrawEllipse(HDC hdc);
VOID				DrawRound(HDC hdc);

VOID CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,HBITMAP hBMP, HDC hDC);
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
void errhandler(LPCTSTR, HWND);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB03, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB03));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB03));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	hbrBackground = wcex.hbrBackground;
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZEBOX| WS_MINIMIZEBOX| WS_SYSMENU,
      0, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
	HDC hdc = GetDC(hWnd);
	PBITMAPINFO pbmi;
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	switch (mode)
	{
		case 0:
			FreeDrawProc(hWnd, message, wParam, lParam);
			break;
		case 1:
			DrawLineProc(hWnd, message, wParam, lParam);
			break;
		case 4:
			DrawRectangleProc(hWnd, message, wParam, lParam);
			break;
		case 5:
			DrawSquareProc(hWnd, message, wParam, lParam);
			break;
		case 6:
			DrawEllipseProc(hWnd, message, wParam, lParam);
			break;
		case 7:
			DrawRoundProc(hWnd, message, wParam, lParam);
			break;
		case 8:
			DrawCurveProc(hWnd, message, wParam, lParam);
			break;
		default:
			break;
	}
	
	switch (message)
    {
		case WM_KEYDOWN:
			if(wParam == 65){
				MessageBox(hWnd,_T("KeyStroke: A"),_T("Notification"),MB_OK);
			}
			break;

		case WM_CREATE:
		{
			p1.x = p1.y = 0;
			p2 = p1;
			mode = 100;
			draw = false;
			enTrainDessin = FALSE;

			//Tạo bitmap
			HDC hdc = GetDC(hWnd);
			RECT rect;
			GetClientRect(hWnd, &rect);
			//Tạo ra một bitmap tương thích với DC màn hình
			hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

			//Tô background cho bitmap
			//Tạo memory dc để tương tác với Bitmap
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			FillRect(memDC, &rect, hBrush);
			DeleteObject(hBrush);
			DeleteDC(memDC);
			ReleaseDC(hWnd, hdc);
			break;
		}

		case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
				case ID_LINE_FREESTYTE:
					mode = 0;
					break;
				case ID_LINE_STRAIGHTLINE:
					mode = 1;
					break;
				case ID_SHAPE_RECTANGLE:
					mode = 4;
					break;
				case ID_SHAPE_SQUARE:
					mode = 5;
					break;
				case ID_SHAPE_ELLIPSE:
					mode = 6;
					break;
				case ID_SHAPE_ROUND:
					mode = 7;
					break;
				case ID_COLOR_RED:
					color1 = 255;
					color2 = 0;
					color3 = 0;
					break;
				case ID_COLOR_BLUE:
					color1 = 0;
					color2 = 0;
					color3 = 255;
					break;
				case ID_COLOR_GREEN:
					color1 = 0;
					color2 = 255;
					color3 = 0;
					break;
				case ID_WIDTH_5:
					size = 5;
					break;
				case ID_WIDTH_10:
					size = 10;
					break;
				case ID_WIDTH_15:
					size = 15;
					break;
				case ID_LINE_CURVE:
					mode = 8;
					break;

				case ID_FILE_SAVE:
					pbmi = CreateBitmapInfoStruct(hWnd, hBitmap);
					CreateBMPFile(hWnd, _T("BITMAP.bmp"), pbmi, hBitmap, hdc);
					draw = true;
					break;

				case ID_FILE_EXIT:
					if (draw)
					{
						if (MessageBox(hWnd, _T("Are you sure to close?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
						{
							DestroyWindow(hWnd);
						}
					}
					else
					{
						if (MessageBox(hWnd, _T("Do you want to save?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == 1)
						{
							pbmi = CreateBitmapInfoStruct(hWnd, hBitmap);
							CreateBMPFile(hWnd, _T("BITMAP.bmp"), pbmi, hBitmap, hdc);
							DestroyWindow(hWnd);
						}
						else
							DestroyWindow(hWnd);
					}
					break;

				case ID_HELP_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC) About);
					break;

				case ID_COLORSELECTDIALOG_CHANGEBACKGROUNDCOLOR:
					BackgroudColorDialog(hWnd);
					break;

				case ID_SYSTEMDIALOG_OPENFILEDIALOG:
				{
					OPENFILENAME ofn; 
					TCHAR szFile[260];

					ZeroMemory(&ofn, sizeof(ofn));

					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					 
					ofn.lpstrFile =szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = _T("All Files *.*\0*.*\0Text Files *.txt\0*.TXT\0 Doc Files\0*.TXT;*.DOC;*.BAK\0");
					ofn.nFilterIndex = 1;
					ofn.lpstrInitialDir = _T("C:\\");
					ofn.lpstrTitle=_T("My Application - Open file");
					ofn.lpstrDefExt=_T("txt");
					ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;

					// Display the Open dialog box. 
					if (GetOpenFileName(&ofn)==TRUE) 
					{
						///////////////Thiênnnn
						HBITMAP hBMP = (HBITMAP)LoadImage( NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
						HDC memDC = CreateCompatibleDC(hdc);

						// Select the new bitmap
						SelectObject(memDC, hBMP);
						RECT rect;
						GetClientRect(hWnd, &rect);
						BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

						//MessageBox(NULL,ofn.lpstrFile,_T("Selected file"),MB_OK);
					
					}
				}
				break;

//=========Na================================================
				case ID_FORMAT_TEXT:
				{
					buff[0] = _T('/0');
					DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXT), hWnd, (DLGPROC) Text);

					// Grab the window dimensions.
					RECT bounds;
					GetClientRect(hWnd, &bounds);
					// Grab a DC to draw with.
					HDC hdc = GetDC(hWnd);
					DrawText(hdc, buff, -1, &bounds, DT_CENTER | DT_VCENTER);

					//Tiến hành cho vẽ lên bitmap
					HDC memDC = CreateCompatibleDC(hdc);
					SelectObject(memDC, hBitmap);
					DrawText(memDC, buff, -1, &bounds, DT_CENTER | DT_VCENTER);
					DeleteObject(memDC);
					ReleaseDC(hWnd, hdc);
				}
				break;
//===================================================
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
            
			}
			break;
		}

		case WM_RBUTTONDBLCLK:
			KillTimer(hWnd, ID_TIMER1);
			//effacer l'ecran
			SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)GetDC(hWnd), (LPARAM) nullptr);
			break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
            
			// TODO: Add any drawing code that uses hdc here...
			HDC hdc = BeginPaint(hWnd, &ps);
			//Vẽ bitmap ra màn hình trước
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			RECT rect;
			GetClientRect(hWnd, &rect);
			BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
			
			draw = false;
			switch (mode)
			{
				case 0:
					//Vẽ tự do thì ta vẽ luôn trên màn hình và bitmap
					FreeDraw(memDC);
					FreeDraw(hdc);
					break;
				case 1:
					//Vẽ đường thẳng thì ta chỉ vẽ lên màn hình
					DrawLine(hdc);
					break;
				case 4:
					DrawRectangle(hdc);
					break;
				case 5:
					DrawSquare(hdc);
					break;
				case 6:
					DrawEllipse(hdc);
					break;
				case 7:
					DrawRound(hdc);
					break;
				case 8:
					DrawCurve(hdc);
					break;
				default:
					break;
			}
			DeleteObject(memDC);
			//SelectObject(hdc, oldPen);
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

VOID DrawLine(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
	}
}

VOID FreeDraw(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
		p1 = p2;
	}
}

VOID DrawCurve(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		//MoveToEx(hdc, p1.x, p1.y, NULL);
		//LineTo(hdc, p2.x, p2.y);
		Arc(hdc, 100,200 , 300 , 400,500,600,700,800);
	}
}

VOID DrawRectangle(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, p1.x, p1.y, NULL);
		Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
	}
}

VOID DrawSquare(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, p1.x, p1.y, NULL);
		int k = p2.x - p1.x;
		if ( (p2.x > p1.x) && (p2.y < p1.y) || (p2.x < p1.x) && (p2.y > p1.y) )
			Rectangle(hdc, p1.x, p1.y, p2.x, p1.y - k);
		if ( (p2.y > p1.y) && (p2.x > p1.x) || (p2.y < p1.y) && (p2.x < p1.x) )
			Rectangle(hdc, p1.x, p1.y, p2.x, p1.y + k);
	}
}

VOID DrawEllipse(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, p1.x, p1.y, NULL);
		Ellipse(hdc, p1.x, p1.y, p2.x, p2.y);
	}
}

VOID DrawRound(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		HPEN hPen = CreatePen(PS_SOLID, size, RGB(color1, color2, color3));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, p1.x, p1.y, NULL);
		int k = p2.x - p1.x;
		if ( (p2.x > p1.x) && (p2.y < p1.y) || (p2.x < p1.x) && (p2.y > p1.y) )
			Ellipse(hdc, p1.x, p1.y, p2.x, p1.y - k);
		if ( (p2.y > p1.y) && (p2.x > p1.x) || (p2.y < p1.y) && (p2.x < p1.x) )
			Ellipse(hdc, p1.x, p1.y, p2.x, p1.y + k);
	}
}

VOID DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawLine(memDC);
			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

VOID FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:
		enTrainDessin = FALSE;
		break;


	}
}

VOID DrawCurveProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawCurve(memDC);
			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

VOID DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawRectangle(memDC);

			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

VOID DrawSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawSquare(memDC);
			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

VOID DrawEllipseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawEllipse(memDC);
			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

VOID DrawRoundProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			p1.x = GET_X_LPARAM(lParam);
			p1.y = GET_Y_LPARAM(lParam);
			p2_ancien = p2 = p1;
			enTrainDessin = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (wParam&MK_LBUTTON)
			{
				p2.x = GET_X_LPARAM(lParam);
				p2.y = GET_Y_LPARAM(lParam);
				GetWindowRect(hWnd, &windowRect);
				InvalidateRect(hWnd, &windowRect, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
		case WM_LBUTTONUP:
		
			//Tiến hành cho vẽ lên bitmap
			HDC hdc = GetDC(hWnd);
			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBitmap);
			DrawRound(memDC);
			DeleteObject(memDC);
			ReleaseDC(hWnd, hdc);

			enTrainDessin = FALSE;
			break;
	}
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		errhandler(_T("GetObject"), hwnd);

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER)+
		sizeof(RGBQUAD)* (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
		errhandler(_T("GlobalAlloc"), hwnd);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		errhandler(_T("GetDIBits"), hwnd);
	}

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
		errhandler(_T("CreateFile"), hwnd);
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER)+
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD)+pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)+
		pbih->biSize + pbih->biClrUsed
		* sizeof (RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL))
	{
		errhandler(_T("WriteFile"), hwnd);
	}
	

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+pbih->biClrUsed * sizeof (RGBQUAD),
		(LPDWORD)&dwTmp, (NULL)))
		errhandler(_T("WriteFile"), hwnd);

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
		errhandler(_T("WriteFile"), hwnd);

	// Close the .BMP file.  
	if (!CloseHandle(hf))
		errhandler(_T("CloseHandle"), hwnd);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

void errhandler(LPCTSTR sz, HWND hwnd)
{
	MessageBox(hwnd, sz, _T("Error"), MB_ICONERROR);
	PostQuitMessage(0);
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

//======================================================na=====
INT_PTR CALLBACK Text(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndCombo; 
	UNREFERENCED_PARAMETER(lParam);

	switch(message)
	{
		case WM_COMMAND:
			//if(wParam == buttonid)
			if (LOWORD(wParam) == IDOK)
			{
				hwndCombo = GetDlgItem(hDlg, IDC_EDIT3); 
				//get text from edit control
				GetWindowText(hwndCombo, buff, 1024);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			else if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}

    return (INT_PTR)FALSE;
}
//===========================================================

VOID BackgroudColorDialog(HWND hWnd)
{
	//MSDN example
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors
	static DWORD rgbCurrent;        // initial color selection
									// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		rgbCurrent = cc.rgbResult;
		hbrBackground = CreateSolidBrush(cc.rgbResult);
		InvalidateRect(hWnd, NULL, true);
	}
}