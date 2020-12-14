//***************************************************************************
// GAME2012_Midterm_LeTrung.cpp - 101264698 (C) 2020 All Rights Reserved.
//
// Assignment 2 submission.
//
// Description:
// Click run to see the results.
//*****************************************************************************

using namespace std;

#include <cstdlib>
#include <ctime>
#include "vgl.h"
#include "GAME2012_Midterm_LeTrung_LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <iostream>
#include <array>

#define FPS 60
#define SPEED 0.25f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define PI 3.14159265358979324

static float circle_radius = 1.0; // Radius of circle.
static float circle_x_pos = 0.0; // X-coordinate of center of circle.
static float circle_y_pos = 0.0; // Y-coordinate of center of circle.
const int kMaxNumVertices = 10; // Number of vertices on circle.


float theta_angle = 0.0f;

GLuint vao, ibo, points_vbo, colours_vbo, mvp_ID;
GLuint ibo2, points_vbo2, colours_vbo2, mvp_ID2;
glm::mat4 MVP, View, Projection;
bool forwardPressed = 0, backPressed = 0, 
	is_move_left_pressed = 0, is_move_right_pressed = 0,
	is_move_up_pressed = 0, is_move_down_pressed = 0; // 2 bytes.
int num_of_cubes = 2;

std::array<glm::vec3, kMaxNumVertices> vertices = {};
std::array<glm::vec3, kMaxNumVertices> colors = {};
std::array<glm::vec3, kMaxNumVertices> vertices2 = {};
std::array<glm::vec3, kMaxNumVertices> colors2 = {};

// PRISM DATA
const int kPrismNumVert = 10;
GLfloat prism_vertices[kMaxNumVertices][3] = { 0 }; //2D arr to store X, Y, Z
GLfloat prism_colors[kMaxNumVertices][3] = { 0 }; //2D arr to store R, G, B
GLshort prism_indices[] = {
	// First side to left.
	0, 1, 3,
	3,2,0,
	// second side
	2, 3, 5,
	5,4,2,
	// third side
	4, 5, 7, 
	7,6,4,
	// fourth side
	6, 7, 9, 
	9,8,6,
	// final side
	8, 9, 1,
	1,0,8,
};

// CRYSTAL DATA
const int kCrystalNumVert = 8;
GLfloat crys_vertices[kMaxNumVertices][3] = { 0 }; //2D arr to store X, Y, Z
GLfloat crys_colors[kMaxNumVertices][3] = { 0 }; //2D arr to store R, G, B
GLshort crys_indices[] = {
	0,1,6,
	1,2,6,
	6,2,3,
	6,3,4,
	5,6,4,
	0,6,5,

	3,2,7,
	2,1,7,
	7,1,0,
	7,0,5,
	4,7,5,
	3,7,4
};

void CreatePrism()
{
	theta_angle = 0.0f;
	for (int i = 0; i < kPrismNumVert; ++i)
	{
		float z_pos = 1; //prism has a height of 2. So if i is odd, z_pos of vert = -1
		if (i % 2) {
			z_pos = -1;
		}
		vertices[i] = glm::vec3(circle_x_pos + circle_radius * cos(theta_angle), circle_y_pos + circle_radius * sin(theta_angle), z_pos); //formula to find points on a circle, increasing the angle will result in the next point coords
		colors[i] = glm::vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);

		if (i % 2) {
			theta_angle += 2 * PI / (kPrismNumVert/2);
		}

	}

	for (int i = 0; i < kPrismNumVert; ++i) {

		prism_vertices[i][0] = vertices[i][0];
		prism_vertices[i][1] = vertices[i][1];
		prism_vertices[i][2] = vertices[i][2];

		prism_colors[i][0] = colors[i][0];
		prism_colors[i][1] = colors[i][1];
		prism_colors[i][2] = colors[i][2];
	}
}

void CreateCrystal()
{
	theta_angle = 0.0f;
	for (int i = 0; i < 6; ++i)
	{
		
		vertices2[i] = glm::vec3(circle_x_pos + circle_radius * cos(theta_angle), circle_y_pos + circle_radius * sin(theta_angle), 0); //formula to find points on a circle, increasing the angle will result in the next point coords
		colors2[i] = glm::vec3(0.0f,1.0f,0.0f);

		if (i % 2) {
			theta_angle += 2 * PI / (6);
		}

	}

	vertices2[6] = glm::vec3(0.0f, 0.0f, 2.0f);
	colors2[6] = glm::vec3(0.0f, 1.0f, 0.0f);
	
	vertices2[7] = glm::vec3(0.0f, 0.0f, -2.0f);
	colors2[7] = glm::vec3(0.0f, 1.0f, 0.0f);


	for (int i = 0; i < kCrystalNumVert; ++i) {

		crys_vertices[i][0] = vertices2[i][0];
		crys_vertices[i][1] = vertices2[i][1];
		crys_vertices[i][2] = vertices2[i][2];

		crys_colors[i][0] = colors2[i][0];
		crys_colors[i][1] = colors2[i][1];
		crys_colors[i][2] = colors2[i][2];
	}
}

