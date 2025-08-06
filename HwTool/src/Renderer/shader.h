#pragma once
#include <glad/glad.h> 
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <expected>
#include <glm/glm.hpp>
namespace fs = std::filesystem;
namespace Mir
{
    class Shader
    {
    private:
        auto compileShader(GLenum type, const char* source) -> std::expected<unsigned int, std::string>;
    public:
        unsigned int m_id; 
        Shader(fs::path vertexPath, fs::path fragmentPath);
        ~Shader();
        void use();
        void setBool(const std::string&name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setMat4(const char* name, const glm::mat4& mat) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        GLint getUniformLocation(const char* name) const;

    };
    

        
} // namespace Mir
