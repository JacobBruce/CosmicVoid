#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2.h>
#include "ReadWrite.h"

struct SoilImage
{
    unsigned char* bytes;
    int width, height, channels;
    GLint format;

    void Free()
    { SOIL_free_image_data(bytes); }
};

class SoilPP
{
public:

    static int SaveScreenshot(const std::string file_name, const int width=1024,
                               const int height=768, const int format=SOIL_SAVE_TYPE_PNG)
    {
        return SOIL_save_screenshot
        (
            file_name.c_str(),
            format, 0, 0, width, height
        );
    }

    static GLuint LoadGLTexture(const std::string file_name)
    {
        GLuint result = SOIL_load_OGL_texture
        (
            file_name.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
        );
        if (result == 0)
        {
            PrintLine("[SOIL2 Error] LoadGLTexture(): "+std::string(SOIL_last_result()));
            exit(EXIT_FAILURE);
        }
        return result;
    }

    static GLuint LoadGLCubemap(const std::vector<std::string>& file_names)
    {
        GLuint result = SOIL_load_OGL_cubemap
        (
            file_names[0].c_str(), file_names[1].c_str(), file_names[2].c_str(),
            file_names[3].c_str(), file_names[4].c_str(), file_names[5].c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
        );
        if (result == 0)
        {
            PrintLine("[SOIL2 Error] LoadGLCubemap(): "+std::string(SOIL_last_result()));
            exit(EXIT_FAILURE);
        }
        return result;
    }

    static bool ReadImageFile(std::string file_name, SoilImage& result)
    {
        bool image_error = false;
        size_t found = file_name.find_last_of(".");
        std::string file_ext = file_name.substr(found+1);

        if (file_ext == "jpg" || file_ext == "jpeg") {
            result.bytes = SOIL_load_image(file_name.c_str(), &(result.width), &(result.height), &(result.channels), SOIL_LOAD_RGB);
            result.format = GL_RGB;
            if (result.channels != 3) {
                PrintLine("[SoilPP Error] ReadImageFile(): invalid channel count in jpg ("+VarToStr(result.channels)+")");
                image_error = true;
            }
        } else if (file_ext == "bmp") {
            result.bytes = SOIL_load_image(file_name.c_str(), &(result.width), &(result.height), &(result.channels), SOIL_LOAD_RGBA);
            result.format = GL_RGBA;
            if (result.channels != 3) {
                PrintLine("[SoilPP Error] ReadImageFile(): invalid channel count in bmp ("+VarToStr(result.channels)+")");
                image_error = true;
            }
        } else if (file_ext == "png") {
            result.bytes = SOIL_load_image(file_name.c_str(), &(result.width), &(result.height), &(result.channels), SOIL_LOAD_RGBA);
            if (result.channels <= 0 || result.channels > 4) {
                PrintLine("[SoilPP Error] ReadImageFile(): invalid channel count in png ("+VarToStr(result.channels)+")");
                image_error = true;
            } else {
                result.format = GL_RGBA;
                result.channels = 4;
            }
        } else if (file_ext == "dds") {
            result.bytes = SOIL_load_image(file_name.c_str(), &(result.width), &(result.height), &(result.channels), SOIL_LOAD_AUTO);
            if (result.channels == 3) {
                result.format = GL_RGB;
            } else if (result.channels == 4) {
                result.format = GL_RGBA;
            } else {
                PrintLine("[SoilPP Error] ReadImageFile(): invalid channel count in dds ("+VarToStr(result.channels)+")");
                image_error = true;
            }
        } else {
            PrintLine("[SoilPP error] ReadImageFile(): unsupported image format");
            return false;
        }

        if (image_error) {
            SOIL_free_image_data(result.bytes);
            return false;
        } else if (result.bytes == 0 || result.bytes == nullptr) {
            PrintLine(std::string("[SoilPP Error] ReadImageFile(): ")+SOIL_last_result());
            return false;
        } else if (result.width == 0 && result.height ==0) {
            PrintLine("[SoilPP Error] ReadImageFile(): image size is 0x0");
            return false;
        } else {
            return true;
        }
    }

};