// Cube data.
GLshort cube_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	7, 4, 0, 3,
	// Bottom.
	0, 4, 5, 1,
	// Right.
	2, 1, 5, 6,
	// Back.
	5, 4, 7, 6,
	// Top.
	2, 6, 7, 3
};

GLfloat cube_vertices[] = {
	-0.9f, -0.9f, 0.9f,		// 0.
	0.9f, -0.9f, 0.9f,		// 1.
	0.9f, 0.9f, 0.9f,		// 2.
	-0.9f, 0.9f, 0.9f,		// 3.
	-0.9f, -0.9f, -0.9f,	// 4.
	0.9f, -0.9f, -0.9f,		// 5.
	0.9f, 0.9f, -0.9f,		// 6.
	-0.9f, 0.9f, -0.9f,		// 7.
};

GLfloat cube_colours[] = {
	0.976f, 0.835f, 0.898f,		// 0.
	0.933f, 0.675f, 0.600f,		// 1.
	0.878f, 0.388f, 0.780f,		// 2.
	0.784f, 0.200f, 0.286f,		// 3.
	0.357f, 0.604f, 0.627f,		// 4.
	0.839f, 0.831f, 0.878f,		// 5.
	0.722f, 0.663f, 0.788f,		// 6.
	0.384f, 0.145f, 0.412f		// 7.
};

// Wireframe data.

GLshort wire_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	7, 4, 0, 3,
	// Bottom.
	0, 4, 5, 1,
	// Right.
	2, 1, 5, 6,
	// Back.
	5, 4, 7, 6,
	// Top.
	2, 6, 7, 3
};

GLfloat wire_vertices[] = {
	-0.9f, -0.9f, 0.9f,		// 0.
	0.9f, -0.9f, 0.9f,		// 1.
	0.9f, 0.9f, 0.9f,		// 2.
	-0.9f, 0.9f, 0.9f,		// 3.
	-0.9f, -0.9f, -0.9f,	// 4.
	0.9f, -0.9f, -0.9f,		// 5.
	0.9f, 0.9f, -0.9f,		// 6.
	-0.9f, 0.9f, -0.9f,		// 7.
};

GLfloat wire_colours[] = {
	1.0f, 1.0f, 1.0f,		// 0.
	1.0f, 1.0f, 1.0f,		// 1.
	1.0f, 1.0f, 1.0f,		// 2.
	1.0f, 1.0f, 1.0f,		// 3.
	1.0f, 1.0f, 1.0f,		// 4.
	1.0f, 1.0f, 1.0f,		// 5.
	1.0f, 1.0f, 1.0f,		// 6.
	1.0f, 1.0f, 1.0f		// 7.
};

float scale = 0.5f, angle1 = 0.0f, angle2 = 0.0f, cam_pos_x = 0.0f, cam_pos_y = 0.0f, cam_pos_z = 5.0f;
glm::vec3 origin_point = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, cam_pos_z);
glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = world_up;
glm::vec3 cam_right;
GLfloat pitch, yaw;

void timer(int);

void resetView()
{
	cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
	cam_up = glm::vec3(0, 1, 0);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

void init(void)
{
	srand((unsigned)time(NULL));
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "GAME2012_Midterm_LeTrung_triangles.vert" },
		{ GL_FRAGMENT_SHADER, "GAME2012_Midterm_LeTrung_triangles.frag" },
		{ GL_NONE, NULL }
	};

	//Loading and compiling shaders
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		cam_pos,  // Camera is at (0,0,5), in World Space
		origin_point,	   // and looks at the origin
		cam_up    // Head is up (set to 0,-1,0 to look upside-down)
	);
	
	resetView();

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		ibo = 0;
		glGenBuffers(1, &ibo);
		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wire_indices), wire_indices, GL_STATIC_DRAW);*/

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(wire_vertices), wire_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		colours_vbo = 0;
		glGenBuffers(1, &colours_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(wire_colours), wire_colours, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.
		
		ibo2 = 1;
		glGenBuffers(1, &ibo2);
		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wire_indices), wire_indices, GL_STATIC_DRAW);*/

		points_vbo2 = 1;
		glGenBuffers(1, &points_vbo2);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo2);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(wire_vertices), wire_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		colours_vbo2 = 1;
		glGenBuffers(1, &colours_vbo2);
		glBindBuffer(GL_ARRAY_BUFFER, colours_vbo2);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(wire_colours), wire_colours, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	CreatePrism();
	CreateCrystal();

	// Enable depth test and face culling.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	// CHANGE BG COLOR
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	timer(0);


}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	/*cam_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam_front.y = sin(glm::radians(pitch));
	cam_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam_front = glm::normalize(cam_front);
	cam_right = glm::normalize(glm::cross(cam_front, world_up));
	cam_up = glm::normalize(glm::cross(cam_right, cam_front));*/

	View = glm::lookAt(
		cam_pos,
		origin_point,
		cam_up);
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
	Model = glm::scale(Model, glm::vec3(scale));
	
	//View = glm::lookAt(
	//	glm::vec3(cam_pos_x, cam_pos_y, cam_pos_z),  // Camera is at (0,0,3), in World Space
	//	glm::vec3(0, 0, 0),	   // and looks at the origin
	//	glm::vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	//);

	calculateView();
	
	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
}

