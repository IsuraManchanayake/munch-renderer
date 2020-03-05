#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct MyShader : IShader {
  vec3f light;
  vec3f rotAngle;

  template <typename... T>
  MyShader(T &&... args) : IShader(std::forward<T>(args)...) {
    rotAngle = {};
  }

  VertShaderOutput vert(const Vertex &v) {
    static size_t size = std::min(width, height) / 2;
    static Matrix transform = Matrix::scale({size, size, size}) *
                              Matrix::translate({width / 2, height / 2, 0.f});
    return {v.pos * Matrix::rotate(rotAngle) * transform};
  }

  FragShaderOutput frag(const Vertex &v) {
    float intensity = v.nrm.normal().dot(light.normal());
    return intensity * texture->uv(v.tex);
  }
};

struct BoxCanvas : Canvas {
  size_t fcount;
  float angle;
  MyShader shader;

  BoxCanvas(size_t width, size_t height)
      : Canvas(width, height), fcount(), angle(), shader(width, height) {
    renderMode = RenderMode::TextureRaster;
    shader.light = {1.f, 0.f, 1.f};
  }
  ~BoxCanvas() {}

  void update() {
    clear();

    static const float speed = .3f;
    // angle += deltaTime * speed;
    angle = fmod(angle, M_PI);
    //shader.light = {cos(angle), 0.f, sin(angle)};
    shader.rotAngle.y += deltaTime * speed ;
    shader.light =
        (shader.light * Matrix::rotate({0.f, -deltaTime * speed, 0.f})).as<3>();
    model(L"african_head/african_head", &shader);
    // model(L"corona/corona-lowpoly", &shader);

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