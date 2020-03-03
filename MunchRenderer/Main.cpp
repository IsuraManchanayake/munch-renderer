#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct BoxCanvas : Canvas {
  size_t fcount;
  float angle;
  BoxCanvas(size_t width, size_t height)
      : Canvas(width, height), fcount(), angle() {
    renderMode = RenderMode::TextureRaster;
  }
  ~BoxCanvas() {}

  void update() {
    // setFlipVertical(false);
    clear();
    // Vertex vx0{{200, 150, 0}, {255, 0, 0}};
    // Vertex vx1{{600, 150, 0}, {0, 255, 0}};
    // Vertex vx2{{400, 450, 0}, {0, 0, 255}};
    renderMode = RenderMode::TextureRaster;
    //drawBoundBox = true;
    // triangle(vx0, vx1, vx2);
    static const float speed = .3f;
    angle += deltaTime * speed;
    angle = fmod(angle, M_PI);
    light = {cos(angle), 0.f, sin(angle)};
    //model(L"monkey/monkey");
    model(L"african_head/african_head");
    // static Image img =
    // TGAImage::load(L"african_head/african_head_diffuse.tga"); Image img =
    // TGAImage::load(L"leehee.tga"); if (!img.good) return;
    // for (size_t y = 0; y < height; y++) {
    //  for (size_t x = 0; x < width; x++) {
    //    data[x + y * width] = img.data[x + y * width];
    //  }
    //}

    // image(img, {}, {width, height});
    std::stringstream ss;
    ss << fcount << ' ' << (1.f / deltaTime) << '\n';
    fcount++;
    OutputDebugStringA(ss.str().c_str());
  }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  if (SUCCEEDED(CoInitialize(NULL))) {
    {
      Renderer app;
      app.InitCanvas<BoxCanvas>(512, 512);

      if (SUCCEEDED(app.Initialize())) {
        app.RunMessageLoop();
      }
    }
    CoUninitialize();
  }

  return 0;
}