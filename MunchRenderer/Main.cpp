#pragma comment(lib, "d2d1")

#include "Renderer.h"

struct BoxCanvas : Canvas {
  BoxCanvas(size_t width, size_t height) : Canvas(width, height) {
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        get(x, y).set(55, 55, 55);
      }
    }
  }
  ~BoxCanvas() {}

  void update() {
    clear();
    //for (size_t y = height / 4; y <= (3 * height) / 4; y++) {
    //  for (size_t x = width / 4; x <= (3 * width) / 4; x++) {
    //    get(x, y).set(200, 0, 128);
    //  }
    //}
    //line(50, 50, 500, 600);
    //line(50, 200, 50, 600);
    //line(50, 100, 500, 100);
    triangle(100, 100, 500, 300, 300, 500, defaultStroke);
  }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  if (SUCCEEDED(CoInitialize(NULL))) {
    {
      Renderer app;
      app.InitCanvas<BoxCanvas>(800, 600);

      if (SUCCEEDED(app.Initialize())) {
        app.RunMessageLoop();
      }
    }
    CoUninitialize();
  }

  return 0;
}