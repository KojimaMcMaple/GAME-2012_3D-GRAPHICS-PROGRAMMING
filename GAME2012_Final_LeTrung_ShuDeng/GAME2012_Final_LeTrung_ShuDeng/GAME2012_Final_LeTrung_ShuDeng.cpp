//***************************************************************************
// GAME2012_Final_LeTrung_ShuDeng.cpp by Kyle Hunter (Trung Le) & Shu Deng (C) 2020 All Rights Reserved.
//
// Final submission.
//
// Description:
//
//*****************************************************************************

using namespace std;

#include <cstdlib>
#include <ctime>
#include "vgl.h"
#include "LoadShaders.h"
#include "Light.h"
#include "Shape.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)


enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or 1 or 01
	KEY_BACKWARD = 0b00000010,		// 0x02 or 2 or 02
	KEY_LEFT = 0b00000100,			// 4
	KEY_RIGHT = 0b00001000,			// 8
	KEY_UP = 0b00010000,			// 16
	KEY_DOWN = 0b00100000,			// 32
	KEY_MOUSECLICKED = 0b01000000,	// 64
	KEY_ALT_FORWARD = 0x80,			// 128
	KEY_ALT_BACKWARD = 0x100,		// 256
	KEY_ALT_LEFT = 0x200,			// 512
	KEY_ALT_RIGHT = 0x400
	// Any other keys you want to add.
};

// IDs.
GLuint vao, ibo, points_vbo, colors_vbo, uv_vbo, normals_vbo, modelID, viewID, projID;
GLuint program;

// Matrices.
glm::mat4 View, Projection;

// Our bitflags. 1 byte for up to 8 keys.
unsigned int keys = 0; // Initialized to 0 or 0b00000000.

// ANIM VARS
float rot_angle = 0.0f;

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Texture variables.
GLuint kBlankTexture, kGroundTexture, kHedgeTexture, kWallTexture, kTowerTexture;
GLint width, height, bitDepth;

// Light variables.
AmbientLight aLight(glm::vec3(1.0f, 1.0f, 1.0f),	// Ambient colour.
	1.0f);							// Ambient strength.
SpotLight sLight(glm::vec3(5.0f, 5.0f, -5.0f),	// Position.
	glm::vec3(1.0f, 0.4f, 0.0f),	// Diffuse colour.
	1.0f,							// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f),  // Direction.
	50.0f);

// Shapes. Recommend putting in a map
Cube kOuterCube(5);
Cube kInnerCube(1);
Prism g_prism(24);
//Plane g_plane;
Grid kGroundShape(30, 10); // New UV scale parameter. Works with texture now.
const int kMazeSize = 500;
Wall kMaze[kMazeSize];







