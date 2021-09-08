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

#include "shader2.h"

using namespace std;

glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f);
vector<Shader*> shaders;
float timer = 60;
void init() {
    float vertices[] = {
         0.6f,  0.5f,    // top right
         0.5f, -0.5f,   // bottom right
        -0.5f, -0.5f,    // bottom left
        -0.5f,  0.5f,     // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shaders.push_back(new Shader("./player.vec", "./player.frag"));
}
void draw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    shaders[0]->use();
    shaders[0]->setVec3("textColor", glm::vec3(1.0f, 1.0f, 0.2f));
    shaders[0]->setMat4("projection", projection);
   // glUniformMatrix4fv(glGetUniformLocation(shaders[0]->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
   // unsigned int transformLoc = glGetUniformLocation(a.ID, "transform");
  //  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glLoadIdentity();
	glutSwapBuffers();
}
void update(int) {
	glutPostRedisplay();
	glutTimerFunc(1000.0 / timer, update, 0);
}
void main(int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1");
 	glViewport(0, 0, 600, 600);
 
	glewInit(); 
	init();
	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	
	//update
	glutTimerFunc(1000.0 / timer, update, 0);

	//start looop
	glutMainLoop();
}
