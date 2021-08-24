#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <map>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include "shader2.h";

#include <ft2build.h>
#include FT_FREETYPE_H  

using namespace std;

float txtMove = 0.1;
int e = 0;
float timer = 60;
float opacity = 1.0f;
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
vector<Shader*> shaders;
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

unsigned int VAOF, VBOF;
unsigned int texture;

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
FT_Face face;
FT_Library ft;

 
void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color){
	// activate corresponding render state	
	s.use();
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOF);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)	{
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

void initGame() {
	//START OF TEXT
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//FT_Library ft;
	if (FT_Init_FreeType(&ft))	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	//FT_Face face;
	if (FT_New_Face(ft, "./fonts/Lato-Bold.ttf", 0, &face))	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)){
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		/* void glTexImage2D(
			GLenum target,
			GLint level,
			GLint internalformat,
			GLsizei width,
			GLsizei height,
			GLint border,
			GLenum format,
			GLenum type,
			const void* data);*/
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

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
	//END OF TEXT

	shaders.push_back(new Shader("./ttf.vec", "./ttf.frag"));
	//shaders.push_back(new Shader("./player.vec", "./player.frag"));

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(600), 0.0f, static_cast<float>(600));
	shaders[0]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaders[0]->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



	entity player;
	entities.push_back(player);
	entities[0].pos;

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(entities[0].pos), entities[0].pos , GL_STATIC_DRAW);
	
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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//shaders[0]->setVec4("flagColorTwo", glm::vec4(1.0f, 0.5f, 0.2f, opacity));
	//shaders[0]->use();

	RenderText(*shaders[0], "Terrible Tutorial", 250.0f, 500.0f, 0.4f, glm::vec3(0.0, 0.8f, 0.2f));

	RenderText(*shaders[0], "Sample Shit", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	glLoadIdentity();
	glutSwapBuffers();
}
void update(int) {

	glutPostRedisplay();
	txtMove += 1;
	e++;

	char s[50];
	time_t  t = time(NULL);
	ctime_s(s, 50, &t);

	if (e >= timer) {
		e = 0;
		//cout << s;
	}

	for (int i = 0; i < events.size(); i++) {
		if (events[i].type == 1) {

			if (events[i].key == 'w') {
				opacity += 0.01f;
				entities[0].setPosY(entities[0].position[1] + entities[0].moveSpeed);
				cout << "FOWARD " << entities[0].position[1] << "\n";
			}
			else if (events[i].key == 'a') {
				entities[0].setPosX(entities[0].position[0] - entities[0].moveSpeed);
				cout << "LEFT " << entities[0].position[0] << "\n";
			}
			else if (events[i].key == 's') {
				opacity -= 0.01f;
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
	//glOrtho(0, 600, 0, 600, -1, 1);//left right bottom top nearz farz
	glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f);//left right bottom top
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
