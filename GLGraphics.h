#pragma once
#include "GLFWFuncs.h"
#include "OpenGL.h"
#include "OpenCL.h"
#include "Camera.h"
#include "Colors.h"
#include "Profiles.h"
#include <assert.h>

class GLGraphics
{
public:
	void Initialize(GLFWwindow* pWindow, CL& openCL);
	cl::Memory& ClGlMemory(cl_uint index);
    void AcquireGLBuffers(CL& openCL);
    void ReleaseGLBuffers(CL& openCL);
	void ToggleCursorLock();
	void GetWindowSize(int* width, int* height);
	void SetWindowSize(int width, int height);
	void UpdateFOV(const GLfloat foc_len);
	void BeginFrame();
	void BlitFrame();
	void DisplayFrame();
	void LoadFonts(const std::string font_list, const std::string char_list);
	void LoadTextures(const std::string tex_list);
	void LoadSkyboxTex(const std::string tex_list);
	void SpriteConfig(cl_float4 color, cl_float2 position, float scale, float rotation, float depth);
	void DrawInstances(GLuint instance_count, GLuint vertex_count=6, GLuint offset=0);
	void DrawSprite(GLuint vertex_count=6);
    void DrawText(const FreetypeGlText& text);
    void DrawText(const std::string& text, const glm::vec2& pos);
    void DrawStarFlare();
	void DrawVolGas();
    void DrawSkybox();
    //void DrawFrameGL();
    void DrawFrameCL();
    void DrawFrameRT();
    void DrawDimFlare();
    void DrawBlurFlare();
    bool CalcBounds(const float cam_foclen, const float& max_radius, const DVec3& position, uint4& result);

private:
	float4      gl_fill_clr;
	GLuint		gl_fb_id;
	GLuint		gl_rb_id;
	GLuint		gl_sb_id;
	GLuint      gl_dtx_id;
	GLuint      gl_rtx_id;
	GLuint      gl_ftx_id;
	GLuint      gl_tex_id;
	GLuint      gl_ptt_id;
	GLuint		gl_clt_id;
	GLuint      gl_sbt_id;
	GLenum		gl_status;
	std::vector<cl::Memory> gl_memSet;

public:
    GL openGL;
    GLcam cam;
	GLFWwindow*	window;
	glm::vec3 resolution;
	SCREEN::Resolution profile;
    float2 pixFrac;
	int	windowWidth;
	int	windowHeight;
	int	widthHalf;
	int	heightHalf;
	int widthSpan;
	int heightSpan;
	float hwRatio;
	bool cursorLocked;

	float fontSizeNormal;
	float fontSizeTiny;
	float fontSizeSmall;
	float fontSizeMedium;
	float fontSizeBig;
	float fontSizeLarge;
	float fontSizeHuge;

	std::vector<ftgl::texture_font_t*> glFonts;

};
