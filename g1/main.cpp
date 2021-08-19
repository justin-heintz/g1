#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <GL/glew.h>
#include <GL/glut.h>

#include <fstream>
#include <sstream>

#include "helper.h";

using namespace std;

float txtMove = 0.1;
int c = 0;
float timer = 60;

class event {
public:
	int type = 0;//0 = idle, 1 = keyboard, 2 =  mouse
	int button = 0;// 0 = no button
	int buttonState = 0;
	int position[2] = { 0, 0 }; //X,Y
	int special = 0;
	char key;
	event(int intType) {
		type = intType;
	}

};
class entity {
public:
	int moveSpeed = 2;
	int position[3]{ 0, 0 ,0 }; //X,Y,Z
	float pos[24]{
		// positions--colors
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  // top left 
	};
	void setPosX(float x) {
		position[0] = x;
		pos[0]  = pos[0] + x;
		pos[6]  = pos[6] + x;
		pos[12] = pos[12] + x;
		pos[18] = pos[18] + x;
	}
	void setPosY(float y) { position[1] = y; 
		pos[1] = pos[1] + y;
		pos[7] = pos[7] + y;
		pos[13] = pos[13] + y;
		pos[19] = pos[19] + y;
	}
};

enum eTypes { CLICK, KEYPRESS, MOVE, REMOVE, CREATE, ATTACK, SPAWN };
vector<event> events;
vector<entity> entities;

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;
unsigned int texture;


void initGame() {
	entity player;
	entities.push_back(player);
	entities[0].pos;

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	string shad = loadShader("./player.vec");
	GLchar const* shader_source = shad.c_str();
	GLint const shader_length = shad.size();
	
	string frag = loadShader("./player.frag");
	GLchar const* fSource = frag.c_str();
	GLint const fLength = frag.size();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(entities[0].pos), entities[0].pos , GL_STATIC_DRAW);


	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shader_source, &shader_length);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fSource, &fLength);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}
void mouseFunc(int button, int state, int x, int y) {
	event ev(2);
	ev.button = button;
	ev.buttonState = state;
	ev.position[0] = x;
	ev.position[1] = y;
	events.push_back(ev);
}
void normalKeysFunc(unsigned char key, int x, int y) {
	event ev(1);
	ev.key = key;
	events.push_back(ev);
}
void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	char buffer[10];
	int ret = snprintf(buffer, sizeof buffer, "%f", txtMove);

	drawText(0, 100, buffer);
	drawText(0, txtMove, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut efficitur, magna eu posuere sollicitudin, dui orci vehicula odio, vitae vestibulum");

	
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	
	drawText(0, 100, buffer);
	drawText(0, txtMove, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut efficitur, magna eu posuere sollicitudin, dui orci vehicula odio, vitae vestibulum");



	glLoadIdentity();
	glutSwapBuffers();
}
void update(int) {
	glutPostRedisplay();
	txtMove += 1;
	c++;

	char s[50];
	time_t  t = time(NULL);
	ctime_s(s, 50, &t);

	if (c >= timer) {
		c = 0;
		//cout << s;
	}

	for (int i = 0; i < events.size(); i++) {
		if (events[i].type == 1) {

			if (events[i].key == 'w') {
				entities[0].setPosY(entities[0].position[1] + entities[0].moveSpeed);
				cout << "FOWARD " << entities[0].position[1] << "\n";
			}
			else if (events[i].key == 'a') {
				entities[0].setPosX(entities[0].position[0] - entities[0].moveSpeed);
				cout << "LEFT " << entities[0].position[0] << "\n";
			}
			else if (events[i].key == 's') {
				entities[0].setPosY(entities[0].position[1] - entities[0].moveSpeed);
				cout << "DOWN " << entities[0].position[1] << "\n";
			}
			else if (events[i].key == 'd') {
				entities[0].setPosX(entities[0].position[0] + entities[0].moveSpeed);
				cout << "RIGHT " << entities[0].position[0] << "\n";
			}

		}
		else if (events[i].type == 2) {
			cout << events[i].button << " - " << events[i].buttonState << "\n";
		}
		events.erase(events.begin());
	}
	events;//remove this is for testing
	//cout << timer << " times every 1 sec " << c << "\n";
	glutTimerFunc(1000.0 / timer, update, 0);
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1");
 	glViewport(0, 0, 600, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 600, 0, 600, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	glewInit();
	initGame();
	
	//draw
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	
	//input
	glutMouseFunc(mouseFunc);
	glutKeyboardFunc(normalKeysFunc);

	//update
	glutTimerFunc(1000.0 / 10.0, update, 0);

	//start looop
	glutMainLoop();
}