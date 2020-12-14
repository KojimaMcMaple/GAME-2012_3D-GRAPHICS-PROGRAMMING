//***************************************************************************
// GAME2012_A3_LeTrung.cpp - 101264698 (C) 2020 All Rights Reserved.
//
// Assignment 3 submission.
//
// Description:
// Click run to see the results.
// Navigation is the same as A2 with WASD and RF keys.
//*****************************************************************************

using namespace std;

#include <cstdlib>
#include <ctime>
#include <iostream>
#include "vgl.h"
#include "GAME2012_A3_LeTrung_LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define PI 3.14159265

#define FPS 60
#define SPEED 0.25f
#define ROTATE_SPEED 1.0f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)

GLuint program;

GLuint vao, ibo, points_vbo, colors_vbo, uv_vbo, mvp_ID;
glm::mat4 MVP, View, Projection;
bool forwardPressed = 0, backPressed = 0, 
	is_move_left_pressed = 0, is_move_right_pressed = 0,
	is_move_up_pressed = 0, is_move_down_pressed = 0; // 2 bytes.
int num_of_cubes = 2;


// Cube data.
GLshort cube_indices[] = {
	// Front.
	0, 1, 2,
	2, 3, 0,
	// Right.
	4, 5, 6,
	6, 7, 4,
	// Back.
	8, 9, 10,
	10, 11, 8,
	// Left.
	12, 13, 14,
	14, 15, 12,
	// Top.
	16, 17, 18,
	18, 19, 16,
	// Bottom.
	20, 21, 22,
	22, 23, 20
};

GLfloat cube_vertices[] = {
	// Front.
	-0.9f, -0.9f, 0.9f,		// 0.
	0.9f, -0.9f, 0.9f,		// 1.
	0.9f, 0.9f, 0.9f,		// 2.
	-0.9f, 0.9f, 0.9f,		// 3.
	// Right.
	0.9f, -0.9f, 0.9f,		// 1. 4
	0.9f, -0.9f, -0.9f,		// 5. 5
	0.9f, 0.9f, -0.9f,		// 6. 6
	0.9f, 0.9f, 0.9f,		// 2. 7
	// Back.
	0.9f, -0.9f, -0.9f,		// 5. 8
	-0.9f, -0.9f, -0.9f,		// 4. 9
	-0.9f, 0.9f, -0.9f,		// 7. 10
	0.9f, 0.9f, -0.9f,		// 6. 11
	// Left.
	-0.9f, -0.9f, -0.9f,		// 4. 12
	-0.9f, -0.9f, 0.9f,		// 0. 13
	-0.9f, 0.9f, 0.9f,		// 3. 14
	-0.9f, 0.9f, -0.9f,		// 7. 15
	// Top.
	-0.9f, 0.9f, -0.9f,		// 7. 16
	-0.9f, 0.9f, 0.9f,		// 3. 17
	0.9f, 0.9f, 0.9f,		// 2. 18
	0.9f, 0.9f, -0.9f,		// 6. 19
	// Bottom.
	-0.9f, -0.9f, -0.9f,		// 4. 20
	0.9f, -0.9f, -0.9f,		// 5. 21
	0.9f, -0.9f, 0.9f,		// 1. 22
	-0.9f, -0.9f, 0.9f		// 0. 23
	
};

GLfloat cube_uvs[] = {
	// Front.
	0.00f, 0.333f, 	// 0.
	0.25f, 0.333f, 	// 1.
	0.25f, 0.666f, 	// 2.
	0.00f, 0.666f,	// 3.
	// Right.
	0.25f, 0.333f, 	// 1.
	0.50f, 0.333f, 	// 5.
	0.50f, 0.666f, 	// 6.
	0.25f, 0.666f,	// 2.
	// Back.
	0.50f, 0.333f, 	// 5.
	0.75f, 0.333f, 	// 4.
	0.75f, 0.666f,	// 7.
	0.50f, 0.666f,	// 6.
	// Left.
	0.75f, 0.333f,		// 4.
	1.00f, 0.333f,		// 0.
	1.00f, 0.666f,		// 3.
	0.75f, 0.666f,		// 7.
	// Top.
	0.25f, 0.666f,		// 7.
	0.50f, 0.666f,		// 3.
	0.50f, 1.00f,		// 2.
	0.25f, 1.00f,		// 6.
	// Bottom.
	0.25f, 0.00f,		// 4.
	0.50f, 0.00f,		// 5.
	0.50f, 0.333f,		// 1.
	0.25f, 0.333f		// 0.
	
};