glm::vec3 kMazeHorizontalCoors[kMazeSize] = {
	//////////ROW #0
	glm::vec3(0.0f, 0.0f, 0.0f), //00
	glm::vec3(1.0f, 0.0f, 0.0f), //01
	glm::vec3(2.0f, 0.0f, 0.0f), //02
	glm::vec3(3.0f, 0.0f, 0.0f), //03
	glm::vec3(4.0f, 0.0f, 0.0f), //04
	glm::vec3(5.0f, 0.0f, 0.0f), //05
	glm::vec3(6.0f, 0.0f, 0.0f), //06
	glm::vec3(7.0f, 0.0f, 0.0f), //07
	glm::vec3(8.0f, 0.0f, 0.0f), //08
	glm::vec3(9.0f, 0.0f, 0.0f), //09
	glm::vec3(10.0f, 0.0f, 0.0f), //10
	glm::vec3(11.0f, 0.0f, 0.0f), //11
									//12
	glm::vec3(13.0f, 0.0f, 0.0f), //13
	glm::vec3(14.0f, 0.0f, 0.0f), //14
	glm::vec3(15.0f, 0.0f, 0.0f), //15
	glm::vec3(16.0f, 0.0f, 0.0f), //16
	glm::vec3(17.0f, 0.0f, 0.0f), //17
	glm::vec3(18.0f, 0.0f, 0.0f), //18
	glm::vec3(19.0f, 0.0f, 0.0f), //19
	glm::vec3(20.0f, 0.0f, 0.0f), //20
	glm::vec3(21.0f, 0.0f, 0.0f), //21
	glm::vec3(22.0f, 0.0f, 0.0f), //22
	glm::vec3(23.0f, 0.0f, 0.0f), //23
	//////////ROW #1
									//00
	glm::vec3(1.0f, 0.0f, -1.0f), //01
									//02
									//03
	glm::vec3(4.0f, 0.0f, -1.0f), //04
	//05
	glm::vec3(6.0f, 0.0f, -1.0f), //06
	glm::vec3(7.0f, 0.0f, -1.0f), //07
	//08
	//09
	glm::vec3(10.0f, 0.0f, -1.0f), //10
	glm::vec3(11.0f, 0.0f, -1.0f), //11
	glm::vec3(12.0f, 0.0f, -1.0f), //12
	//13
	//14
	//15
	//16
	glm::vec3(17.0f, 0.0f, -1.0f), //17
	glm::vec3(18.0f, 0.0f, -1.0f), //18
	glm::vec3(19.0f, 0.0f, -1.0f), //19
	//20
	//21
	glm::vec3(22.0f, 0.0f, -1.0f), //22
	//23
	//////////ROW #2
	//00
	//01
	//02
	//03
	//04
	glm::vec3(5.0f, 0.0f, -2.0f), //05
	//06
	glm::vec3(7.0f, 0.0f, -2.0f), //07
	glm::vec3(8.0f, 0.0f, -2.0f), //08
	//09
	//10
	glm::vec3(11.0f, 0.0f, -2.0f), //11
	//12
	glm::vec3(13.0f, 0.0f, -2.0f), //13
	glm::vec3(14.0f, 0.0f, -2.0f), //14
	//15
	//16
	//17
	//18
	//19
	//20
	glm::vec3(21.0f, 0.0f, -2.0f), //21
	//22
	//23
	//////////ROW #3
	glm::vec3(3.0f, 0.0f, -3.0f), //03
	glm::vec3(6.0f, 0.0f, -3.0f), //06
	glm::vec3(8.0f, 0.0f, -3.0f), //08
	glm::vec3(9.0f, 0.0f, -3.0f), //09
	glm::vec3(12.0f, 0.0f, -3.0f), //12
								   
								   
								   
	//ROW #N
	glm::vec3(0.0f, 0.0f, -1.0f), //00
	glm::vec3(1.0f, 0.0f, -1.0f), //01
	glm::vec3(2.0f, 0.0f, -1.0f), //02
	glm::vec3(3.0f, 0.0f, -0.0f), //03
	glm::vec3(4.0f, 0.0f, -0.0f), //04
	glm::vec3(5.0f, 0.0f, -0.0f), //05
	glm::vec3(6.0f, 0.0f, -0.0f), //06
	glm::vec3(7.0f, 0.0f, -0.0f), //07
	glm::vec3(8.0f, 0.0f, -0.0f), //08
	glm::vec3(9.0f, 0.0f, -0.0f), //09
	glm::vec3(10.0f, 0.0f, -0.0f), //10
	glm::vec3(11.0f, 0.0f, -0.0f), //11
	glm::vec3(12.0f, 0.0f, -0.0f), //12
	glm::vec3(13.0f, 0.0f, -0.0f), //13
	glm::vec3(14.0f, 0.0f, -0.0f), //14
	glm::vec3(15.0f, 0.0f, -0.0f), //15
	glm::vec3(16.0f, 0.0f, -0.0f), //16
	glm::vec3(17.0f, 0.0f, -0.0f), //17
	glm::vec3(18.0f, 0.0f, -0.0f), //18
	glm::vec3(19.0f, 0.0f, -0.0f), //19
	glm::vec3(20.0f, 0.0f, -0.0f), //20
	glm::vec3(21.0f, 0.0f, -0.0f), //21
	glm::vec3(22.0f, 0.0f, -0.0f), //22
	glm::vec3(23.0f, 0.0f, -0.0f), //23
};

//******************************************************************** 
// FUNCTIONS 
//********************************************************************
void timer(int);

