#pragma comment(lib, "d2d1")

#include "Renderer.h"

struct BoxCanvas : Canvas {

  BoxCanvas(size_t width, size_t height) : Canvas(width, height) {}
  ~BoxCanvas() {}

  void update() {
    clear();
    Vertex vx0{{200, 150, 0}, {255, 0, 0}};
    Vertex vx1{{600, 150, 0}, {0, 255, 0}};
    Vertex vx2{{400, 450, 0}, {0, 0, 255}};
    triangle(vx0, vx1, vx2);
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