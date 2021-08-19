#pragma once
using namespace std;

string loadShader(string file = "") {
    stringstream streamStr;
    ifstream inFile;

    inFile.open(file);
    streamStr << inFile.rdbuf();
    inFile.close();

    return streamStr.str();
}
//draws 2d text using screen positions NOT opengls.
void drawText(float x, float y, char* string) {
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(x, y);
    for (char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}
void drawText(float x, float y, string string) {
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(x, y);
    for ( const char* c = string.c_str(); *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}