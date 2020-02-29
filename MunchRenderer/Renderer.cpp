#include "Renderer.h"

Renderer::Renderer()
    : m_hwnd(NULL), m_pDirect2dFactory(NULL), m_pRenderTarget(NULL),
      m_pLightSlateGrayBrush(NULL), m_pCornflowerBlueBrush(NULL),
      canvas(nullptr), width(800), height(600) {}

Renderer::~Renderer() {
  SafeRelease(&m_pDirect2dFactory);
  SafeRelease(&m_pRenderTarget);
  SafeRelease(&m_pLightSlateGrayBrush);
  SafeRelease(&m_pCornflowerBlueBrush);
  delete canvas;
}

void Renderer::RunMessageLoop() {
  MSG msg;

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

HRESULT Renderer::Initialize() {
  HRESULT hr;

  // Initialize device-indpendent resources, such
  // as the Direct2D factory.
  hr = CreateDeviceIndependentResources();

  if (SUCCEEDED(hr)) {
    // Register the window class.
    WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Renderer::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = HINST_THISCOMPONENT;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = L"D2DDemoApp";

    RegisterClassEx(&wcex);

    // Because the CreateWindow function takes its size in pixels,
    // obtain the system DPI and use it to scale the window size.
    FLOAT dpiX, dpiY;

    // The factory returns the current system DPI. This is also the value it
    // will use to create its own windows.
#pragma warning(push)
#pragma warning(disable : 4996)
    m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)

    // Create the window.
    m_hwnd = CreateWindow(
        L"D2DDemoApp", L"Direct2D Demo App",
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<UINT>(ceil(static_cast<float>(width) * dpiX / 96.f)),
        // Adding title bar height
        static_cast<UINT>(ceil(static_cast<float>(height + 39.f) * dpiY / 96.f)), NULL,
        NULL, HINST_THISCOMPONENT, this);
    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      ShowWindow(m_hwnd, SW_SHOWNORMAL);
      UpdateWindow(m_hwnd);
    }
  }

  return hr;
}

HRESULT Renderer::CreateDeviceIndependentResources() {
  HRESULT hr = S_OK;

  // Create a Direct2D factory.
  hr =
      D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

  return hr;
}

HRESULT Renderer::CreateDeviceResources() {
  HRESULT hr = S_OK;

  if (!m_pRenderTarget) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    // Create a Direct2D render target.
    hr = m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

    if (SUCCEEDED(hr)) {
      // Create a gray brush.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
    }
    if (SUCCEEDED(hr)) {
      // Create a blue brush.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
    }
  }

  return hr;
}

void Renderer::DiscardDeviceResources() {
  SafeRelease(&m_pRenderTarget);
  SafeRelease(&m_pLightSlateGrayBrush);
  SafeRelease(&m_pCornflowerBlueBrush);
}

LRESULT CALLBACK Renderer::WndProc(HWND hwnd, UINT message, WPARAM wParam,
                                   LPARAM lParam) {
  LRESULT result = 0;

  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    Renderer *pDemoApp = (Renderer *)pcs->lpCreateParams;

    ::SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>(pDemoApp));
    SetTimer(hwnd, 1, 1, NULL);
    result = 1;
  } else {
    Renderer *pDemoApp = reinterpret_cast<Renderer *>(
        static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

    bool wasHandled = false;

    if (pDemoApp) {
      switch (message) {
      case WM_SIZE: {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        pDemoApp->OnResize(width, height);
      }
        result = 0;
        wasHandled = true;
        break;

      case WM_DISPLAYCHANGE: {
        InvalidateRect(hwnd, NULL, FALSE);
      }
        result = 0;
        wasHandled = true;
        break;

      case WM_PAINT: {
        pDemoApp->OnRender();
        ValidateRect(hwnd, NULL);
      }
        result = 0;
        wasHandled = true;
        break;

      case WM_DESTROY: {
        PostQuitMessage(0);
      }
        result = 1;
        wasHandled = true;
        break;
      case WM_TIMER:
        InvalidateRect(hwnd, NULL, FALSE);
        result = 0;
        wasHandled = true;
        break;
      }
    }

    if (!wasHandled) {
      result = DefWindowProc(hwnd, message, wParam, lParam);
    }
  }

  return result;
}

HRESULT Renderer::OnRender() {
  HRESULT hr = S_OK;

  hr = CreateDeviceResources();
  if (SUCCEEDED(hr)) {
    canvas->update();

    m_pRenderTarget->BeginDraw();

    // create offscreen bitmap for pixel rendering
    D2D1_PIXEL_FORMAT desc2D = D2D1::PixelFormat();
    desc2D.format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    desc2D.alphaMode = D2D1_ALPHA_MODE_IGNORE;

    D2D1_BITMAP_PROPERTIES bmpProperties = D2D1::BitmapProperties();
    m_pRenderTarget->GetDpi(&bmpProperties.dpiX, &bmpProperties.dpiY);
    bmpProperties.pixelFormat = desc2D;

    D2D1_RECT_F rect = D2D1::RectF(0.0, 0.0, width, height);
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    ID2D1Bitmap *_backBufferBmp = NULL;
    HRESULT hr = m_pRenderTarget->CreateBitmap(size, canvas->data, width * 4,
                                               bmpProperties, &_backBufferBmp);

    // copy pixel buffer to bitmap
    D2D1_RECT_U crect;
    crect.left = 0;
    crect.top = 0;
    crect.right = width;
    crect.bottom = height;
    _backBufferBmp->CopyFromMemory(&crect, canvas->data, width * 4);

    // render bitmap to screen
    D2D1_RECT_F rectf;
    rectf.left = 0;
    rectf.top = 0;
    rectf.bottom = height;
    rectf.right = width;
    // Flip vertically
    m_pRenderTarget->SetTransform(
        D2D1::Matrix3x2F::Translation(0, 0.f - height) *
        D2D1::Matrix3x2F::Scale(D2D1::Size(1.f, -1.f)));
    if (_backBufferBmp) {
      m_pRenderTarget->DrawBitmap(_backBufferBmp, rectf);
      _backBufferBmp->Release();
    }

    m_pRenderTarget->EndDraw();
  }
  if (hr == D2DERR_RECREATE_TARGET) {
    hr = S_OK;
    DiscardDeviceResources();
  }
  return hr;
}
void Renderer::OnResize(UINT width, UINT height) {
  if (m_pRenderTarget) {
    // Note: This method can fail, but it's okay to ignore the
    // error here, because the error will be returned again
    // the next time EndDraw is called.
    m_pRenderTarget->Resize(D2D1::SizeU(width, height));
  }
}