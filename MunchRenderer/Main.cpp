#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct MyShader : IShader {
  vec3f light;

  template <typename... T>
  MyShader(T &&... args) : IShader(std::forward<T>(args)...) {}

  VertShaderOutput vert(const Vertex &v) {
    size_t size = std::min(width, height) / 2;
    return {v.pos.scale({size, size, size})
                .translate({width / 2.f, height / 2.f, 0.f})};
  }

  FragShaderOutput frag(const Vertex &v) {
    float intensity = v.nrm.normal().dot(light);
    return intensity * texture->uv(v.tex);
  }
};

struct BoxCanvas : Canvas {
  size_t fcount;
  float angle;
  BoxCanvas(size_t width, size_t height)
      : Canvas(width, height), fcount(), angle() {
    renderMode = RenderMode::TextureRaster;
  }
  ~BoxCanvas() {}

  void update() {
    clear();

    static const float speed = .3f;
    angle += deltaTime * speed;
    angle = fmod(angle, M_PI);

    MyShader shader(width, height);
    shader.light = {cos(angle), 0.f, sin(angle)};
    model(L"african_head/african_head", &shader);

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