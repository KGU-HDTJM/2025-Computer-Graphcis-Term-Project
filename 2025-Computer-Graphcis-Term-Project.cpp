// 2025-Computer-Graphcis-Term-Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "2025-Computer-Graphcis-Term-Project.h"

#include <windowsx.h>

#include <d3d11.h>
#include <DirectXMath.h>

#include"D3D11Base.h"
#include"Map.h"
#include "Sphere.h"
#include "SphereGenerator.h"
#include "Camera.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hWnd;

// TODO: Pope Coding standard
D3D11Base base;
Map* pMap; 
SphereGenerator* pSPGen;
Sphere* pSphere;
Camera* MainCamera;
// moving
struct MoveFactor {
	float Forward;
	float Left;
	float Backward;
	float Right;
	float Up;
	float Down;
} MovingFactor;

bool bFixMousePos;

struct Position {
	int X;
	int Y;
} MousePos;

struct RectInfo {
	Position Pos;
	UINT Width;
	UINT Height;
} WinInfo;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void RenderFrame(ID3D11Buffer*, ID3D11Buffer*, const ID3D11SamplerState*, const eShaderID, const eShaderID);
bool Init(void);
void Update(void);
void Render(void);
void Shutdown(void);


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
	LoadStringW(hInstance, IDC_MY2025COMPUTERGRAPHCISTERMPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2025COMPUTERGRAPHCISTERMPROJECT));

	MSG msg = { 0 };

	if (!Init()) {
		return FALSE;
	}
	// Main message loop:
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Update();
		Render();

	}
	Shutdown();
	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2025COMPUTERGRAPHCISTERMPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2025COMPUTERGRAPHCISTERMPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 'W':
		case 'w':
		{
			MovingFactor.Forward = 1.0F;
		}
		break;
		case 'A':
		case 'a':
		{
			MovingFactor.Left = 1.0F;
		}
		break;
		case 'S':
		case 's':
		{
			MovingFactor.Backward = 1.0F;
		}
		break;
		case 'D':
		case 'd':
		{
			MovingFactor.Right = 1.0F;
		}
		break;
		case VK_SPACE:
		{
			MovingFactor.Up = 1.0F;
		}
		break;
		case VK_SHIFT:
		{
			MovingFactor.Down = 1.0F;
		}
		break;
		case VK_ESCAPE:
		{
			bFixMousePos = !bFixMousePos;
			ShowCursor(bFixMousePos);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 'W':
		case 'w':
		{
			MovingFactor.Forward = 0.0F;
		}
		break;
		case 'A':
		case 'a':
		{
			MovingFactor.Left = 0.0F;
		}
		break;
		case 'S':
		case 's':
		{
			MovingFactor.Backward = 0.0F;
		}
		break;
		case 'D':
		case 'd':
		{
			MovingFactor.Right = 0.0F;
		}
		break;
		case VK_SPACE:
		{
			MovingFactor.Up = 0.0F;
		}
		break;
		case VK_SHIFT:
		{
			MovingFactor.Down = 0.0F;
		}
		break;
		default:
			break;
		}
		break;
	}
	break;
	case WM_MOUSEMOVE:
	{
		MousePos.X = GET_X_LPARAM(lParam);
		MousePos.Y = GET_Y_LPARAM(lParam);
	}
	break;
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
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
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

bool Init(void)
{
    if (!base.Initialize(g_hWnd)) {
        return false;
    }
    pMap = new Map(&base, 10, 10, 4);

	pSPGen = new SphereGenerator(&base);

	pSphere = pSPGen->CreateSphere(1.5F, XMFLOAT4(0.F, 0.F, 0.F, 1.F));
	MainCamera = new Camera(
		XMFLOAT4(0.F, 20.F, -40.F, 1.0F), 
		XMFLOAT4(0.F, 0.F, 0.F, 0.F), 
		XMFLOAT4(0.F, 1.F, 0.F, 0.F));

	RECT rect;
	GetWindowRect(g_hWnd, &rect);
	WinInfo.Width = rect.right - rect.left;
	WinInfo.Height = rect.bottom - rect.top;
	WinInfo.Pos.X = rect.left;
	WinInfo.Pos.Y = rect.top;

	MovingFactor.Down = 0;
	MovingFactor.Backward = 0;
	MovingFactor.Forward = 0;
	MovingFactor.Left = 0;
	MovingFactor.Right = 0;
	MovingFactor.Up = 0;
	return true;
}

void Update(void)
{

}

void Render(void)
{
	const static float BG_COLOR[] = { 0.0f, 0.125f, 0.3f, 1.0f };

	ID3D11Device* device = base.GetDevice();
	ID3D11DeviceContext* immediateContext = base.GetImmediateContext();
	IDXGISwapChain* swapChain = base.GetSwapChain();


	ID3D11Buffer* frameCBBuffer = base.GetCBFrameBuffer();
	CBFrame cbFrame;
	
	

	immediateContext->ClearRenderTargetView(base.GetRenderTargetView(), BG_COLOR);
	immediateContext->ClearDepthStencilView(base.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    immediateContext->IASetInputLayout(base.GetInputLayout());


    
    pSphere->Draw();
    pMap->Draw();
    
    swapChain->Present(1, 0);
}

void Shutdown(void)
{
    delete pMap;
    delete pSphere;
    delete pSPGen;
}
