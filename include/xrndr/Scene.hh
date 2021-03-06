#pragma once
#include <xrndr/Camera.hh>
#include <xrndr/Program.hh>
#include <xrndr/Projection.hh>
#include <xrndr/Texture2D.hh>
#include <xrndr/Model.hh>
#include <xrndr/VertexFormat.hh>
#include <xrndr/MatrixStack.hh>
#include <xrndr/Light.hh>
#include <xrndr/TextureCache.hh>
#include <xrndr/Settings.hh>


namespace xrndr
{

class Renderer
{
public:
    virtual void setMaterial(Material * material) = 0;

protected:
    virtual ~Renderer() = default;
};

enum class RendererPass
{
    None,
    Geometry,
    Debug,
    Postprocess,
};


class Scene
    : public Renderer
{
public:
    explicit Scene(Settings & settings);

    ~Scene();

    void update(float);

    void draw();

    void toggleMode();

    shared_ptr<Camera> getCamera();

    void updateViewport(int width, int height);

private:
    virtual void setMaterial(Material * material) override;

private:
    Settings &                  _settings;
    RendererPass                _pass;

    bool                        _mode;
    MatrixStack                 _matrixStack;
    TextureCache                _textureCache;

    shared_ptr<Projection>      _projection;
    shared_ptr<Camera>          _camera;

    shared_ptr<Program>         _firstPassProgram;

    float                       _lightPhase;
    vector<PointLight>          _pointLights;
    vector<DirectedLight>       _directedLights;

    vector<std::shared_ptr<Model>> _models;

    shared_ptr<Program>         _nontexturedGeometryProgram;
    std::shared_ptr<Model>      _lightVisualizationModel;

    shared_ptr<Program>         _postprocessProgram;

    int _width;
    int _height;
    GLuint _framebuffer;
    GLuint _framebufferRenderbuffer;
    GLuint _framebufferTexture;
    Mesh _screenQuad;
};

}
