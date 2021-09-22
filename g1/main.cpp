#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
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
vector<Shader*> shaders;
std::map<GLchar, Character> Characters;
FT_Face face;
FT_Library ft;
unsigned int VAOF, VBOF;
float testMove = 0.0f;
float z = -3.0f;
float xp = 0.0f;
float yp = 0.0f;
float timer = 60;
float rotater = 0.0f;
float winsize = 200.0f;

float a = 0.0, b = 0.0, c = 0.0;

drawOBJ element0;
drawOBJ element1;
drawOBJ element2;
drawOBJ element3;
drawOBJ element4;

//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::mat4 pro = glm::perspective(glm::radians(45.0f), winsize / winsize, 0.1f, 100.0f);
//glm::mat4 projection = glm::ortho(0.0f, 200.0f, 0.0f, 200.0f);

unsigned int texture;
unsigned int fonttexture;

vector<float> vecs0 = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    /*
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
    */
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
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f,// top right
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,// bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,// bottom left
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,// top left 
};
vector<int> attrs = {3,2};
vector<int> indices = { 0, 1, 3, 1, 2, 3 };
vector<int> emptyInt = {};

void init() {
    /*start texture*/
    int width, height, nrChannels;
    unsigned char* data = stbi_load("link.png", &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    /*end texture*/
   
    /*start font*/
    //FT_Library ft;
    if (FT_Init_FreeType(&ft)) { std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl; }
    //FT_Face face;
    if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face)) { std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; }

    FT_Set_Pixel_Sizes(face, 0, 12);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) { std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl; }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        }

        // generate texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }//end of the for
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAOF);
    glGenBuffers(1, &VBOF);
    glBindVertexArray(VAOF);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*end font*/
 
    element0.create(vecs0, { 3 }, emptyInt);
    element1.create(vecs, { 3 }, emptyInt);
    element2.create(vecs2, { 3 }, emptyInt);
    element3.create(vecs3, { 3 }, emptyInt);
    element4.create(vtex, attrs, indices);

    shaders.push_back(new Shader("./player.vec", "./player.frag"));
    shaders.push_back(new Shader("./text.vec", "./text.frag"));
    shaders.push_back(new Shader("./ttf.vec", "./ttf.frag"));
    
}
void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state	
    s.use();
   // s.setMat4("projection", pro);
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    //glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAOF);

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
void draw() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(xp, yp, z));
    glm::mat4 v2 = glm::mat4(1.0f);
    v2 = glm::translate(v2, glm::vec3(testMove, 0, -3));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotater, glm::vec3(0.5f, 1.0f, 0.0f));

    shaders[0]->use();
    shaders[0]->setMat4("projection", pro);
    shaders[0]->setMat4("view", view);
    shaders[0]->setMat4("model", model);

    shaders[0]->setVec3("bcolor", glm::vec3(240.0f, 56.0f, 0.2f));
    element0.bind();
    element0.draw();

    shaders[0]->setVec3("bcolor", glm::vec3(240.0f, 240.0f, 240.0f));
    element1.bind();
    element1.draw();

    shaders[0]->setVec3("bcolor", glm::vec3(255.5f, 0.5f, 255.5f));
    element2.bind();
    element2.draw();

    shaders[0]->setVec3("bcolor", glm::vec3(0.0f, 147.0f, 1.0f));
    element3.bind();
    element3.draw();

   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    RenderText(*shaders[2], "~", 0.9f,  0.9f, 0.009f, glm::vec3(1.0f, 0.0f, 0.0f));


    std::string strx = std::to_string(xp);
    strx.resize(5);
    std::string stry = std::to_string(yp);
    stry.resize(5);
    std::string strz = std::to_string(z);
    strz.resize(5);

    RenderText(*shaders[2], "x: " + strx,-1.0f, -0.3f,  0.009f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(*shaders[2], "y: " + stry,-1.0f, -0.4f, 0.009f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(*shaders[2], "z: " + strz, -1.0f, -0.5f, 0.009f, glm::vec3(1.0f, 1.0f, 1.0f));

    
    shaders[1]->use();
    shaders[1]->setMat4("projection", pro);
    shaders[1]->setMat4("view", v2);
    //shaders[1]->setMat4("model", model);

    shaders[1]->setVec2("move", glm::vec2(a, b));
    glBindTexture(GL_TEXTURE_2D, 1);
    element4.bind();
    element4.draw();
    glDisable(GL_BLEND);

	glLoadIdentity();
	glutSwapBuffers();
} 
void update(int) {
	glutPostRedisplay();
    rotater += 0.05f;
   // testMove += 0.001f;
    a += 0.001;
    b += 0.001;
    c += 0.001;
    //std::cout << rotater << "\n";
	glutTimerFunc(1000.0 / timer, update, 0);
}
void mouseFunc(int button, int state, int x, int y) {
    //std::cout << button << " | " << state << " | " << x << " | " << y << '\n';
}
void mouseFuncMove( int x, int y) {
   // std::cout  << x << " | " << y << '\n';
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

    if (key == 'w'){}
    if (key == 's'){}
    if (key == 'a'){}
    if (key == 'd'){}
       

}
void main(int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowSize(winsize, winsize);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1"); 
 	glViewport(0, 0, winsize, winsize);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
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