GLfloat cube_colours[] = {
	//0.7f, 0.0f, 0.0f,
	//0.0f, 0.0f, 0.7f,
	//0.7f, 0.0f, 0.0f,
	//0.7f, 0.0f, 0.0f,
	//0.7f, 0.0f, 0.0f,
	//0.7f, 0.0f, 0.0f,
	//0.7f, 0.0f, 0.0f,
	//0.7f, 0.0f, 0.0f,
	
	//0.976f, 0.835f, 0.898f,		// 0.
	//0.933f, 0.675f, 0.600f,		// 1.
	//0.878f, 0.388f, 0.780f,		// 2.
	//0.784f, 0.200f, 0.286f,		// 3.
	//0.357f, 0.604f, 0.627f,		// 4.
	//0.839f, 0.831f, 0.878f,		// 5.
	//0.722f, 0.663f, 0.788f,		// 6.
	//0.384f, 0.145f, 0.412f		// 7.

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
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

// CAM VARS
float scale = 0.5f, angle1 = 0.0f, angle2 = 0.0f, cam_pos_x = 0.0f, cam_pos_y = 0.0f, cam_pos_z = 15.0f;
glm::vec3 origin_point = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, cam_pos_z);
float cam_angle = 0.0f;
glm::vec3 cam_forward_vector = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = world_up;
glm::vec3 cam_right_vector;
GLfloat pitch, yaw;

// TEX VARS
GLuint texture_id;
GLint width, height, bit_depth;

void timer(int);

void ResetView()
{
	cam_pos = glm::vec3(0.0f, 0.0f, 10.0f);
	cam_forward_vector = glm::vec3(0.0f, 0.0f, -1.0f);
	cam_up = glm::vec3(0, 1, 0);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject

	// Camera matrix
	View = glm::lookAt(
		cam_pos,  // Camera is at (0,0,5), in World Space
		origin_point,	   // and looks at the origin
		cam_up    // Head is up (set to 0,-1,0 to look upside-down)
	);
}

void CreateBuffer() {
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

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(wire_colours), wire_colours, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		
		// UV VALUES
		uv_vbo = 0;
		glGenBuffers(1, &uv_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.
}

void LoadTexture() {
	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load("GAME2012_A3_LeTrung_Tex_d6.png", &width, &height, &bit_depth, 0);
	//unsigned char* image = stbi_load("pben303.jpg", &width, &height, &bit_depth, 0);
	if (!image) {
		cout << "Unable to load file!" << endl;
	}

	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //another option is GL_NEAREST 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //another option is GL_NEAREST 

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up. But we don't want to unbind the texture or we cannot use it.
	stbi_image_free(image);
}

void init(void)
{
	srand((unsigned)time(NULL));
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "GAME2012_A3_LeTrung_triangles.vert" },
		{ GL_FRAGMENT_SHADER, "GAME2012_A3_LeTrung_triangles.frag" },
		{ GL_NONE, NULL }
	};

	//Loading and compiling shaders
	program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates

	ResetView();

	LoadTexture();

	CreateBuffer();

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

	cam_forward_vector = glm::normalize(origin_point - cam_pos);
	cam_right_vector = glm::normalize(glm::cross(world_up, cam_forward_vector));
	cam_up = glm::normalize(glm::cross(cam_forward_vector, cam_right_vector));

	View = glm::lookAt(
		cam_pos,
		origin_point,
		cam_up);
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
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

