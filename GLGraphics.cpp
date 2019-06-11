#include "GLGraphics.h"

void GLGraphics::Initialize(GLFWwindow* pWindow, CL& openCL)
{
    // Initialize OpenGL
    openGL.Initialize();

    window = pWindow;
    cl_con = openCL.context();
    cursorLocked = false;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    SetWindowSize(windowWidth, windowHeight);

    // Create framebuffer for OpenCL
    glGenFramebuffers(1, &gl_fb_id);
    glGenRenderbuffers(1, &gl_rb_id);
    glGenTextures(1, &gl_rtx_id);
    glGenTextures(1, &gl_tex_id);

    glBindFramebuffer(GL_FRAMEBUFFER, gl_fb_id);
    glBindRenderbuffer(GL_RENDERBUFFER, gl_rb_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gl_rb_id);

    glBindTexture(GL_TEXTURE_2D, gl_rtx_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, gl_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_tex_id, 0);

    gl_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (gl_status != GL_FRAMEBUFFER_COMPLETE) {
        PrintLine("[OpenGL Error]: FrameBuffer is not complete");
        GLFW::error_exit(window);
    }

    gl_memSet.push_back(cl::Memory(clCreateFromGLTexture(cl_con, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, gl_tex_id, &cl_error)));
    gl_memSet.push_back(cl::Memory(clCreateFromGLTexture(cl_con, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, gl_rtx_id, &cl_error)));
    //gl_memSet.push_back(cl::Memory(clCreateFromGLBuffer(cl_con, CL_MEM_READ_WRITE, openGL.StarBuffer(), &cl_error)));

    if (cl_error != CL_SUCCESS)
    {
        PrintLine("[OpenCL Error]: Failed to get OpenGL framebuffer reference!");
        GLFW::error_exit(window);
    }

    gl_sb_id = openGL.SpriteBuffer();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glActiveTexture(GL_TEXTURE0);
    glFinish();

	fontSizeNormal = (float)windowWidth / 1280.0f;
	fontSizeTiny = fontSizeNormal * 0.5f;
    fontSizeSmall = fontSizeNormal * 0.7f;
	fontSizeMedium = fontSizeNormal * 0.9f;
	fontSizeBig = fontSizeNormal * 1.1f;
	fontSizeLarge = fontSizeNormal * 1.3f;
	fontSizeHuge = fontSizeNormal * 1.5f;
}

const cl::Memory& GLGraphics::ClFrameMemory(cl_uint index)
{
    return gl_memSet[index];
}

void GLGraphics::ToggleCursorLock()
{
	if (cursorLocked) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetCursorPos(window, 0.0, 0.0);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	cursorLocked = !cursorLocked;
}

void GLGraphics::AcquireGLBuffers(CL& openCL)
{
    glFinish();
    openCL.queue.enqueueAcquireGLObjects(&gl_memSet);
}

void GLGraphics::ReleaseGLBuffers(CL& openCL)
{
    openCL.queue.enqueueReleaseGLObjects(&gl_memSet);
    openCL.queue.finish();
}

void GLGraphics::GetWindowSize(int* width, int* height)
{
    glfwGetFramebufferSize(window, width, height);
}

void GLGraphics::SetWindowSize(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, windowWidth, windowHeight);
    openGL.SetProjMatrix(windowWidth, windowHeight);

    widthHalf = windowWidth / 2;
    heightHalf = windowHeight / 2;
    widthSpan = windowWidth - 1;
    heightSpan = windowHeight - 1;
    //TODO: stuff
}

void GLGraphics::BeginFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_fb_id);
	glClear(GL_COLOR_BUFFER_BIT);
	glFinish();
}

