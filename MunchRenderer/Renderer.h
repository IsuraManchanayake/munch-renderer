#pragma once

#include "Common.h"
#include "Resource.h"

// C RunTime Header Files:
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <wchar.h>

#include <utility>

#include "Canvas.h"

class Renderer {
public:
  Renderer();
  ~Renderer();

  // Register the window class and call methods for instantiating drawing
  // resources
  HRESULT Initialize();

  // Process and dispatch messages
  void RunMessageLoop();

  template <typename CanvasType, typename... T>
  void InitCanvas(size_t width, size_t height, T &&... args);

private:
  // Initialize device-independent resources.
  HRESULT CreateDeviceIndependentResources();

  // Initialize device-dependent resources.
  HRESULT CreateDeviceResources();

  // Release device-dependent resource.
  void DiscardDeviceResources();

  // Draw content.
  HRESULT OnRender();

  // Resize the render target.
  void OnResize(UINT width, UINT height);

  // The windows procedure.
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                                  LPARAM lParam);

private:
  HWND m_hwnd;
  ID2D1Factory *m_pDirect2dFactory;
  ID2D1HwndRenderTarget *m_pRenderTarget;
  ID2D1SolidColorBrush *m_pLightSlateGrayBrush;
  ID2D1SolidColorBrush *m_pCornflowerBlueBrush;
  Canvas *canvas;
  size_t width;
  size_t height;
};

template <typename CanvasType, typename... T>
void Renderer::InitCanvas(size_t width, size_t height, T &&... args) {
  this->canvas = new CanvasType(width, height, std::forward<T>(args)...);
  this->width = width;
  this->height = height;
}