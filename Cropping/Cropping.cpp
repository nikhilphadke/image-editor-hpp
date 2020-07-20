#include <Windows.h>
#include<ctime>
#include<gdiplus.h>
#include<stdio.h>
#include<windowsx.h>

using namespace Gdiplus;

#pragma comment (lib, "gdiplus.lib")

HDC         imageDC;
HBITMAP     imageBmp;
HBITMAP     imageBmpOld;

const int   screenSize_X = 1920;
const int   screenSize_Y = 1080;




clock_t start_s;
clock_t stop_s;
COLORREF color[2000][1200], gsColor[2000][1200];
double R[2000][1200], G[2000][1200], B[2000][1200];
double x[2000][1200];
int bmWidth, bmHeight;
int flag = 0;


void loadImage(const char* pathname)
{
	imageDC = CreateCompatibleDC(NULL);

	BITMAP bm;
	imageBmp = (HBITMAP)LoadImageA(
		NULL,
		pathname,
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE
		);

	imageBmpOld = (HBITMAP)SelectObject(imageDC, imageBmp);

	GetObject(imageBmp, sizeof(bm), &bm);
	bmWidth = bm.bmWidth;
	bmHeight = bm.bmHeight;

}

void cleanUpImage()
{
	SelectObject(imageDC, imageBmpOld);
	DeleteObject(imageBmp);
	DeleteDC(imageDC);
}


void drawImage(HDC screen)
{
	flag = 1;
	int i = 0, j = 0;


	BitBlt(
		screen,
		0, 0,
		screenSize_X,
		screenSize_Y,
		imageDC,
		0, 0,
		SRCCOPY
		);



	for (i = 0; i < 1911; i++)
	{
		for (j = 0; j < 1075; j++)
		{
			color[i][j] = GetPixel(imageDC, i, j);
		}
	}



}

void ImageCrop(HDC screen,int x1pos,int y1pos, int x2pos, int y2pos)
{

	int i, j;
	LARGE_INTEGER start, stop, elapsed;
	LARGE_INTEGER frequency;
//	TCHAR str[256];
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	EncoderParameters *pGetEncoderParameters = NULL;

	RECT rc;


	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
		
	for (i = 0; i < 1911; i++)
		for (j = 0; j < 1075; j++)
		{
			if ((i >= x1pos && i <= x2pos) && (j >= y1pos && j <= y2pos))
				SetPixel(screen, i, j, color[i][j]);
			else
				SetPixel(screen, i, j, RGB(255,255,255));
		}

	for (i = x1pos; i < x2pos ; i++)
		for (j = y1pos; j < y2pos; j++)
			SetPixel(screen, i, j, color[i][j]);// RGB(x[i][j], x[i][j], x[i][j]));

	QueryPerformanceCounter(&stop);

	elapsed.QuadPart = stop.QuadPart - start.QuadPart;

	elapsed.QuadPart *= 1000;
	elapsed.QuadPart /= frequency.QuadPart;

}


void DrawBoxOutline(HWND hwnd, POINT pt1, POINT pt2)
{
	HDC hdc;

	hdc = GetDC(hwnd);

	SetROP2(hdc, R2_NOT);
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
//	ImageCrop(hdc, pt1.x, pt1.y, pt2.x, pt2.y);

	Rectangle(hdc, pt1.x, pt1.y, pt2.x, pt2.y);

	ReleaseDC(hwnd, hdc);

}

LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{

	int counter = 0; 
	TCHAR str[100];
	bool fDraw=true;

	RECT rc;

	
	HDC screen = GetDC(wnd);
	//HDC screen;
	static BOOL fBlocking, fValidBox;
	static POINT pt1, pt2, ptBox1,ptBox2;

	switch (msg)
	{

	case WM_CREATE: loadImage("1.bmp");
		break;


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC screen = BeginPaint(wnd, &ps);
		if (flag == 0)
		{
			drawImage(screen);
			flag = 1;
		}

		if (fValidBox)
		{
			SelectObject(screen, GetStockObject(BLACK_BRUSH));
			Rectangle(screen, ptBox1.x, ptBox1.y, ptBox2.x, ptBox2.y);

		}
		if (fBlocking)
		{
			SetROP2(screen, R2_NOT);
			SelectObject(screen,GetStockObject(NULL_BRUSH));
			Rectangle(screen, pt1.x, pt1.y, pt2.x, pt2.y);

		}
		
		EndPaint(wnd, &ps);
	}
	break;


	case WM_KEYDOWN:
		switch (w)
		{

		case VK_SPACE:
			if (counter == 0)
			{
				
			//	rgb_to_greyscale(screen);
				counter = counter + 1;
			}
			break;


		case VK_ESCAPE: DestroyWindow(wnd);
			break;

		case 'c': 
		case 'C': ImageCrop(screen,pt1.x, pt1.y, pt2.x, pt2.y);
				  break;
			      

		default: break;

		}
		break;

	
	case WM_LBUTTONDOWN: pt1.x = pt2.x = LOWORD(l);
						 pt1.y = pt2.y = HIWORD(l);
						 DrawBoxOutline(wnd, pt1, pt2);

						 SetCursor(LoadCursor(NULL, IDC_CROSS));

						 fBlocking = TRUE;

						 return 0;
						
	case WM_MOUSEMOVE: if (fBlocking)
					  { 
						  SetCursor(LoadCursor(NULL, IDC_CROSS));

						  DrawBoxOutline(wnd, pt1, pt2);

						  pt2.x = LOWORD(l);
						  pt2.y = HIWORD(l);

						  DrawBoxOutline(wnd, pt1, pt2);
				   	  }
					   return 0;

	case WM_LBUTTONUP: if (fBlocking)
						{

							DrawBoxOutline(wnd, pt1, pt2);


							ptBox1 = pt1;
							ptBox2.x = LOWORD(l);
							ptBox2.y = HIWORD(l);

							SetCursor(LoadCursor(NULL, IDC_ARROW));
							fBlocking = FALSE;
							fValidBox = TRUE;
							ImageCrop(screen, pt1.x, pt1.y, pt2.x, pt2.y);
			//				InvalidateRect(wnd, NULL, TRUE);
					
						}
					   return 0;
	

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	}

	return DefWindowProc(wnd, msg, w, l);
}

HWND createWindow(HINSTANCE inst)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wc.hInstance = inst;
	wc.lpfnWndProc = wndProc;
	wc.lpszClassName = TEXT("DisplayImage");
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);

	int style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	RECT rc = { 0, 0, screenSize_X, screenSize_Y };
	AdjustWindowRect(&rc, style, FALSE);

	return CreateWindow(
		TEXT("DisplayImage"),
		TEXT("Display an Image"),
		style | WS_VISIBLE,
		0, 0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL,
		inst,
		NULL);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	HWND wnd = createWindow(inst);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	cleanUpImage();

	return((int)msg.wParam);
}