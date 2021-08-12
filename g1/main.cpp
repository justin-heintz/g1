#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <GL/glew.h>
#include <GL/glut.h>

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
	int position[3] = { 0, 0 ,0 }; //X,Y,Z
	void setPosX(int x) { position[0] = x; }
	void setPosY(int y) { position[1] = y; }
	void setPosZ(int z) { position[2] = z; }
};
enum eTypes { CLICK, KEYPRESS, MOVE, REMOVE, CREATE, ATTACK, SPAWN };
vector<event> events;
vector<entity> entities;

void initGame() {
	entity player;
	entities.push_back(player);
}
void drawText(float x, float y, char* string) {
	glRasterPos2f(x, y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
}
void mouseFunc(int button, int state, int x, int y) {
	event ev(2);
	ev.button = button;
	ev.buttonState = state;
	ev.position[0] = x;
	ev.position[1] = y;
	events.push_back(ev);
}
void specialKeysFunc(int key, int x, int y) {
	//cout << "SPECIAL| " << key << " | " << x << " | " << y << "\n";
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

	glColor3f(1, 1, 0);
	drawText(0, 100, buffer);

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
	glutInitWindowSize(200, 200);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1");

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 200, 0, 200, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	glutTimerFunc(1000.0 / 10.0, update, 0);

	glutMouseFunc(mouseFunc);
	glutKeyboardFunc(normalKeysFunc);
	glutSpecialFunc(specialKeysFunc);

	glewInit();
	initGame();
	glutMainLoop();
}