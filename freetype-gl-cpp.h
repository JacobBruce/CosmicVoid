#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <cassert>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/font-manager.h>
#include <freetype-gl/vertex-buffer.h>
#include <freetype-gl/text-buffer.h>
#include <freetype-gl/markup.h>
#include "freetype-mat4.h"

//include <fontconfig/fontconfig.h>

#define DEFAULT_FONT_SIZE 20.0f

#define GLM_FTGL_VEC4(X) {X.x, X.y, X.z, X.w}

namespace ftgl {

constexpr char* shader_text_frag = (char*)
#include "./Data/shaders/text.fs"
;

constexpr char* shader_text_vert = (char*)
#include "./Data/shaders/text.vs"
;

typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

const glm::vec4 COLOR_BLACK  = {0.0, 0.0, 0.0, 1.0};
const glm::vec4 COLOR_WHITE  = {1.0, 1.0, 1.0, 1.0};
const glm::vec4 COLOR_RED    = {1.0, 0.0, 0.0, 1.0};
const glm::vec4 COLOR_GREEN  = {0.0, 1.0, 0.0, 1.0};
const glm::vec4 COLOR_BLUE   = {0.0, 0.0, 1.0, 1.0};
const glm::vec4 COLOR_YELLOW = {1.0, 1.0, 0.0, 1.0};
const glm::vec4 COLOR_GREY   = {0.5, 0.5, 0.5, 1.0};
const glm::vec4 COLOR_NONE   = {1.0, 1.0, 1.0, 0.0};
const ftgl::mat4 identity = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

/*inline std::string findFont(const std::string& search_pattern) {

    // If the search pattern is a path already, return this path
    if ((search_pattern.find('\\') != std::string::npos) || (search_pattern.find('/') != std::string::npos)) return search_pattern;

    auto throwError = [&search_pattern]() {
        throw std::runtime_error("FreetypeGL error -- could not find font: " + search_pattern);
    };

    std::string filename;
    FcInit();
    FcPattern *pattern = FcNameParse((const FcChar8*)search_pattern.c_str());
    FcConfigSubstitute( 0, pattern, FcMatchPattern );
    FcDefaultSubstitute( pattern );
    FcResult font_result;
    FcPattern *match = FcFontMatch(0, pattern, &font_result);
    FcPatternDestroy(pattern);
    if (!match) throwError();

    FcValue value;
    FcResult result = FcPatternGet( match, FC_FILE, 0, &value );
    if (result) throwError();

    filename = (char *)(value.u.s);
    FcPatternDestroy( match );
    return filename;
}*/

class Markup {
public:
    Markup() {
        description.font = NULL;
        description.family = NULL;
        //manager = NULL;
    }
    Markup(const std::string& font_family,
           float size,
           const glm::vec4 &color,
           bool bold,
           bool underlined,
           bool italic,
           bool strikethrough,
           bool overline,
           texture_font_t* font) {

        //manager = font_mngr;
        description.family = strdup(font_family.c_str());
        description.size   = size;
        description.bold   = bold;
        description.italic = italic;
        description.spacing= 0.0f;
        description.gamma  = 2.0f;
        description.foreground_color   = GLM_FTGL_VEC4(color);
        description.background_color   = GLM_FTGL_VEC4(COLOR_NONE);
        description.outline            = 0;
        description.outline_color      = GLM_FTGL_VEC4(COLOR_NONE);
        description.underline          = underlined;
        description.underline_color    = GLM_FTGL_VEC4(color);
        description.overline           = overline;
        description.overline_color     = GLM_FTGL_VEC4(color);
        description.strikethrough      = strikethrough;
        description.strikethrough_color= GLM_FTGL_VEC4(color);
        description.font = font;//font_manager_get_from_markup(manager, &description);
    }

    Markup(Markup&& other) {
        description = other.description;
        other.description.font = NULL;
        other.description.family = NULL;
        //manager = NULL;
    }

    void operator =(Markup &&other){
        description = other.description;
        other.description.font = NULL;
        other.description.family = NULL;
        //manager = NULL;
    }

    ~Markup() {
        if(description.family != NULL)
            free(description.family);

        //if(manager != NULL && description.font != NULL)
        //    font_manager_delete_font(manager, description.font);
        //FIXME why is this crashing?
        //FIXME Remove glyphs as well!
    }

    markup_t description;
//private:
    //font_manager_t* manager;
};

class FreetypeGl;

class FreetypeGlText {
public:
    explicit FreetypeGlText()
    {
        text_buffer = text_buffer_new();
        mat4_set_identity(pose);
        calcModelMatrix();
    }

    FreetypeGlText(FreetypeGlText&& other) {
        //manager = other.manager;
        text_buffer = other.text_buffer;
        pose = other.pose;
        model_matrix = other.model_matrix;
        scaling_factor = other.scaling_factor;
        //other.manager = NULL;
        other.text_buffer = NULL;
    }

