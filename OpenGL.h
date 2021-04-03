#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "freetype-gl-cpp.h"
#include "SoilPP.h"

enum ShaderType {
    VERTEX_SHADER=0,
    FRAGMENT_SHADER=1,
};

class ShaderSource
{
private:
    const char* srcPtrs[2];
public:
    const std::string vertexSource;
    const std::string fragmentSource;

    ShaderSource(const std::string& vertex_file, const std::string& fragment_file) :
        vertexSource(ReadFileStr(vertex_file)),
        fragmentSource(ReadFileStr(fragment_file))
    {
        srcPtrs[VERTEX_SHADER] = vertexSource.c_str();
        srcPtrs[FRAGMENT_SHADER] = fragmentSource.c_str();
    }

    const GLchar** GetGLPtr(const ShaderType shader_index)
    {
        return &srcPtrs[shader_index];
    }
};

class GL
{
protected:

    GLuint starVAO;
    GLuint spriteVAO;
    GLuint frameVAO;
    GLuint skyboxVAO;
    GLuint quadVBO;
    GLuint skyboxVBO;
    GLuint starIBO;
    GLuint spriteIBO;
    GLuint starShaderProg;
    GLuint spriteShaderProg;
    GLuint frameShaderProg;
    GLuint volgasShaderProg;
    GLuint flareShaderProg;
    GLuint skyboxShaderProg;
    GLuint starFragShader;
    GLuint spriteFragShader;
    GLuint frameFragShader;
    GLuint skyboxFragShader;
    GLuint volgasFragShader;
    GLuint flareFragShader;
    GLuint starVertShader;
    GLuint spriteVertShader;
    GLuint frameVertShader;
    GLuint skyboxVertShader;
    GLuint volgasVertShader;
    GLuint flareVertShader;
    GLuint activeShader;

    GLuint lcid, lpid, luid,
    arid, itid, dfid, tfid,
    faid, vpid, gcid, rsid,
    roid, gvid, cfid, crid, cuid;

    std::vector<GLuint> textures;
    std::vector<GLuint> freeTexSlots;

    ftgl::FreetypeGl textRenderer;

public:

    glm::mat4 orthoProj;
    glm::mat4 perspProj;
    glm::mat4 mvp;

    GL() : textRenderer(true) {}

    ~GL()
    {
        glDeleteTextures(textures.size(), textures.data());

        glDeleteProgram(starShaderProg);
        glDeleteProgram(spriteShaderProg);
        glDeleteProgram(frameShaderProg);
        glDeleteProgram(volgasShaderProg);
        glDeleteProgram(flareShaderProg);
        glDeleteProgram(skyboxShaderProg);
        glDeleteShader(starFragShader);
        glDeleteShader(spriteFragShader);
        glDeleteShader(frameFragShader);
        glDeleteShader(volgasFragShader);
        glDeleteShader(flareFragShader);
        glDeleteShader(skyboxFragShader);
        glDeleteShader(starVertShader);
        glDeleteShader(spriteVertShader);
        glDeleteShader(frameVertShader);
        glDeleteShader(volgasVertShader);
        glDeleteShader(flareVertShader);
        glDeleteShader(skyboxVertShader);

        glDeleteBuffers(1, &starIBO);
        glDeleteBuffers(1, &spriteIBO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &skyboxVBO);

        glDeleteVertexArrays(1, &starVAO);
        glDeleteVertexArrays(1, &spriteVAO);
        glDeleteVertexArrays(1, &frameVAO);
        glDeleteVertexArrays(1, &skyboxVAO);
    }

    static const GLfloat* QuadVertices()
    {
        static const GLfloat vertices[] = {
        //  Position      Texcoords
            -1.0f,  1.0f, 0.0f, 1.0f, // Top-left
             1.0f,  1.0f, 1.0f, 1.0f, // Top-right
             1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right

             1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
            -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            -1.0f,  1.0f, 0.0f, 1.0f  // Top-left
        };

        return vertices;
    }
    static const GLfloat* SkyboxVertices()
    {
        static const GLfloat vertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        return vertices;
    }

    void SetupMainBuffers()
    {
        // Create Vertex Array Object
        glGenVertexArrays(1, &starVAO);
        glBindVertexArray(starVAO);

        // Create a Vertex Buffer Object and copy data to it
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * 6 * sizeof(float), QuadVertices(), GL_STATIC_DRAW);

        // Specify the layout of the vertex data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

        // Create Instance Buffer Object
        glGenBuffers(1, &starIBO);
        glBindBuffer(GL_ARRAY_BUFFER, starIBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat) * IBUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