void drawCube(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colours), cube_colours, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void drawWire(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wire_indices), wire_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wire_vertices), wire_vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
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

	for (int i = 0; i < num_of_cubes; i++) {
		//float pos = 0.45f;
		//float pos_mod = i* pos;
		//int sign = 1;
		//float angle = angle1;
		//if (i % 2) { //odd
		//	sign = -1;
		//	angle = angle2;
		//	pos_mod = (i-1)*pos;
		//}
		float pos = 3.0f;
		int sign = 1;
		float angle = angle1;
		if (i % 2) { //odd
			sign = -1;
			angle = angle2;
		}

		transformObject(scale, XY_AXIS, angle, glm::vec3(pos * sign, 0.0f, 0.0f));
		//transformObject(scale, XY_AXIS, 0, glm::vec3(0.0f, 0.0f, 0.0f));
		
		//Ordering the GPU to start the pipeline
		drawCube();
		drawWire();
	}

	

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glutSwapBuffers(); // Now for a potentially smoother render.
}

//void idle()
//{
//	glutPostRedisplay(); // Tells glut to redisplay the current window.
//}

void MoveCamForward() {
	cam_pos += cam_forward_vector * SPEED;
}
void MoveCamBackward() {
	cam_pos -= cam_forward_vector * SPEED;
}

void MoveCamRight() {
	//float distance = glm::distance(cam_pos, origin_point); //circle radius
	//cam_angle += ROTATE_SPEED * PI / 180;
	//cam_pos.x = distance * sin(cam_angle);
	//cam_pos.z = distance * cos(cam_angle);

	cam_pos += cam_right_vector * SPEED;
}

void MoveCamLeft() {
	//float distance = glm::distance(cam_pos, origin_point); //circle radius
	//cam_angle -= ROTATE_SPEED * PI / 180;
	//cam_pos.x = distance * sin(cam_angle);
	//cam_pos.z = distance * cos(cam_angle);

	cam_pos -= cam_right_vector * SPEED;
}

void MoveCamUp() {
	//float distance = glm::distance(cam_pos, origin_point); //circle radius
	//cam_angle += ROTATE_SPEED * PI / 180;
	//cam_pos.y = distance * sin(cam_angle);
	//cam_pos.z = distance * cos(cam_angle);

	cam_pos += cam_up * SPEED;
}

void MoveCamDown() {
	//float distance = glm::distance(cam_pos, origin_point); //circle radius
	//cam_angle -= ROTATE_SPEED * PI / 180;
	//cam_pos.y = distance * sin(cam_angle);
	//cam_pos.z = distance * cos(cam_angle);

	cam_pos -= cam_up * SPEED;
}

void parseKeys()
{
	if (forwardPressed)
		MoveCamForward();
	if (backPressed)
		MoveCamBackward();
	if (is_move_left_pressed)
		//cam_pos -= glm::normalize(glm::cross(cam_front, cam_up)) * SPEED; // cross product to find right vect, norm to ensure consistent move speed
		MoveCamLeft();
	if (is_move_right_pressed)
		//cam_pos += glm::normalize(glm::cross(cam_front, cam_up)) * SPEED;
		MoveCamRight();
	if (is_move_up_pressed)
		//cam_pos += cam_up * SPEED;
		MoveCamUp();
	if (is_move_down_pressed)
		//cam_pos -= cam_up * SPEED;
		MoveCamDown();
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
	/*cout << "Please enter the number of cubes you want to draw: ";
	cin >> num_of_cubes;
	if (num_of_cubes < 1) {
		cout << "Input number < 1, defaulting to 2!";
		num_of_cubes = 2;
	}*/
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitWindowSize(800, 800);
	glutCreateWindow("GAME2012_A3_LeTrung");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp); // New function for third example.

	//glutIdleFunc(idle); // We cannot use this to set the framerate, but we can set a callback to run when the window receives no events.

	glutMainLoop();

	return 0;
}
