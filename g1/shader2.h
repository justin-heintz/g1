#pragma once

class triangle {
    public:
        unsigned int VBO, VAO, EBO;

        void create(std::vector<float> vertices, std::vector<unsigned int> indices ) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        void setVAO() {
            glBindVertexArray(VAO);
        }
        void setVBO() {
            glBindVertexArray(VBO);
        }
        void setEBO() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        }
    private:
};
class Shader {
public:
    unsigned int vecShader;
    unsigned int fragShader;
    unsigned int ID;
    Shader(std::string vecFile, std::string fragFile) {
        std::ifstream vecIn;
        std::ifstream fragIn;
        std::stringstream vecStream;
        std::stringstream fragStream;

        vecIn.open(vecFile);
        vecStream << vecIn.rdbuf();
        vecIn.close();

        fragIn.open(fragFile);
        fragStream << fragIn.rdbuf();
        fragIn.close();

        std::string vecSourceTMP = vecStream.str();
        std::string fragSourceTMP = fragStream.str();

        GLchar const* vecSource = vecSourceTMP.c_str();
        GLchar const* fragSource = fragSourceTMP.c_str();

        GLint const vecLength = vecSourceTMP.size();
        GLint const fragLength = fragSourceTMP.size();

        vecShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vecShader, 1, &vecSource, &vecLength);
        glCompileShader(vecShader);
        checkCompileErrors(vecShader, "VERTEX");

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSource, &fragLength);
        glCompileShader(fragShader);
        checkCompileErrors(fragShader, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vecShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);
    }
    void use() {
        glUseProgram(ID);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void checkCompileErrors(GLuint shader, std::string type){
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)  {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type==: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }else{
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)  {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type=+: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};