    ~FreetypeGlText() {
        if(text_buffer)
            text_buffer_delete(text_buffer);
    }

    /*inline void setText(const markup_t* markup, const char* text)
    {
        vec2 pen = {{0,0}};
        text_buffer_clear(text_buffer);
        text_buffer_printf(text_buffer, &pen, markup, text, NULL);
    }*/

    inline void setText(const markup_t* markup, const std::string& text)
    {
        vec2 pen = {{0,0}};
        text_buffer_clear(text_buffer);
        text_buffer_printf(text_buffer, &pen, markup, text.c_str(), NULL);
    }

    inline const text_buffer_t* getTextBuffer() const
    {
        return text_buffer;
    }

    //inline void render() { manager->renderText(*this); }

    inline void setPose(const ftgl::mat4& p)
    {
        pose = p;
        calcModelMatrix();
    }

    inline void setPosition(float x, float y, float z)
    {
        pose.m30 = x; pose.m31 = y; pose.m32 = z;
        calcModelMatrix();
    }

    inline void setScalingFactor(float s)
    {
        scaling_factor = s;
        calcModelMatrix();
    }

    inline void setScaleAndPos(float s, float x, float y, float z)
    {
        scaling_factor = s;
        pose.m30 = x; pose.m31 = y; pose.m32 = z;
        calcModelMatrix();
    }

    const ftgl::mat4& getModelMatrix() const
    {
        return model_matrix;
    }

    void calcModelMatrix()
    {
        model_matrix = pose;
        model_matrix.m00 *= scaling_factor;
        model_matrix.m01 *= scaling_factor;
        model_matrix.m02 *= scaling_factor;
        model_matrix.m10 *= scaling_factor;
        model_matrix.m11 *= scaling_factor;
        model_matrix.m12 *= scaling_factor;
        model_matrix.m20 *= scaling_factor;
        model_matrix.m21 *= scaling_factor;
        model_matrix.m22 *= scaling_factor;
    }

private:
    ftgl::mat4 pose;
    ftgl::mat4 model_matrix;
    float scaling_factor = 1.0f;

    //const FreetypeGl* manager;
    text_buffer_t* text_buffer;
};

class FreetypeGl {
friend class Markup;
public:
    FreetypeGl(bool initialise = false) {
        if(initialise) init();
    }

    ~FreetypeGl() {
        glDeleteTextures(1, &font_manager->atlas->id);
        font_manager_delete(font_manager);
        glDeleteProgram(text_shader);
    }

    void init() {
        assert(!text_shader && "FreetypeGl was already initialised");
        font_manager = font_manager_new(1024, 1024, LCD_FILTERING_ON);

        text_shader = loadShader(shader_text_frag, shader_text_vert);

        default_font = LoadFont("./Data/fonts/monofont.ttf", latin1_alphabet);
        default_markup = std::move(createMarkup("Monofont", default_font));

        mat4_set_identity(view);
    }

    texture_font_t* LoadFont(const std::string& font_file, const char* chars, const float font_size=DEFAULT_FONT_SIZE)
    {
        texture_font_t* result = font_manager_get_from_filename(font_manager, font_file.c_str(), font_size);
        addCharacters(result, chars);
        return result;
    }

    Markup createMarkup(const std::string& font_family,
                        texture_font_t* font,
                        float size=DEFAULT_FONT_SIZE,
                        const glm::vec4& color=COLOR_WHITE,
                        bool bold=false,
                        bool underlined=false,
                        bool italic=false,
                        bool strikethrough=false,
                        bool overline=false) {

        return Markup(font_family, size, color, bold, underlined, italic, strikethrough, overline, font);
    }

    //FreetypeGlText createText(const std::string& text, const Markup& markup) {
    //    return createText(text, &markup.description);
    //}
    /*FreetypeGlText createText(const std::string& text, const markup_t* markup = NULL) {
        assert(text_shader && "FreetypeGl needs to be initialised first");
        if(markup == NULL) return FreetypeGlText(&default_markup.description, text.c_str());
        return FreetypeGlText(markup, text.c_str());
    }*/

    /**
     * @brief renderText Renders text directly (slow but easy to use)
     * @param text
     */
    void renderText(const std::string& text, const float x_pos, const float y_pos) {
        assert(text_shader && "FreetypeGl needs to be initialised first");
        text_buffer_t* buffer = text_buffer_new( );
        vec2 pen = {{x_pos,y_pos}};
        text_buffer_printf(buffer, &pen, &default_markup.description, text.c_str(), NULL);
        //updateTexture();

        static const GLuint mod_id = glGetUniformLocation( text_shader, "model" );
        glUniformMatrix4fv( mod_id, 1, 0, identity.data );

        vertex_buffer_render( buffer->buffer, GL_TRIANGLES );
        text_buffer_delete( buffer );
    }

