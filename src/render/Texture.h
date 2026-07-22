#pragma once
#include <stb/stb_image.h>
#include <GL/gl.h>
#include <QOpenGLFunctions_4_5_Core>
#include <iostream>

class Texture {
public:
    Texture() = delete;
    ~Texture() {
        if (m_textureID) {
            m_glFunctions->glDeleteTextures(1, &m_textureID);
        }
    }

    Texture(const char* path, QOpenGLFunctions_4_5_Core* glFunctions) 
    : m_path(std::string(path)), m_glFunctions(glFunctions), m_format(GL_RGBA), m_textureID(0) {
        unsigned char* data = stbi_load(path, &m_width, &m_height, &m_channels, 0);
        if (data) {

            switch (m_channels) {
                case 1:
                    m_format = GL_RED;
                    break;
                case 3:
                    m_format = GL_RGB;
                    break;
                case 4:
                    m_format = GL_RGBA;
                    break;
                default:
                    std::cout << "Unsupported number of channels: " << m_channels << std::endl;
                    stbi_image_free(data);
                    return;
            }
            
            m_glFunctions->glGenTextures(1, &m_textureID);
            m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_textureID);
            m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
            m_glFunctions->glGenerateMipmap(GL_TEXTURE_2D);

            // 设置纹理参数
            m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
    }

   void bind(unsigned int slot = 0) {
        m_glFunctions->glActiveTexture(GL_TEXTURE0 + slot);
        m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_textureID);
    }

    void release() {
        m_glFunctions->glBindTexture(GL_TEXTURE_2D, 0);
    }

private:

    QOpenGLFunctions_4_5_Core* m_glFunctions = nullptr;    

    std::string m_path;
    GLuint m_format;
    GLuint m_textureID = 0;
    int m_width;
    int m_height;
    int m_channels;
};

