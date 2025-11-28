// 2025-Computer-Graphcis-Term-Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "2025-Computer-Graphcis-Term-Project.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include"D3D11Base.h"
#include"Cube.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hWnd;

D3D11Base base;
Cube* cube;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void RenderFrame(ID3D11Buffer*, ID3D11Buffer*, const ID3D11SamplerState*, const eShaderID, const eShaderID);

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

    base.Initialize(g_hWnd);
    cube = new Cube(base.GetDevice(), base.GetImmediateContext(), 20, 20, 20, 10);

    // Main message loop:
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            RenderFrame(cube->GetVertexBuffer(), cube->GetIndexBuffer(), nullptr, eShaderID::Basic, eShaderID::Basic);
        }
    }

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


void RenderFrame(ID3D11Buffer* _vertexBuffer, ID3D11Buffer* _indexBuffer,
    const ID3D11SamplerState* sampler, const eShaderID vsID, const eShaderID psID)
{
    const float BG_COLOR[] = { 0.0f, 0.125f, 0.3f, 1.0f };

    ID3D11Device* device = base.GetDevice();
    ID3D11DeviceContext* immediateContext = base.GetImmediateContext();
    IDXGISwapChain* swapChain = base.GetSwapChain();


    immediateContext->ClearRenderTargetView(base.GetRenderTargetView(), BG_COLOR);
    immediateContext->ClearDepthStencilView(base.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    ID3D11Buffer* changeEveryFrame = base.GetCBChangeEveryFrame();

    struct CBChangeEveryFrame
    {
        XMMATRIX World;
    }CBFrame;

    CBFrame.World = XMMatrixTranspose(XMMatrixIdentity());

    immediateContext->UpdateSubresource(changeEveryFrame, 0, nullptr, &CBFrame, 0, 0);

    ID3D11Buffer* neverChange = base.GetNeverChangeBuffer();
    ID3D11Buffer* changeOnResize = base.GetChangeOnResizeBuffer();


    immediateContext->VSSetShader(base.GetVertexShader(vsID), nullptr, 0);
    immediateContext->VSSetConstantBuffers(0, 1, &neverChange);
    immediateContext->VSSetConstantBuffers(1, 1, &changeOnResize);
    immediateContext->VSSetConstantBuffers(2, 1, &changeEveryFrame);
    immediateContext->PSSetShader(base.GetPixelShader(psID), nullptr, 0);
    immediateContext->PSSetConstantBuffers(0, 1, &changeEveryFrame);

    UINT stride = cube->GetVertexSize();
    UINT offset = 0;

    immediateContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
    immediateContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    immediateContext->DrawIndexed(static_cast<UINT>(cube->GetIndexCount()), 0, 0);

    swapChain->Present(1, 0);
}