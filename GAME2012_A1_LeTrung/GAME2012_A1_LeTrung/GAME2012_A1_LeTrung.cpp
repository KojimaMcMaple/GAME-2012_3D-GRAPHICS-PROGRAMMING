//***************************************************************************
// GAME2012 - Assignment1.cpp by Trung Le (Kyle) - 101264698
//
// Assignment 1 submission.
//
// Description:
// Click run to see the results.
//
//****************************************************************************

using namespace std;
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#define PI 3.14159265
#define FPS 30
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint Buffers[NumBuffers];

const GLuint NumVertices = 10;
const GLfloat scale = 0.5f;
GLfloat vertices[NumVertices][2];

GLuint vao, points_vbo, colors_vbo, model_matrix_id;

vector<GLuint> vao_ids;
int num_of_squares = 10;
float angle_of_rotation = 45.0f;

GLfloat points[] = {
	-0.9f,  0.9f,  0.0f,
	0.9f,  0.9f,  0.0f,
	0.9f, -0.9f,  0.0f,
	-0.9f, -0.9f,  0.0f 
};

GLfloat colors[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f
};

void timer(int); 

void init(void)
{
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "shader_square.vert" },
		{ GL_FRAGMENT_SHADER, "shader_square.frag" },
		{ GL_NONE, NULL }
	};

	//Loading and compiling shaders
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	model_matrix_id = glGetUniformLocation(program, "model_matrix");

	vao = 0;

	for (int i = 0; i < num_of_squares; i++)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		vao_ids.push_back(vao);

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		// RANDOMIZE SQUARE COLORS
		float red_value = (float)(rand() % 256) / 255;
		float green_value = (float)(rand() % 256) / 255;
		float blue_value = (float)(rand() % 256) / 255;
		for (int i = 0; i < 4 * 3; i += 3) {
			colors[i] = red_value;
		}
		for (int i = 1; i < 4 * 3; i += 3) {
			colors[i] = green_value;
		}
		for (int i = 2; i < 4 * 3; i += 3) {
			colors[i] = blue_value;
		}

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}
	
	// CHANGE BG COLOR
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	////Generating two buffers, one is used to store the coordinates of the vertices
	////The other one is not used. Just wanted to show that we can allocate as many as buffers, some of which might left unused.
	//glGenBuffers(2, Buffers);
	//glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	//glBindAttribLocation(program, 0, "vPosition");
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//glEnableVertexAttribArray(0);

	timer(0); 
}

void TransformObject(float scale_factor, glm::vec3 rot_axis, float rot_angle, glm::vec3 translation) {
	glm::mat4 model_matrix;
	model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, translation);
	model_matrix = glm::rotate(model_matrix, glm::radians(rot_angle), rot_axis);
	model_matrix = glm::scale(model_matrix, glm::vec3(scale_factor));
	glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	////Selecting the buffer
	//glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	////Randomizing vertices coordinates
	//for (int i = 0; i < NumVertices; i++)
	//{
	//	float x = (rand() % 200 + 1) / 100.0f - 1;
	//	float y = (rand() % 200 + 1) / 100.0f - 1;
	//	vertices[i][0] = x;
	//	vertices[i][1] = y;

	//}

	////Pushing the coordinates of the vertices into the buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	////Ordering the GPU to start the pipeline
	//glDrawArrays(GL_LINE_STRIP, 0, NumVertices);
	
	// DRAW INITIAL SQUARE
	glBindVertexArray(vao_ids[0]);
	TransformObject(1.0f, Z_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	// DRAW INNER SQUARES
	float edge_len = abs(points[0]) + abs(points[3]);
	float former_edge_len = edge_len;
	float adjacent_edge_len;
	float hypotenuse_edge_len;
	if (vao_ids.size() > 1) {
		for (int i = 1; i < vao_ids.size(); i++) {
			glBindVertexArray(vao_ids[i]);
			
			// CALCULATION OF NEW EDGE LENGTH
			float scale_factor = 1.0f;
			/*if (angle_of_rotation == 45.0f) { //OBSOLETE
				edge_len = sqrt(pow(edge_len / 2, 2) + pow(edge_len / 2, 2));
				scale_factor = edge_len / former_edge_len;
			}
			else {
				
			}*/
			/*
			==============================================================================
			EXPLANATION: Let's say opposite_edge + adjacent_edge = edge_length_of_the_square. (1)
			Since tan(A) = opp/adj, we can say opp = adj * tan(A). (2)
			Plugging (2) to (1), we have adj*tan(A) + adj = edge_length_of_the_square,
			now we have adj = edge_length_of_the_square / (1 + tan(A)). (3)
			From (3), we now have adj & A, Easy Mode is now selectable as the hypotenuse_edge can be found by
			adj / cos(A)
			==============================================================================
			*/
			adjacent_edge_len = edge_len / (1 + tan(angle_of_rotation * PI / 180.0f));
			edge_len = adjacent_edge_len / (cos(angle_of_rotation * PI / 180.0f));
			scale_factor = edge_len / former_edge_len;

			// ONLY ROTATE EVERY 2ND SQUARE
			float rot_angle = 0.0f;
			if (i % 2) {
				rot_angle = angle_of_rotation;
			}
			TransformObject(scale_factor, Z_AXIS, rot_angle, glm::vec3(0.0f, 0.0f, 0.0f));
			
			//Ordering the GPU to start the pipeline
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}
	}
	
	
	
	//glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glFlush();
}

void idle()
{
	//glutPostRedisplay(); // Tells glut to redisplay the current window.
}

void timer(int) {
	//glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer, 0); // 60 FPS or 16.67ms.
}

//---------------------------------------------------------------------
//
// main
//

void UserInputForValues() {
	cout << "Please enter the number of squares you want to draw: ";
	cin >> num_of_squares;
	if (num_of_squares < 1) {
		cout << "Input number < 1, defaulting to 10!";
		num_of_squares = 10;
	}
	cout << "Please enter the angle that the inner squares will be rotated: ";
	cin >> angle_of_rotation;
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	UserInputForValues();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(720, 720);
	
	glutCreateWindow("Le_Trung_101264698");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
		
	init();

	glutDisplayFunc(display);
	
	//glutIdleFunc(idle); // We cannot use this to set the framerate, but we can set a callback to run when the window receives no events.

	glutMainLoop();

	return 0;
}
