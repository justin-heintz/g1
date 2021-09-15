#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
 
#include <sstream>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader2.h"

using namespace std;
//https://learnopengl.com/Getting-started/Hello-Triangle

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
void createTexture(int* width, int* height, unsigned char* data) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
   
    stbi_image_free(data);
}
void loadImage(char const* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    createTexture(&width, &height, data);
}

void init() {
   // char const* filename = "./container.jpg";
    char const* filename = "./mm.png";
    loadImage(filename);
    element0.create(vecs0);
    element1.create(vecs);
    element2.create(vecs2);
    element3.create(vecs3);
    element4.create(vtex, attrs);
    shaders.push_back(new Shader("./player.vec", "./player.frag"));
    shaders.push_back(new Shader("./text.vec", "./text.frag"));
}
void draw() {
    glEnable(GL_DEPTH_TEST);
    
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
    /**/
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