    void renderText(const FreetypeGlText& text/*, bool call_pre_post=true*/) const {
        assert(text_shader && "FreetypeGl needs to be initialised first");
        //if(call_pre_post) preRender();
        static const GLint mod_id = glGetUniformLocation( text_shader, "model" );
        glUniformMatrix4fv(mod_id, 1, 0, text.getModelMatrix().data);

        vertex_buffer_render( text.getTextBuffer()->buffer, GL_TRIANGLES );
        //if(call_pre_post) postRender();
    }

    void updateTexture() {
        assert(text_shader && "FreetypeGl needs to be initialised first");
        glDeleteTextures(1, &font_manager->atlas->id);
        glGenTextures(1, &font_manager->atlas->id);
        glBindTexture( GL_TEXTURE_2D, font_manager->atlas->id );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, font_manager->atlas->width,
                      font_manager->atlas->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                      font_manager->atlas->data );
    }

    void preRender() const {
        //glColor4f(1.00,1.00,1.00,1.00);
        glBindVertexArray( 0 );
        glUseProgram(text_shader);
        glBindTexture( GL_TEXTURE_2D, font_manager->atlas->id );

        static const GLuint view_id = glGetUniformLocation( text_shader, "view" );
        static const GLuint proj_id = glGetUniformLocation( text_shader, "projection" );
        static const GLuint tex_id = glGetUniformLocation( text_shader, "tex" );
        static const GLuint pix_id = glGetUniformLocation( text_shader, "pixel" );

        glUniformMatrix4fv( view_id, 1, 0, view.data );
        glUniformMatrix4fv( proj_id, 1, 0, projection.data );
        glUniform1i( tex_id, 0 );
        glUniform3f( pix_id,
                     1.0f/font_manager->atlas->width,
                     1.0f/font_manager->atlas->height,
                     (float)font_manager->atlas->depth );

        //glActiveTexture( GL_TEXTURE0 );
        //glEnable( GL_BLEND );
        //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glDisable(GL_DEPTH_TEST);
        glBlendColor( 1, 1, 1, 1 );
    }

    void postRender() const {
        glBindTexture( GL_TEXTURE_2D, 0 );
        //glDisable( GL_BLEND );
        glEnable(GL_DEPTH_TEST);
        glBlendColor( 0, 0, 0, 0 );
        glUseProgram( 0 );
    }

    void setView(const ftgl::mat4& v) { view = v; }
    void setProjection(const ftgl::mat4& p) { projection = p; }
    void setView(const float* v) { std::memcpy(&view, v, sizeof(ftgl::mat4)); }
    void setProjection(const float* p) { std::memcpy(&projection, p, sizeof(ftgl::mat4)); }

    void setProjectionOrtho(float left,   float right,
                                   float bottom, float top,
                                   float znear,  float zfar) {

        mat4_set_orthographic(projection, left, right, bottom, top, znear, zfar);
    }

    void setProjectionPersp(float fovy,   float aspect,
                                   float znear,  float zfar) {

        mat4_set_perspective(projection, fovy, aspect, znear, zfar);
    }

    void addCharacters(texture_font_t* font, const std::string& chars) {
        texture_font_load_glyphs(font, chars.c_str());
        updateTexture();
    }

    ftgl::mat4 view;
    ftgl::mat4 projection;
    Markup default_markup;

private:

    GLuint compileShader(const char* source, const GLenum type) {
        GLint gl_compile_status;
        GLuint handle = glCreateShader(type);
        glShaderSource(handle, 1, &source, 0);
        glCompileShader(handle);
        glGetShaderiv( handle, GL_COMPILE_STATUS, &gl_compile_status);
        if(gl_compile_status == GL_FALSE){
            GLint log_length = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<GLchar> log(log_length);
            glGetShaderInfoLog(handle, log_length, &log_length, &log[0]);
            glDeleteShader(handle);
            throw std::runtime_error((char*)&log[0]);
        }
        return handle;
    }

    GLuint loadShader(char* frag, char* vert) {
        GLuint handle = glCreateProgram( );
        GLint link_status;
        if(strlen(vert)){
            GLuint vert_shader = compileShader(vert, GL_VERTEX_SHADER);
            glAttachShader(handle, vert_shader);
            glDeleteShader(vert_shader);
        }
        if(strlen(frag)){
            GLuint frag_shader = compileShader(frag, GL_FRAGMENT_SHADER);
            glAttachShader(handle, frag_shader);
            glDeleteShader(frag_shader);
        }

        glLinkProgram( handle );

        glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
        if (link_status == GL_FALSE){
            GLint log_length = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<GLchar> log(log_length);
            glGetProgramInfoLog(handle, log_length, &log_length, &log[0]);
            glDeleteProgram(handle);
            throw std::runtime_error((char*)&log[0]);
        }

        return handle;
    }

    const char* latin1_alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ";

    GLuint text_shader = 0;
    font_manager_t* font_manager;
    texture_font_t* default_font;
};

}