        // Specify the layout of the instance data
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(7 * sizeof(GLfloat)));

        // Make vertex shader update instance data per-quad
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);

        // Create VAO for skybox
        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);

        // Create a VBO for skybox
        glGenBuffers(1, &skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), SkyboxVertices(), GL_STATIC_DRAW);

        // Specify layout of skybox vertices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    void SetupSpriteBuffers()
    {
        // Create Vertex Array Object
        glGenVertexArrays(1, &spriteVAO);
        glBindVertexArray(spriteVAO);

        // Bind quad vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

        // Specify the layout of the vertex data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

        // Create Instance Buffer Object
        glGenBuffers(1, &spriteIBO);
        glBindBuffer(GL_ARRAY_BUFFER, spriteIBO);
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat) * IBUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

        // Specify the layout of the instance data
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(7 * sizeof(GLfloat)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));

        // Make vertex shader update attribs per-quad
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        // Do same stuff for frame vao
        glGenVertexArrays(1, &frameVAO);
        glBindVertexArray(frameVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    }

    void SetupShaders()
    {
        // Read star shader source files
        ShaderSource starShadersSrc("./Data/shaders/star.vs", "./Data/shaders/star.fs");

        // Read sprite shader source files
        ShaderSource spriteShadersSrc("./Data/shaders/sprite.vs", "./Data/shaders/sprite.fs");

        // Read sprite shader source files
        ShaderSource frameShadersSrc("./Data/shaders/frame.vs", "./Data/shaders/frame.fs");

        // Read space gas shader source files
        ShaderSource volgasShadersSrc("./Data/shaders/volgas.vs", "./Data/shaders/volgas.fs");

        // Read sprite shader source files
        ShaderSource flareShadersSrc("./Data/shaders/flare.vs", "./Data/shaders/flare.fs");

        // Read skybox shader source files
        ShaderSource skyboxShadersSrc("./Data/shaders/skybox.vs", "./Data/shaders/skybox.fs");

        // Create and compile the vertex shaders
        starVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(starVertShader, 1, starShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(starVertShader);
        CheckCompileStatus(starVertShader, VERTEX_SHADER);

        spriteVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(spriteVertShader, 1, spriteShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(spriteVertShader);
        CheckCompileStatus(spriteVertShader, VERTEX_SHADER);

        frameVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(frameVertShader, 1, frameShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(frameVertShader);
        CheckCompileStatus(frameVertShader, VERTEX_SHADER);

        volgasVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(volgasVertShader, 1, volgasShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(volgasVertShader);
        CheckCompileStatus(volgasVertShader, VERTEX_SHADER);

        flareVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(flareVertShader, 1, flareShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(flareVertShader);
        CheckCompileStatus(flareVertShader, VERTEX_SHADER);

        skyboxVertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(skyboxVertShader, 1, skyboxShadersSrc.GetGLPtr(VERTEX_SHADER), nullptr);
        glCompileShader(skyboxVertShader);
        CheckCompileStatus(skyboxVertShader, VERTEX_SHADER);

        // Create and compile the fragment shaders
        starFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(starFragShader, 1, starShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(starFragShader);
        CheckCompileStatus(starFragShader, FRAGMENT_SHADER);

        spriteFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(spriteFragShader, 1, spriteShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(spriteFragShader);
        CheckCompileStatus(spriteFragShader, FRAGMENT_SHADER);

        frameFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frameFragShader, 1, frameShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(frameFragShader);
        CheckCompileStatus(frameFragShader, FRAGMENT_SHADER);

        volgasFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(volgasFragShader, 1, volgasShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(volgasFragShader);
        CheckCompileStatus(volgasFragShader, FRAGMENT_SHADER);

        flareFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(flareFragShader, 1, flareShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(flareFragShader);
        CheckCompileStatus(flareFragShader, FRAGMENT_SHADER);

        skyboxFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(skyboxFragShader, 1, skyboxShadersSrc.GetGLPtr(FRAGMENT_SHADER), nullptr);
        glCompileShader(skyboxFragShader);
        CheckCompileStatus(skyboxFragShader, FRAGMENT_SHADER);

        // Attach shaders to shader programs
        starShaderProg = glCreateProgram();
        glAttachShader(starShaderProg, starVertShader);
        glAttachShader(starShaderProg, starFragShader);
        glBindFragDataLocation(starShaderProg, 0, "outColor");

        spriteShaderProg = glCreateProgram();
        glAttachShader(spriteShaderProg, spriteVertShader);
        glAttachShader(spriteShaderProg, spriteFragShader);
        glBindFragDataLocation(spriteShaderProg, 0, "outColor");

        frameShaderProg = glCreateProgram();
        glAttachShader(frameShaderProg, frameVertShader);
        glAttachShader(frameShaderProg, frameFragShader);
        glBindFragDataLocation(frameShaderProg, 0, "outColor");

        volgasShaderProg = glCreateProgram();
        glAttachShader(volgasShaderProg, volgasVertShader);
        glAttachShader(volgasShaderProg, volgasFragShader);
        glBindFragDataLocation(volgasShaderProg, 0, "outColor");

        flareShaderProg = glCreateProgram();
        glAttachShader(flareShaderProg, flareVertShader);
        glAttachShader(flareShaderProg, flareFragShader);
        glBindFragDataLocation(flareShaderProg, 0, "outColor");

        skyboxShaderProg = glCreateProgram();
        glAttachShader(skyboxShaderProg, skyboxVertShader);
        glAttachShader(skyboxShaderProg, skyboxFragShader);
        glBindFragDataLocation(skyboxShaderProg, 0, "outColor");

        // Link the shaders into shader programs
        glLinkProgram(starShaderProg);
        CheckProgramStatus(starShaderProg, GL_LINK_STATUS);
        glValidateProgram(starShaderProg);
        CheckProgramStatus(starShaderProg, GL_VALIDATE_STATUS);

        glLinkProgram(spriteShaderProg);
        CheckProgramStatus(spriteShaderProg, GL_LINK_STATUS);
        glValidateProgram(spriteShaderProg);
        CheckProgramStatus(spriteShaderProg, GL_VALIDATE_STATUS);

        glLinkProgram(frameShaderProg);
        CheckProgramStatus(frameShaderProg, GL_LINK_STATUS);
        glValidateProgram(frameShaderProg);
        CheckProgramStatus(frameShaderProg, GL_VALIDATE_STATUS);

        glLinkProgram(volgasShaderProg);
        CheckProgramStatus(volgasShaderProg, GL_LINK_STATUS);
        glValidateProgram(volgasShaderProg);
        CheckProgramStatus(volgasShaderProg, GL_VALIDATE_STATUS);

        glLinkProgram(flareShaderProg);
        CheckProgramStatus(flareShaderProg, GL_LINK_STATUS);
        glValidateProgram(flareShaderProg);
        CheckProgramStatus(flareShaderProg, GL_VALIDATE_STATUS);

        glLinkProgram(skyboxShaderProg);
        CheckProgramStatus(skyboxShaderProg, GL_LINK_STATUS);
        glValidateProgram(skyboxShaderProg);
        CheckProgramStatus(skyboxShaderProg, GL_VALIDATE_STATUS);

        // Setup shader texture sampler uniforms
        glUseProgram(starShaderProg);
        glUniform1i(glGetUniformLocation(starShaderProg, "texSmplr"), 0);

        glUseProgram(spriteShaderProg);
        glUniform1i(glGetUniformLocation(spriteShaderProg, "texSmplr"), 0);

        glUseProgram(frameShaderProg);
        glUniform1i(glGetUniformLocation(frameShaderProg, "texSmplr"), 0);

        glUseProgram(skyboxShaderProg);
        glUniform1i(glGetUniformLocation(skyboxShaderProg, "texSmplr"), 0);
        vpid = glGetUniformLocation(skyboxShaderProg, "mvp");

        glUseProgram(volgasShaderProg);
        glUniform1i(glGetUniformLocation(volgasShaderProg, "frameTex"), 0);
        glUniform1i(glGetUniformLocation(volgasShaderProg, "depthTex"), 1);
        roid = glGetUniformLocation(volgasShaderProg, "rayOrigin");
        gvid = glGetUniformLocation(volgasShaderProg, "gasVis");
        gcid = glGetUniformLocation(volgasShaderProg, "gasColor");
        rsid = glGetUniformLocation(volgasShaderProg, "resolution");
        cfid = glGetUniformLocation(volgasShaderProg, "camForward");
        crid = glGetUniformLocation(volgasShaderProg, "camRight");
        cuid = glGetUniformLocation(volgasShaderProg, "camUp");

        glUseProgram(flareShaderProg);
        lcid = glGetUniformLocation(flareShaderProg, "lightColor");
        lpid = glGetUniformLocation(flareShaderProg, "lightPos");
        luid = glGetUniformLocation(flareShaderProg, "luminosity");
        arid = glGetUniformLocation(flareShaderProg, "aspectRatio");
        itid = glGetUniformLocation(flareShaderProg, "iTime");
        dfid = glGetUniformLocation(flareShaderProg, "dFrac");
        tfid = glGetUniformLocation(flareShaderProg, "tFrac");
        faid = glGetUniformLocation(flareShaderProg, "alpha");
    }

    void Initialize()
    {
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Shader Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        // Setup buffers and shaders
        SetupMainBuffers();
        SetupSpriteBuffers();
        SetupShaders();

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (PrintErrors()) exit(EXIT_FAILURE);
    }

    void LoadTexture(const std::string tex_file)
    {
        if (freeTexSlots.empty()) {
            textures.push_back(SoilPP::LoadGLTexture(tex_file));
        } else {;
            textures[freeTexSlots.back()] = SoilPP::LoadGLTexture(tex_file);
            freeTexSlots.pop_back();
        }
        //glBindTexture(GL_TEXTURE_2D, textures[texIndex]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        if (PrintErrors()) {
            PrintLine("Failed loading texture: "+tex_file);
            exit(EXIT_FAILURE);
        }
    }

    void FreeTexture(const GLuint index)
    {
        glDeleteTextures(1, &textures[index]);
        textures[index] = 0;//uneeded?
        freeTexSlots.push_back(index);
    }

    GLuint GetTextureID(const GLuint index) const
    {
        return textures[index];
    }

    GLuint LoadCubemap(std::vector<std::string> faces)
    {
        GLuint textureID;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (uint32_t i = 0; i < faces.size(); i++)
        {
            SoilImage cmFace;
            if (SoilPP::ReadImageFile(faces[i], cmFace)) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGBA, cmFace.width, cmFace.height,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, cmFace.bytes);
                cmFace.Free();
            } else {
                PrintLine("Failed to load cubemap texture: " + faces[i]);
                exit(EXIT_FAILURE);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void UpdateMVP(const glm::mat4& view /*const glm::mat4& model*/)
    {
        mvp = perspProj * view;// * model;
    }

    void SetProjMatrix(const GLint& window_width, const GLint& window_height, const GLfloat fov=0.0f)
    {
        assert(window_width > 0 && window_height > 0);
        if (fov > 0.0f) {
            // Create 3D projection matrix
            perspProj = glm::perspective(fov, (float)window_width/(float)window_height, 0.1f, 100000.0f);
        } else {
            // Create 2D projection matrix
            orthoProj = glm::ortho(0.0f, (GLfloat)window_width, 0.0f, (GLfloat)window_height, -1.0f, 1.0f);
            // Give projection matrix to shaders
            glUseProgram(starShaderProg);
            glUniformMatrix4fv(glGetUniformLocation(starShaderProg, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
            glUseProgram(spriteShaderProg);
            glUniformMatrix4fv(glGetUniformLocation(spriteShaderProg, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
            // Set text renderer projection matrix
            textRenderer.setProjectionOrtho(0.0f, window_width, 0.0f, window_height, -1.0f, 1.0f);
        }
        glUseProgram(activeShader);
    }

    /*inline FreetypeGlText CreateText(const std::string text, const ftgl::markup_t* markup)
    {
        return textRenderer.createText(text, markup);
    }*/

    inline Markup CreateMarkup(const std::string& font_family, texture_font_t* font)
    {
        return textRenderer.createMarkup(font_family, font);
    }

    inline void RenderText(const FreetypeGlText& text)
    {
        textRenderer.renderText(text);
    }

    inline void RenderText(const std::string& text, const glm::vec2& pos)
    {
        textRenderer.renderText(text, pos.x, pos.y);
    }

    inline void StopTextRenderer()
    {
        textRenderer.postRender();
    }

    inline ftgl::texture_font_t* LoadFont(const std::string& font_file, const std::string& chars)
    {
        return textRenderer.LoadFont(font_file, chars.c_str());
    }

    inline void InitTextRender()
    {
        textRenderer.preRender();
    }

    inline void InitStarRender()
    {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(starVAO);
        BindShader(starShaderProg);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    inline void InitSpriteRender()
    {
        glBindVertexArray(spriteVAO);
        BindShader(spriteShaderProg);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    inline void InitFrameRender()
    {
        glBindVertexArray(frameVAO);
        BindShader(frameShaderProg);
    }

    inline void InitSkyboxRender(const GLuint cubemap_tex)
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(skyboxVAO);
        BindShader(skyboxShaderProg);
        glUniformMatrix4fv(vpid, 1, GL_FALSE, &mvp[0][0]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
    }

    inline void InitVolGasRender(const glm::vec3& color, const glm::vec3& res, const glm::vec3& ray_orig,
                                 const glm::vec3& cf, const glm::vec3& cr, const glm::vec3& cu, float gas_vis)
    {
        glDepthMask(GL_FALSE);
        glBindVertexArray(frameVAO);
        BindShader(volgasShaderProg);
        glUniform1f(gvid, gas_vis);
        glUniform3f(roid, ray_orig.x, ray_orig.y, ray_orig.z);
        glUniform3f(gcid, color.x, color.y, color.z);
        glUniform3f(rsid, res.x, res.y, res.z);
        glUniform3f(cfid, cf.x, cf.y, cf.z);
        glUniform3f(crid, cr.x, cr.y, cr.z);
        glUniform3f(cuid, cu.x, cu.y, cu.z);
    }

    inline void InitFlareRender(const float3& color, const float2& pos, float lumi, float aratio,
                                float itime, float dfrac, float tfrac, float alpha)
    {
        glBindVertexArray(frameVAO);
        BindShader(flareShaderProg);
        glUniform3f(lcid, color.x, color.y, color.z);
        glUniform2f(lpid, pos.x, pos.y);
        glUniform1f(luid, lumi);
        glUniform1f(arid, aratio);
        glUniform1f(itid, itime);
        glUniform1f(dfid, dfrac);
        glUniform1f(tfid, tfrac);
        glUniform1f(faid, alpha);
    }

    inline void BindShader(const GLuint shader_id)
    {
        glUseProgram(shader_id);
        activeShader = shader_id;
    }

    inline void BindTexture(const GLuint index)
    {
        glBindTexture(GL_TEXTURE_2D, textures[index]);
    }

    /*inline void BindBuffer(const GLuint buff_id)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buff_id);
    }

    inline void BindVertexArray(const GLuint buff_id)
    {
        glBindVertexArray(buff_id);
    }*/

    const Markup& DefaultMarkup() const { return textRenderer.default_markup; }

    const GLuint& QuadBuffer() const { return quadVBO; }

    const GLuint& SkyboxBuffer() const { return skyboxVBO; }

    const GLuint& StarBuffer() const { return starIBO; }

    const GLuint& SpriteBuffer() const { return spriteIBO; }

    static void CheckProgramStatus(const GLuint& program_id, GLint check_type)
    {
        GLint result;
        glGetProgramiv(program_id, check_type, &result);

        if (result == GL_FALSE) {
            GLint length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
            std::string message((size_t)length, ' ');
            glGetProgramInfoLog(program_id, length, &length, &message[0]);
            std::stringstream ss;
            ss << std::endl << "[OpenGL Error] Failed to ";
            ss << (check_type == GL_LINK_STATUS ? "link" : "validate");
            ss << " shaders\n" << message;
            PrintLine(ss);
            exit(EXIT_FAILURE);
        }
    }

    static void CheckCompileStatus(const GLuint& shader_id, const ShaderType shader_type)
    {
        GLint result;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE) {
            GLint length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
            std::string message((size_t)length, ' ');
            glGetShaderInfoLog(shader_id, length, &length, &message[0]);
            std::stringstream ss;
            ss << std::endl << "[OpenGL Error] Failed to compile ";
            ss << (shader_type == VERTEX_SHADER ? "vertex" : "fragment");
            ss << " shader\n" << message;
            PrintLine(ss);
            exit(EXIT_FAILURE);
        }
    }

    static std::string ErrorCodeToStr(const GLenum error_code)
    {
        switch (error_code) {
            case GL_INVALID_ENUM: return "invalid enumeration"; break;
            case GL_INVALID_VALUE: return "invalid value"; break;
            case GL_INVALID_OPERATION: return "invalid operation"; break;
            case GL_STACK_OVERFLOW: return "stack overflow"; break;
            case GL_STACK_UNDERFLOW: return "stack underflow"; break;
            case GL_OUT_OF_MEMORY: return "out of memory"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid framebuffer operation"; break;
            case GL_CONTEXT_LOST: return "context lost"; break;
            default: return "unknown error code"; break;
        }
    }

    static bool PrintErrors()
    {
        bool errorFound = false;
        while (GLenum error = glGetError()) {
            PrintLine("\n[OpenGL Error] ("+VarToStr(error)+"): "+ErrorCodeToStr(error));
            errorFound = true;
        }
        return errorFound;
    }

};
