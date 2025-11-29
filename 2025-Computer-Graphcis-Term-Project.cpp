// 2025-Computer-Graphcis-Term-Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "2025-Computer-Graphcis-Term-Project.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include"D3D11Base.h"
#include"Perlin.h"
#include "Sphere.h"
#include "SphereGenerator.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hWnd;

D3D11Base base;
Perlin* perlin; 
SphereGenerator* pSPGen;
Sphere* pSphere;

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
    if (!InitInstance (hInstance, nCmdShow))
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2025COMPUTERGRAPHCISTERMPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY2025COMPUTERGRAPHCISTERMPROJECT);
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

bool Init(void)
{
    if (!base.Initialize(g_hWnd)) {
        return false;
    }
    perlin = new Perlin(base.GetDevice(), base.GetImmediateContext(), 10, 10, 4);

    pSPGen = new SphereGenerator(&base);

    pSphere = pSPGen->CreateSphere(1.5, XMFLOAT4(0.F, 0.F, 0.F, 1.F));
    
    return true;
}

void Update(void)
{
}


void RenderPerlin()
{
    
    ID3D11Buffer* cbWorld = base.GetCBChangeEveryFrame();   // World
    ID3D11Buffer* cbView = base.GetCBNeverChangeBuffer();    // View
    ID3D11Buffer* cbProj = base.GetCBChangeOnResizeBuffer(); // Projection
    ID3D11DeviceContext* ctx = base.GetImmediateContext();

    
    CBFrame cbFrame;

    XMMATRIX scale = XMMatrixScaling(3.0f, 1.0f, 3.0f);

    
    XMMATRIX translate = XMMatrixTranslation(-40.0f, -10.0f, -40.0f);
    cbFrame.World = scale * translate;
    cbFrame.World = XMMatrixTranspose(cbFrame.World);
    
    
    ctx->UpdateSubresource(cbWorld, 0, nullptr, &cbFrame, 0, 0);

    // 셰이더 설정
    ctx->VSSetShader(base.GetVertexShader(eShaderID::Basic), nullptr, 0);
    ctx->PSSetShader(base.GetPixelShader(eShaderID::Basic), nullptr, 0);

    ctx->VSSetConstantBuffers(0, 1, &cbWorld); // b0 = World
    ctx->VSSetConstantBuffers(1, 1, &cbView);  // b1 = View
    ctx->VSSetConstantBuffers(2, 1, &cbProj);  // b2 = Projection
    ctx->HSSetShader(nullptr, nullptr, 0);
    ctx->DSSetShader(nullptr, nullptr, 0);
    ctx->PSSetConstantBuffers(0, 1, &cbWorld); // Pixel Shader도 World 필요 시

    // IA 설정
    UINT stride = perlin->GetVertexSize();
    UINT offset = 0;
    ID3D11Buffer* vertex = perlin->GetVertexBuffer();
    ctx->IASetInputLayout(base.GetInputLayout());
    ctx->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
    ctx->IASetIndexBuffer(perlin->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw
    ctx->DrawIndexed(static_cast<UINT>(perlin->GetIndexCount()), 0, 0);
}


void Render(void) 
{
    const float BG_COLOR[] = { 0.0f, 0.125f, 0.3f, 1.0f };

    ID3D11Device* device = base.GetDevice();
    ID3D11DeviceContext* immediateContext = base.GetImmediateContext();
    IDXGISwapChain* swapChain = base.GetSwapChain();


    immediateContext->ClearRenderTargetView(base.GetRenderTargetView(), BG_COLOR);
    immediateContext->ClearDepthStencilView(base.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    immediateContext->IASetInputLayout(base.GetInputLayout());



    pSphere->Draw();
    RenderPerlin();
    
    swapChain->Present(1, 0);
}

void Shutdown(void)
{
    delete perlin;
    delete pSphere;
    delete pSPGen;
}
