#pragma once
#include "render/Texture.h"
#include "shader/Shader.h"

class Material
{
public:

    Material() = default;
    Material(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, std::shared_ptr<Texture> normal)
        : diffuse(diffuse), specular(specular), normal(normal) {}
    
    void setDiffuseTexture(std::shared_ptr<Texture> texture) {
        diffuse = texture;
    }

    void setSpecularTexture(std::shared_ptr<Texture> texture) {
        specular = texture;
    }

    void setNormalTexture(std::shared_ptr<Texture> texture) {
        normal = texture;
    }

    void bind(Shader& shader) const
    {
        if(diffuse)
        {
            diffuse->bind(0);
            shader.setInt("diffuseMap",0);
        }

        if(specular)
        {
            specular->bind(1);
            shader.setInt("specularMap",1);
        }

        if(normal)
        {
            normal->bind(2);
            shader.setInt("normalMap",2);
        }
    }

    void release() const
    {
        if(diffuse)
            diffuse->release();
        if(specular)
            specular->release();
        if(normal)
            normal->release();
    }

private:

    std::shared_ptr<Texture> diffuse;
    std::shared_ptr<Texture> specular;
    std::shared_ptr<Texture> normal;
};
