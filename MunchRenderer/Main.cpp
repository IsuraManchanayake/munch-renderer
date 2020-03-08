#pragma comment(lib, "d2d1")

#include "Renderer.h"
#include "TGAImage.h"

struct MyShader : IShader {
  static vec3f light;
  static vec3f look; // look at position
  static vec3f up;   // camera up
  static vec3f cam;  // camera location
  mat4f pvminvtrans;
  mat4f pvm;

  static vec3f translate;
  static vec3f rotAngle;

  template <typename... T>
  MyShader(T &&... args) : IShader(std::forward<T>(args)...) {}

  VertShaderOutput vert(const Vertex &v, size_t idx) {
    triorig[idx] = v;
    static const size_t size = std::min(width, height) / 2;
    const mat4f model = mat4f::translate(translate) * mat4f::rotate(rotAngle);
    static const mat4f view = mat4f::view(look, cam, up);
    static const mat4f projection = mat4f::camera(look.dist(cam));
    static const Matrix viewport =
        mat4f::translate({width / 2, height / 2, 0.f}) *
        mat4f::scale({size, size, size});
    static const mat4f projectionview = projection * view;
    static const mat4f viewportpv = viewport * projectionview;
    pvm = projectionview * model;
    pvminvtrans = pvm.invtrans();

    const mat4f transform = viewportpv * model;
    vec4f out = transform * v.pos;
    out = out / out.w;
    // vec4f nrm = pvminvtrans * v.nrm;
    // vec4f tan = pvminvtrans * v.tan;
    // float d1 = v.nrm.dot(v.tan);
    // float d2 = nrm.dot(tan);

    triview[idx].pos = out.as<3>();
    triview[idx].tex = v.tex;
    // triview[idx].nrm = nrm.as<3>().normal();
    // triview[idx].tan = tan.as<3>().normal();
    triview[idx].nrm = (pvminvtrans * v.nrm.as<4>()).as<3>().normal();
    //triview[idx].tan = (pvminvtrans * v.tan.as<4>()).as<3>().normal();
    return {triview[idx].pos};
  }

  FragShaderOutput frag(float r0, float r1, float r2) {
    const Vertex fragVert = computeFragVert(r0, r1, r2);
    /*
    const vec3f &n = fragVert.nrm.normal();
    const vec3f &t = fragVert.tan.normal();
    const vec3f b = n.cross(t).normal();
    const Color normaluv = normal->uv(fragVert.tex);
    const vec3f normaldb{(2.f / 255.f) * normaluv.r - 1.f,
                         (2.f / 255.f) * normaluv.g - 1.f,
                         (2.f / 255.f) * normaluv.b - 1.f};
    //const mat3f dbtoobj{t.x, t.y, t.z, b.x, b.y, b.z, n.x, n.y, n.z};
    const mat3f dbtoobj{t.x, t.y, t.z, b.x, b.y, b.z, n.x, n.y, n.z};
    //vec3f lighttbn = dbtoobj * light;
    // const float intensity = normaldb.normal().dot(lighttbn.normal());
    vec3f normalv = dbtoobj * normaldb;
    // normalv = (pvm * normalv).as<3>();*/

    const float intensity = fragVert.nrm.normal().dot(light.normal());
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