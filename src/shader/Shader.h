#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>

#include "math/Matrix4f.h"

class Shader {
public:
    Shader(QOpenGLFunctions_4_5_Core* glFunctions) : m_glFunctions(glFunctions), m_shaderProgram(nullptr) {}
    ~Shader(){
        delete m_shaderProgram;
    }

    bool loadShaders(const char* vertexPath, const char* fragmentPath) {
        m_shaderProgram = new QOpenGLShaderProgram();

        // 加载并编译着色器（路径对应 CMake 复制后的目标文件夹）
        if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath)) {
            qCritical() << "Vertex shader compile error:" << m_shaderProgram->log();
            return false;
        }
        if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath)) {
            qCritical() << "Fragment shader compile error:" << m_shaderProgram->log();
            return false;
        }

        // 链接着色器程序
        if (!m_shaderProgram->link()) {
            qCritical() << "Shader program link error:" << m_shaderProgram->log();
            return false;
        }

        return true;
    }

    void bind() {
        if (m_shaderProgram) {
            m_shaderProgram->bind();
        }
    }

    void release() {
        if (m_shaderProgram) {
            m_shaderProgram->release();
        }
    }

    void setMatrix4(const char* name, const Matrix4f& matrix) {
        if (m_shaderProgram) {
            int location = m_shaderProgram->uniformLocation(name);
            if (location != -1) {
                m_glFunctions->glUniformMatrix4fv(location, 1, GL_FALSE, matrix.m);
            } else {
                qWarning() << "Uniform" << name << "not found in shader program.";
            }
        }
    }
    
    void setInt(const char* name, int value) {
        if (m_shaderProgram) {
            int location = m_shaderProgram->uniformLocation(name);
            if (location != -1) {
                m_glFunctions->glUniform1i(location, value);
            } else {
                qWarning() << "Uniform" << name << "not found in shader program.";
            }
        }
    }

private:
    QOpenGLFunctions_4_5_Core* m_glFunctions;
    QOpenGLShaderProgram* m_shaderProgram;
};