void DrawPrism(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prism_indices), prism_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prism_vertices), prism_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prism_colors), prism_colors, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_SHORT, 0);
}

void DrawCrystal(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crys_indices), crys_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crys_vertices), crys_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crys_colors), crys_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void drawCube(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colours), cube_colours, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawWire(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wire_indices), wire_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wire_vertices), wire_vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wire_colours), wire_colours, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.
		
	glLineWidth(4.0f); // Thicken up the rendered lines.
	glDrawElements(GL_LINE_LOOP, 24, GL_UNSIGNED_SHORT, 0);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	
	angle1 += 1.0f;
	angle2 -= 1.0f;

	//for (int i = 0; i < num_of_cubes; i++) {
	//	float pos = 0.45f;
	//	float pos_mod = i* pos;
	//	int sign = 1;
	//	float angle = angle1;
	//	if (i % 2) { //odd
	//		sign = -1;
	//		angle = angle2;
	//		pos_mod = (i-1)*pos;
	//	}

	//	transformObject(scale, XY_AXIS, angle, glm::vec3(0.0f, pos * sign + pos_mod, 0.0f));
	//	//Ordering the GPU to start the pipeline
	//	drawCube();
	//	drawWire();
	//}
	//transformObject(glm::vec3(1.5f, 1.0f, 1.5f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//transformObject(0.25, Y_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	
	// PRISM DRAW
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::rotate(Model, glm::radians(-90.0f), X_AXIS);
	Model = glm::scale(Model, glm::vec3(1.5f, 1.5f, 0.25f));

	//View = glm::lookAt(
	//	glm::vec3(cam_pos_x, cam_pos_y, cam_pos_z),  // Camera is at (0,0,3), in World Space
	//	glm::vec3(0, 0, 0),	   // and looks at the origin
	//	glm::vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	//);

	calculateView();

	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
	DrawPrism();

	// CRYSTAL DRAW
	Model = glm::mat4(1.0f);
	Model = glm::rotate(Model, glm::radians(-90.0f), X_AXIS);
	Model = glm::scale(Model, glm::vec3(0.5f, 0.5f, 0.5f));

	//View = glm::lookAt(
	//	glm::vec3(cam_pos_x, cam_pos_y, cam_pos_z),  // Camera is at (0,0,3), in World Space
	//	glm::vec3(0, 0, 0),	   // and looks at the origin
	//	glm::vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	//);

	calculateView();

	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);

	DrawCrystal();

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glutSwapBuffers(); // Now for a potentially smoother render.
}

//void idle()
//{
//	glutPostRedisplay(); // Tells glut to redisplay the current window.
//}

void MoveCamRight() {
	float distance = glm::distance(cam_pos, origin_point);
	cam_pos.x += distance * sin(SPEED);
	cam_pos.z += distance * cos(SPEED);
}

void MoveCamUp() {

}

void parseKeys()
{
	if (forwardPressed)
		cam_pos += cam_front * SPEED;
	if (backPressed)
		cam_pos -= cam_front * SPEED;
	if (is_move_left_pressed)
		cam_pos -= glm::normalize(glm::cross(cam_front, cam_up)) * SPEED; // cross product to find right vect, norm to ensure consistent move speed
	if (is_move_right_pressed)
		cam_pos += glm::normalize(glm::cross(cam_front, cam_up)) * SPEED;
	if (is_move_up_pressed)
		cam_pos += cam_up * SPEED;
	if (is_move_down_pressed)
		cam_pos -= cam_up * SPEED;
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
	if (key == 'w')
	{
		if (!forwardPressed)
			forwardPressed = true;
	}
	else if (key == 's')
	{
		if (!backPressed)
			backPressed = true;
	}
	else if (key == 'a')
	{
		if (!is_move_left_pressed)
			is_move_left_pressed = true;
	}
	else if (key == 'd')
	{
		if (!is_move_right_pressed)
			is_move_right_pressed = true;
	}
	else if (key == 'r')
	{
		if (!is_move_up_pressed)
			is_move_up_pressed = true;
	}
	else if (key == 'f')
	{
		if (!is_move_down_pressed)
			is_move_down_pressed = true;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	if (key == 'w')
	{
		forwardPressed = false;
	}
	else if (key == 's')
	{
		backPressed = false;
	}
	else if (key == 'a')
	{
		is_move_left_pressed = false;
	}
	else if (key == 'd')
	{
		is_move_right_pressed = false;
	}
	else if (key == 'r')
	{
		is_move_up_pressed = false;
	}
	else if (key == 'f')
	{
		is_move_down_pressed = false;
	}
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
	glutCreateWindow("GAME2012_Midterm_LeTrung");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp); // New function for third example.

	//glutIdleFunc(idle); // We cannot use this to set the framerate, but we can set a callback to run when the window receives no events.

	glutMainLoop();

	return 0;
}
