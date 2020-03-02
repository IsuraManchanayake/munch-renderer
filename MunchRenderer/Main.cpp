#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct BoxCanvas : Canvas {

  BoxCanvas(size_t width, size_t height) : Canvas(width, height) {}
  ~BoxCanvas() {}

  void update() {
    setFlipVertical(false);
    clear();
    // Vertex vx0{{200, 150, 0}, {255, 0, 0}};
    // Vertex vx1{{600, 150, 0}, {0, 255, 0}};
    // Vertex vx2{{400, 450, 0}, {0, 0, 255}};
    // renderMode = RenderMode::WireFrame;
    // triangle(vx0, vx1, vx2);
    // model("african_head/african_head.obj");

    Image img = TGAImage::load(L"african_head/african_head_diffuse.tga");
    if (!img.good)
      return;
    //for (size_t y = 0; y < height; y++) {
    //  for (size_t x = 0; x < width; x++) {
    //    data[x + y * width] = img.data[x + y * width];
    //  }
    //}

     image(img, vec2i{100, 100}, vec2i{800, 300});
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