void resetView()
{
	position = glm::vec3(5.0f, 3.0f, 10.0f);
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

void LoadTexture() {
	stbi_set_flip_vertically_on_load(true);

	// Blank texture.
	unsigned char* image0 = stbi_load("blank.jpg", &width, &height, &bitDepth, 0);
	if (!image0) cout << "Unable to load file!" << endl;

	glGenTextures(1, &kBlankTexture);
	glBindTexture(GL_TEXTURE_2D, kBlankTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image0);
	
	// GROUND TEXTURE
	unsigned char* image = stbi_load("TexturesCom_FloorsMedieval0063_1_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kGroundTexture);
	glBindTexture(GL_TEXTURE_2D, kGroundTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);



	// HEDGE TEXTURE
	unsigned char* image2 = stbi_load("TexturesCom_Hedges0060_1_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!image2) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kHedgeTexture);
	glBindTexture(GL_TEXTURE_2D, kHedgeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image2);

	

	// WALL TEXTURE
	unsigned char* image4 = stbi_load("TexturesCom_BrickOldOvergrown0044_4_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!image4) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kWallTexture);
	glBindTexture(GL_TEXTURE_2D, kWallTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image4);



	// TOWER TEXTURE
	unsigned char* image5 = stbi_load("TexturesCom_BrickOldOvergrown0006_1_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!image5) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kTowerTexture);
	glBindTexture(GL_TEXTURE_2D, kWallTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image5);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);
}

void init(void)
{
	srand((unsigned)time(NULL));
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	//Loading and compiling shaders
	program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	modelID = glGetUniformLocation(program, "model");
	projID = glGetUniformLocation(program, "projection");
	viewID = glGetUniformLocation(program, "view");

	// Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	LoadTexture();

	// Setting ambient Light.
	glUniform3f(glGetUniformLocation(program, "aLight.ambientColour"), aLight.ambientColour.x, aLight.ambientColour.y, aLight.ambientColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.ambientStrength"), aLight.ambientStrength);

	// Setting spot light.
	glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColour"), sLight.diffuseColour.x, sLight.diffuseColour.y, sLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		ibo = 0;
		glGenBuffers(1, &ibo);
	
		points_vbo = 0;
		glGenBuffers(1, &points_vbo);

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);

		uv_vbo = 0;
		glGenBuffers(1, &uv_vbo);

		normals_vbo = 0;
		glGenBuffers(1, &normals_vbo);

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	// Change shape data.
	g_prism.SetMat(0.1, 16);
	kGroundShape.SetMat(0.0, 16);

	// CHANGE BG COLOR
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	// Enable depth test and blend.
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Enable smoothing.
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	// Enable face culling.
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	timer(0); 
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
	glUniform3f(glGetUniformLocation(program, "eyePosition"), position.x, position.y, position.z);
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	calculateView();
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	// Draw all shapes.

	/*glBindTexture(GL_TEXTURE_2D, alexTx);
	g_plane.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(10.0f, 10.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_plane.NumIndices(), GL_UNSIGNED_SHORT, 0);*/

	//glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, kGroundTexture);
	kGroundShape.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, kGroundShape.NumIndices(), GL_UNSIGNED_SHORT, 0);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	glBindTexture(GL_TEXTURE_2D, kBlankTexture);
	g_prism.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(2.0f, 2.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(4.0f, 0.0f, -6.0f));
	glDrawElements(GL_TRIANGLES, g_prism.NumIndices(), GL_UNSIGNED_SHORT, 0);

	glBindTexture(GL_TEXTURE_2D, kWallTexture);
	kInnerCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, kInnerCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
	
	float x_offset = 3.0f;
	for (int i = 0; i < kMazeSize; i++) {
		glBindTexture(GL_TEXTURE_2D, kHedgeTexture);
		kMaze[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(kMazeHorizontalCoors[i].x + x_offset, kMazeHorizontalCoors[i].y, kMazeHorizontalCoors[i].z));
		glDrawElements(GL_TRIANGLES, kMaze[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, kHedgeTexture);
	kOuterCube.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(2.0f, 2.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(5.0, 3.0f, -5.0));
	glDrawElements(GL_TRIANGLES, kOuterCube.NumIndices(), GL_UNSIGNED_SHORT, 0);
	
	glBindVertexArray(0); // Done writing.
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	else if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	else if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position.y += MOVESPEED;
	else if (keys & KEY_DOWN)
		position.y -= MOVESPEED;

	glm::vec3 ground_front_vec = glm::normalize(glm::cross(rightVec, worldUp));

	/*if (keys & KEY_ALT_FORWARD)
		sLight.position -= ground_front_vec * MOVESPEED;
	else if (keys & KEY_ALT_BACKWARD)
		sLight.position += ground_front_vec * MOVESPEED;
	if (keys & KEY_ALT_LEFT)
		sLight.position -= rightVec * MOVESPEED;
	else if (keys & KEY_ALT_RIGHT)
		sLight.position += rightVec * MOVESPEED;*/
}

void timer(int) { // essentially our update()
	parseKeys();
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer, 0); // 60 FPS or 16.67ms.
}

//---------------------------------------------------------------------
//
// keyDown
//
void keyDown(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD; break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT; break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT; break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP; break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN; break;

	// LIGHT MOVEMENT
	case 'i':
		if (!(keys & KEY_ALT_FORWARD))
			keys |= KEY_ALT_FORWARD; break;
	case 'k':
		if (!(keys & KEY_ALT_BACKWARD))
			keys |= KEY_ALT_BACKWARD; break;
	case 'j':
		if (!(keys & KEY_ALT_LEFT))
			keys |= KEY_ALT_LEFT; break;
	case 'l':
		if (!(keys & KEY_ALT_RIGHT))
			keys |= KEY_ALT_RIGHT; break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	if (key == GLUT_KEY_UP)
	{
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; break;
	case 's':
		keys &= ~KEY_BACKWARD; break;
	case 'a':
		keys &= ~KEY_LEFT; break;
	case 'd':
		keys &= ~KEY_RIGHT; break;
	case 'r':
		keys &= ~KEY_UP; break;
	case 'f':
		keys &= ~KEY_DOWN; break;
	
	case 'i':
		keys &= ~KEY_ALT_FORWARD; break;
	case 'k':
		keys &= ~KEY_ALT_BACKWARD; break;
	case 'j':
		keys &= ~KEY_ALT_LEFT; break;
	case 'l':
		keys &= ~KEY_ALT_RIGHT; break;

	case ' ':
		resetView();
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	if (key == GLUT_KEY_UP)
	{
		keys &= ~KEY_FORWARD;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		keys &= ~KEY_BACKWARD;
	}
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &kGroundTexture);
	glDeleteTextures(1, &kHedgeTexture);
	glDeleteTextures(1, &kBlankTexture);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitWindowSize(800, 800);
	glutCreateWindow("GAME2012_Final_LeTrung_ShuDeng");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp); // New function for third example.
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.
	
	atexit(clean); // This GLUT function calls specified function before terminating program. Useful!

	glutMainLoop();

	return 0;
}
