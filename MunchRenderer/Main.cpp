#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct MyShader : IShader {
  static vec3f light;
  static vec3f look; // look at position
  static vec3f up;   // camera up
  static vec3f cam;  // camera location

  static vec3f translate;
  static vec3f rotAngle;

  template <typename... T>
  MyShader(T &&... args) : IShader(std::forward<T>(args)...) {}

  VertShaderOutput vert(const Vertex &v, size_t idx) {
    // tri[idx] = v;
    static const size_t size = std::min(width, height) / 2;
    // static const Matrix projection =
    //    Matrix::translate({width / 2, height / 2, 0.f}) *
    //    Matrix::scale({size, size, size});
    const Matrix model =
        Matrix::translate(translate) * Matrix::rotate(rotAngle);
    static const Matrix view = Matrix::view(look, cam, up);
    static const Matrix projection = Matrix::camera(look.dist(cam));
    static const Matrix viewport =
        Matrix::translate({width / 2, height / 2, 0.f}) *
        Matrix::scale({size, size, size});
    static const Matrix projectionview = projection * view;
    static const Matrix viewportpv = viewport * projectionview;
    const Matrix pvm = projectionview * model;
    const Matrix pvminvtrans = pvm.invtrans();

    const Matrix transform = viewportpv * model;
    vec4f out = transform * v.pos;
    // out = Matrix::scale({size, size, size}) * out;
    out = out / out.w;
    vec4f nrm = pvminvtrans * v.nrm;
    tri[idx].pos = out.as<3>();
    tri[idx].tex = v.tex;
    tri[idx].nrm = nrm.as<3>();
    // tri[idx].nrm = v.nrm;
    return {tri[idx].pos};
  }

  FragShaderOutput frag(float r0, float r1, float r2) {
    Vertex fragVert = computeFragVert(r0, r1, r2);
    float intensity = fragVert.nrm.normal().dot(light.normal());
    return {intensity * texture->uv(fragVert.tex)};
  }
};

vec3f MyShader::light;
vec3f MyShader::translate;
vec3f MyShader::rotAngle;
vec3f MyShader::look;
vec3f MyShader::up;
vec3f MyShader::cam;

struct BoxCanvas : Canvas {
  size_t fcount;
  float angle;
  MyShader shader;

  BoxCanvas(size_t width, size_t height)
      : Canvas(width, height), fcount(), angle(), shader(width, height) {
    renderMode = RenderMode::TextureRaster;
    light = {1.f, 0.f, 1.f};
    MyShader::translate = {0.f, 0.f, 0.f};
    MyShader::light = {1.f, 0.f, 1.f};
    MyShader::look = {0.f, 0.f, 0.f};
    MyShader::up = {0.f, 1.f, -1.f};
    MyShader::cam = {0.f, 1.f, 3.f};
  }
  ~BoxCanvas() {}

  void update() {
    clear();

    static const float speed = .3f;
    // angle += deltaTime * speed;
    angle = fmod(angle, M_PI);
    // shader.light = {cos(angle), 0.f, sin(angle)};
    shader.rotAngle.y += deltaTime * speed;
    // shader.light =
    //    (shader.light * Matrix::rotate({0.f, -deltaTime * speed,
    //    0.f})).as<3>();
    model(L"african_head/african_head", &shader);
    model(L"african_head/african_head_eye_inner", &shader);
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