#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

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
 
float timer = 60;
float opacity = 1.0f;

enum eTypes { CLICK, KEYPRESS, MOVE, REMOVE, CREATE, ATTACK, SPAWN };

vector<Shader*> shaders;
 

unsigned int VAOF, VBOF;
unsigned int texture;

float color = 0.0f;
float sizer = 0.0f;
float x = 0;
float y = 0;
int mx = 0;
int my = 0;
triangle a;
glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(600), 0.0f, static_cast<float>(600));
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
	srand(static_cast <unsigned> (time(0)));
	 x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 250));
	 y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10));
	//START OF TEXT
	//glEnable(GL_CULL_FACE);
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
		}

		// generate texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer );

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
	shaders.push_back(new Shader("./player.vec", "./player.frag"));

	//glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(600), 0.0f, static_cast<float>(600));
	
	shaders[0]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaders[0]->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	vector<float> b  = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	vector<unsigned int> c = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	
	a.create(b,c);
}
void mouseFunc(int button, int state, int x, int y) {}
void normalKeysFunc(unsigned char key, int x, int y) {}
void draw() {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	bool dirx, diry;
	if (mx == 0) { x = x + 4; }else { x = x - 4; }
	if (my == 0) { y = y + 4; }	else { y = y - 4; }


 
	//shaders[1]->setVec4("flagColorTwo", glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));
	//glUniformMatrix4fv(glGetUniformLocation(shaders[1]->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	shaders[1]->use();
	//shaders[1]->setVec4("flagColorTwo", glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));
	
	a.setVAO(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	 //glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	RenderText(*shaders[0], "ANGERY!", x, y, sizer, glm::vec3(color, 0, 0));
	RenderText(*shaders[0], "Sample crap", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

	//a.setVAO();
	//a.setEBO();
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a.EBO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	glLoadIdentity();
	glutSwapBuffers();
}
void update(int) {
	glutPostRedisplay();
 
	//x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 600));
	//y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 600));
	mx = static_cast <int>  (rand()) / (static_cast <int> (RAND_MAX / 2));
	my = static_cast <int>  (rand()) / (static_cast <int> (RAND_MAX / 2));

	color += 0.005;
	sizer += 0.005;

	glutTimerFunc(1000.0 / timer, update, 0);
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("G1");
 	glViewport(0, 0, 600, 600);
//	glMatrixMode(GL_PROJECTION);
	//glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f);//left right bottom top
	//glMatrixMode(GL_MODELVIEW);

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
