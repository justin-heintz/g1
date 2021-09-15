#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>

#include <map>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H


#include "shader2.h"

using namespace std;

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
FT_Library  library; 
FT_Face     face; 

int fonttexture = 0;

vector<Shader*> shaders;
float z = -3.0f;
float xp = 0.0f;
float yp = 0.0f;
float timer = 60;
float rotater = 0.0f;
float winsize = 200.0f;

drawOBJ element0;
drawOBJ element1;
drawOBJ element2;
drawOBJ element3;
drawOBJ element4;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

vector<float> vecs0 = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};
vector<float> vecs = {
   -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f 
};
vector<float> vecs2 = {
     0.5f,  0.5f, 1.0f,  // top right
     0.5f, -0.5f, 1.0f,  // bottom right
    -0.5f, -0.5f, 1.0f,  // bottom left
   
};
vector<float> vecs3 = {
    0.15f, 0.15f, -1.0f,  // top right
    0.15f, -0.15f, -1.0f,  // bottom right
    -0.15f, -0.15f, -1.0f,  // bottom left
};
vector<float> vtex = {
     0.5f,  0.5f, 0.0f,   0.3f, 0.0f,   
     0.5f, -0.5f, 0.0f,   0.3f, 0.3f,  
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f
};
vector<int> attrs = {3,2};

int createTexture(int width, int height, unsigned char* data, GLenum type = GL_RGBA, bool mipmaps = true) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (mipmaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texture;
}
void loadImage(char const* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    createTexture(width, height, data);
}
void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state	
    s.use();
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAOF);
    //https://github.com/justin-heintz/g1/blob/master/g1/main.cpp
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOF);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void init() {

    int error = FT_Init_FreeType(&library);
    if (error)  { }

    error = FT_New_Face(library, "./fonts/Lato-Bold.ttf", 0,  &face);
    if (error == FT_Err_Unknown_File_Format)  {  }  else if (error)  {  }

    FT_Set_Pixel_Sizes(face, 0, 48);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) { std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl; }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    fonttexture = createTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, GL_RED);
    
    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        }

        // now store character for later use
        Character character = {
            fonttexture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }//end of the for
     
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    char const* filename = "./mm.png";
    loadImage(filename);

    element0.create(vecs0);
    element1.create(vecs);
    element2.create(vecs2);
    element3.create(vecs3);
    element4.create(vtex, attrs);

    shaders.push_back(new Shader("./player.vec", "./player.frag"));
    shaders.push_back(new Shader("./text.vec", "./text.frag"));
    shaders.push_back(new Shader("./ttf.vec", "./ttf.frag"));
}
void draw() {
    glEnable(GL_DEPTH_TEST);
    /*
    glm::mat4 pro;
    pro = glm::perspective(glm::radians(45.0f), winsize / winsize, 0.1f, 100.0f);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(xp, yp, z));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotater, glm::vec3(0.5f, 1.0f, 0.0f));
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    shaders[0]->use();
    shaders[0]->setVec3("bcolor", glm::vec3(1.0f, 1.0f, 0.2f));
    shaders[0]->setMat4("projection", pro);
    shaders[0]->setMat4("view", view);
    shaders[0]->setMat4("model", model);
   
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    element0.bind();
    glDrawArrays(GL_TRIANGLES, 0, vecs0.size() / 3);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    element1.bind();
    glDrawArrays(GL_TRIANGLES, 0, vecs.size() / 3);


    shaders[0]->setVec3("bcolor", glm::vec3(0.0f, 1.0f, 0.2f));
    element2.bind();
    glDrawArrays(GL_TRIANGLES, 0, vecs2.size() / 3);


    shaders[0]->setVec3("bcolor", glm::vec3(0.0f, 0.0f, 1.2f));
    element3.bind();
    glDrawArrays(GL_TRIANGLES, 0, vecs3.size() / 3);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, 1);
        shaders[1]->use();
        shaders[1]->setMat4("projection", pro);
        shaders[1]->setMat4("view", view);
        shaders[1]->setMat4("model", model);
        element4.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_BLEND);
    */
	glLoadIdentity();
	glutSwapBuffers();
} 
void update(int) {
	glutPostRedisplay();
    rotater += 0.05f;
    //std::cout << rotater << "\n";
	glutTimerFunc(1000.0 / timer, update, 0);
}
void mouseFunc(int button, int state, int x, int y) {
    std::cout << button << " | " << state << " | " << x << " | " << y << '\n';
}
void mouseFuncMove( int x, int y) {
    std::cout  << x << " | " << y << '\n';
   // xp = x * .1;
   // yp = y * -.1;
}
void normalKeysFunc(unsigned char key, int x, int y) {
    cout << key << "\n";
    if (key == '0') { z += 1; }
    if (key == '1') { z -= 1; }

    if (key == '2') { xp += 1; }
    if (key == '5') { xp -= 1; }

    if (key == '6') { yp += 1; }
    if (key == '3') { yp -= 1; }
}
void main(int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(winsize, winsize);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1"); 
 	glViewport(0, 0, winsize, winsize);
    glEnable(GL_MULTISAMPLE);
	glewInit(); 
	init();
	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

    glutPassiveMotionFunc(mouseFuncMove);
    glutMouseFunc(mouseFunc);
    glutKeyboardFunc(normalKeysFunc);

    //update
	glutTimerFunc(1000.0 / timer, update, 0);

	//start looop
	glutMainLoop();
}