void GLGraphics::BlitFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void GLGraphics::DisplayFrame()
{
	glFinish();
	glfwSwapBuffers(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLGraphics::LoadFonts(const std::string font_list, const std::string char_list)
{
    std::vector<std::string> fontChars(ReadFileLines(char_list));
    std::vector<std::string> fontFiles(ReadFileLines(font_list));
    glFonts.reserve(fontFiles.size());

    for (size_t i=0; i < fontFiles.size(); i++)
    {
        glFonts.push_back(openGL.LoadFont(fontFiles[i], fontChars[i]));
    }
}


void GLGraphics::LoadTextures(const std::string tex_list)
{
    std::vector<std::string> texFiles(ReadFileLines(tex_list));

    for (const std::string& file : texFiles)
    {
        openGL.LoadTexture(file);
    }
}

void GLGraphics::SpriteConfig(cl_float4 color, cl_float2 position, float scale, float rotation, float depth)
{
    static cl_SpriteData data;
    data = { color, position, scale, rotation, depth };
    glNamedBufferSubData(gl_sb_id, 0, sizeof(cl_SpriteData), &data);
    #if GL_SHOW_ERRORS
    if (GL::PrintErrors()) {
        PrintLine("Bad function: GLGraphics::SpriteConfig()");
        exit(EXIT_FAILURE);
    }
    #endif
	glFinish();
}

void GLGraphics::DrawInstances(GLuint instance_count, GLuint vertex_count, GLuint offset)
{
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertex_count, instance_count, offset);
    #if GL_SHOW_ERRORS
    if (GL::PrintErrors()) {
        PrintLine("Bad function: GLGraphics::DrawInstances()");
        exit(EXIT_FAILURE);
    }
    #endif
    glFinish();
}

void GLGraphics::DrawSprite(GLuint vertex_count)
{
    //glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    #if GL_SHOW_ERRORS
    if (GL::PrintErrors()) {
        PrintLine("Bad function: GLGraphics::DrawSprite()");
        exit(EXIT_FAILURE);
    }
    #endif
    //glFinish();
}

void GLGraphics::DrawText(const FreetypeGlText& text)
{
    openGL.RenderText(text);
    #if GL_SHOW_ERRORS
    if (GL::PrintErrors()) {
        PrintLine("Bad function: GLGraphics::DrawText()");
        exit(EXIT_FAILURE);
    }
    #endif
}

void GLGraphics::DrawText(const std::string& text, const glm::vec2& pos)
{
    openGL.RenderText(text, pos);
    #if GL_SHOW_ERRORS
    if (GL::PrintErrors()) {
        PrintLine("Bad function: GLGraphics::DrawText()");
        exit(EXIT_FAILURE);
    }
    #endif
}

void GLGraphics::DrawFrameTex()
{
    glDisable(GL_DEPTH_TEST);
    DrawSprite();
    glEnable(GL_DEPTH_TEST);
}

void GLGraphics::DrawFrameRT()
{
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, gl_rtx_id);
    DrawSprite();
    glEnable(GL_DEPTH_TEST);
}

bool GLGraphics::CalcBounds(const float cam_foclen, const float& max_radius, const DVec3& position, uint4& result)
{
    float maxX = 0.0f;
    float maxY = 0.0f;
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float diffX, diffY;

    DVec3 bboxVert;
    DVec3 boundBox[8];
    float2 coords[8];

    if ((position.z+max_radius) <= 0.0) return false;

    boundBox[0] = position.VectAdd(DVec3(max_radius, max_radius, max_radius));
    boundBox[1] = position.VectAdd(DVec3(max_radius, -max_radius, max_radius));
    boundBox[2] = position.VectAdd(DVec3(max_radius, max_radius, -max_radius));
    boundBox[3] = position.VectAdd(DVec3(max_radius, -max_radius, -max_radius));
    boundBox[4] = position.VectAdd(DVec3(-max_radius, max_radius, max_radius));
    boundBox[5] = position.VectAdd(DVec3(-max_radius, -max_radius, max_radius));
    boundBox[6] = position.VectAdd(DVec3(-max_radius, max_radius, -max_radius));
    boundBox[7] = position.VectAdd(DVec3(-max_radius, -max_radius, -max_radius));

    // transform points on bounding box to screen coordinates
    for (uint32_t p=0; p<8; p++) {

        // map corner vertex onto screen coords
        if (boundBox[p].z < 0.0) {
            // fix coordinates for points behind cam
            coords[p].x = (boundBox[p].x < 0.0) ? -1.0 : windowWidth;
            coords[p].y = (boundBox[p].y < 0.0) ? -1.0 : windowHeight;
        } else {
            coords[p].x = widthHalf + (boundBox[p].x / boundBox[p].z) * cam_foclen;
            coords[p].y = heightHalf + (boundBox[p].y / boundBox[p].z) * cam_foclen;
        }

        // get top left and bottom right coordinates
        minX = std::min(coords[p].x, minX);
        maxX = std::max(coords[p].x, maxX);
        minY = std::min(coords[p].y, minY);
        maxY = std::max(coords[p].y, maxY);
    }

    // cull objects not in field of view
    if (((minX < 0.0f && maxX < 0.0f) || (minX > windowWidth && maxX > windowWidth))
    || ((minY < 0.0f && maxY < 0.0f) || (minY > windowHeight && maxY > windowHeight)))
    { return false; }

    // clip coordinates if outside screen
    minX = std::min(std::max(minX, 0.0f), (float)widthSpan);
    minY = std::min(std::max(minY, 0.0f), (float)heightSpan);
    maxX = std::min(maxX, (float)windowWidth);
    maxY = std::min(maxY, (float)windowHeight);

    diffX = maxX - minX;
    diffY = maxY - minY;

    // skip objects covering less than a pixel
    if (diffX < 0.5f || diffY < 0.5f) return false;

    result.x = minX;
    result.y = minY;
    result.z = std::min((uint32_t)std::ceil(diffX + 1.0f), (uint32_t)windowWidth-result.x);
    result.w = std::min((uint32_t)std::ceil(diffY + 1.0f), (uint32_t)windowHeight-result.y);

    return true;
}
