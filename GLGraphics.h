#pragma once
#include "GLFWFuncs.h"
#include "OpenGL.h"
#include "OpenCL.h"
#include "Colors.h"
#include <assert.h>

class GLGraphics
{
public:
	void Initialize(GLFWwindow* pWindow, CL& openCL);
	const cl::Memory& ClFrameMemory(cl_uint index);
    void AcquireGLBuffers(CL& openCL);
    void ReleaseGLBuffers(CL& openCL);
	void ToggleCursorLock();
	void GetWindowSize(int* width, int* height);
	void SetWindowSize(int width, int height);
	void BeginFrame();
	void BlitFrame();
	void DisplayFrame();
	void LoadFonts(const std::string font_list, const std::string char_list);
	void LoadTextures(const std::string tex_list);
	void SpriteConfig(cl_float4 color, cl_float2 position, float scale, float rotation, float depth);
	void DrawInstances(GLuint instance_count, GLuint vertex_count=6, GLuint offset=0);
	void DrawSprite(GLuint vertex_count=6);
    void DrawText(const FreetypeGlText& text);
    void DrawText(const std::string& text, const glm::vec2& pos);
    void DrawFrameTex();
    void DrawFrameRT();
    bool CalcBounds(const float cam_foclen, const float& max_radius, const DVec3& position, uint4& result);

private:
	float4      gl_fill_clr;
	GLuint		gl_fb_id;
	GLuint		gl_rb_id;
	GLuint		gl_sb_id;
	GLuint      gl_rtx_id;
	GLuint		gl_tex_id;
	GLenum		gl_status;
	cl_context	cl_con;
	std::vector<cl::Memory> gl_memSet;

public:
    GL openGL;
	GLFWwindow*	window;
	int	windowWidth;
	int	windowHeight;
	int	widthHalf;
	int	heightHalf;
	int widthSpan;
	int heightSpan;
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
