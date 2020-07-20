#include <Windows.h>
#include<stdio.h>
#include "helper_timer.h"
#include "CudaHeader.h"


void cleanUpImage();



HDC         imageDC;
HBITMAP     imageBmp;
HBITMAP     imageBmpOld;

COLORREF color[2000][1200], gsColor[2000][1200];
float R[2000][1200], G[2000][1200], B[2000][1200];
float x[2000][1200];
int bmWidth, bmHeight;
int flag = 0;

float timeOnCpu;
float timeOnGpu;

char deviceName[256];

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




void drawImage(HDC screen)
{
	flag = 1;
	int i = 0, j = 0;


	BitBlt(
		screen,
		0, 0,
		bmWidth,
		bmHeight,
		imageDC,
		0, 0,
		SRCCOPY
		);


	for (i = 0; i < bmWidth; i++)
	{
		for (j = 0; j < bmHeight; j++)
		{
			color[i][j] = GetPixel(imageDC, i, j);
		}
	}

	MessageBox(NULL, TEXT("Press 'Space' to start Color Inversion and its benchmarking "), TEXT("Hello CUDA"), MB_OK);
}



void rgb_to_greyscale(HDC screen)
{

	HBITMAP hBitmap;
	int i, j;
	TCHAR str[256];
	long total_pixels = 0;
	
	total_pixels = bmWidth * bmHeight;

	float *rgb;
	float *x1;


	rgb = (float *)malloc(sizeof(float) * 6162980);
	x1 = (float *)malloc(sizeof(float) * 6162980);

	for (i = 0; i < total_pixels; i++)
	{
		rgb[i] = 0;
		x1[i] = 0;
	}


	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	for (i = 0; i < bmWidth; i++)
		for (j = 0; j < bmHeight; j++)
		{
			R[i][j] = 0.299 * GetRValue(color[i][j]);
			G[i][j] = 0.587 * GetGValue(color[i][j]);
			B[i][j] = 0.114 * GetBValue(color[i][j]);

			x[i][j] = R[i][j] + G[i][j] + B[i][j];
		}

	sdkStopTimer(&timer);
	timeOnCpu = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);



	int k = 0;
	for (i = 0; i < bmWidth; i++)
		for (j = 0; j < bmHeight; j++)
		{
			rgb[k]     = GetRValue(color[i][j]);
			rgb[k + 1] = GetGValue(color[i][j]);
			rgb[k + 2] = GetBValue(color[i][j]);

			k = k + 3;
		}



	ProcessImage(rgb, x1, &timeOnGpu, deviceName);

	sprintf(str, "Graphics Card Detected : NVIDIA %s \n\nTime On CPU = %f miliseconds\nTime On GPU = %f miliseconds",deviceName, timeOnCpu, timeOnGpu);

	MessageBox(NULL, str, TEXT("CUDA Benchmarking"), MB_OK);

	k = 0;
	for (i = 0; i < bmWidth; i++)
		for (j = 0; j < bmHeight; j++)
		{

			SetPixel(screen, i, j, RGB(x1[k], x1[k], x1[k]));
			k = k + 3;
		}

}




LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{

	int counter = 0;


	switch (msg)
	{

	case WM_CREATE: //loadImage("1.bmp");
		break;


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC screen = BeginPaint(wnd, &ps);
		if (flag == 0)
			drawImage(screen);
		EndPaint(wnd, &ps);
	}
	break;


	case WM_KEYDOWN:
		switch (w)
		{

		case VK_SPACE:
			if (counter == 0)
			{
				HDC screen = GetDC(wnd);
				rgb_to_greyscale(screen);
				counter = counter + 1;
			}
			break;


		case VK_ESCAPE: DestroyWindow(wnd);
			break;

		default: break;

		}
		break;

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

	RECT rc = { 0, 0, bmWidth, bmHeight };
	AdjustWindowRect(&rc, style, FALSE);

	return CreateWindow(
		TEXT("DisplayImage"),
		TEXT("Image Grayscale Filtering"),
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
	loadImage("1.bmp");
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


void cleanUpImage()
{
	SelectObject(imageDC, imageBmpOld);
	DeleteObject(imageBmp);
	DeleteDC(imageDC);


	free(color);
	free(gsColor);
	free(R);
	free(G);
	free(B);
	free(x);

}