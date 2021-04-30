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
#define MOVESPEED 0.2f
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
GLuint kBlankTexture, kGroundTexture, kHedgeTexture, kRoomTexture,
	kWallTexture, kStairTexture, kCrystalTexture,
	kTowerRoofTexture, kTowerBaseTexture, kGateTexture;
GLint width, height, bitDepth;

// Light variables.
AmbientLight aLight(glm::vec3(1.0f, 1.0f, 1.0f),	// Ambient colour.
	0.8f);							// Ambient strength.

const int kPointLightSize = 8;
PointLight pLights[kPointLightSize] = { { glm::vec3(6.0f, 1.25f, -6.0f), 10.0f, glm::vec3(1.0f, 0.4f, 0.0f), 5.0f },
										{ glm::vec3(6.0f, 1.25f, -24.0f), 10.0f, glm::vec3(1.0f, 0.4f, 0.0f), 5.0f },
										{ glm::vec3(24.0f, 1.25f, -24.0f), 10.0f, glm::vec3(1.0f, 0.4f, 0.0f), 5.0f },
										{ glm::vec3(24.0f, 1.25f, -6.0f), 10.0f, glm::vec3(1.0f, 0.4f, 0.0f), 5.0f },

										{ glm::vec3(15.0f, 1.0f, -3.0f), 5.0f, glm::vec3(1.0f, 0.0f, 0.0f), 7.0f },
										{ glm::vec3(15.0f, 1.0f, -27.0f), 5.0f, glm::vec3(0.0f, 1.0f, 0.0f), 7.0f },
										{ glm::vec3(4.0f, 1.25f, -15.0f), 5.0f, glm::vec3(0.78f, 0.14f, 0.69f), 5.0f },
										{ glm::vec3(26.0f, 1.25f, -15.0f), 5.0f, glm::vec3(0.08f, 0.96f, 0.93f), 5.0f },
};

SpotLight sLight(glm::vec3(15.5f, 5.0f, -15.5f),	// Position.
	glm::vec3(1.0f, 0.72f, 0.1f),	// Diffuse colour.
	1.0f,							// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f),  // Direction.
	40.0f);

// Shapes. Recommend putting in a map
Grid kGroundShape(30, 20); // New UV scale parameter. Works with texture now.

// TOWER
Cone roof(24, 2);
Prism baseTower(24), baseGatehouse(8);

// CASTLE WALL
Cube castleWall;

// CRYSTAL KEY
Crystal kCrystalKey(1);
float kCrystalRotAngle = 0.0f;
glm::vec3 kCrystalScale = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 kCrystalPos = glm::vec3(26.5f, 0.5f, -14.5f);
bool kIsCrystalActive = true;

// RECEPTOR
Prism kReceptor(24);
glm::vec3 kReceptorPos = glm::vec3(15.5f, 0.0f, -15.5f);
bool kIsReceptorActive = false;

// MAZE
glm::vec3 kExitPoint = glm::vec3(14.5f, 0.5f, -27.0f); //green light
const float kMazeWallThickness = 0.2f;
const int kMazeHorizontalSize = 304;
const int kMazeVerticalSize = 318;
const int kMazeSize = 622;
Wall* kMazeHorizontal = new Wall[kMazeHorizontalSize];
Wall* kMazeVertical = new Wall[kMazeVerticalSize];
glm::vec3 kMazeHorizontalCoords[kMazeHorizontalSize] = {
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
		glm::vec3(1.0f, 0.0f, -1.0f), //01
		glm::vec3(4.0f, 0.0f, -1.0f), //04
		glm::vec3(6.0f, 0.0f, -1.0f), //06
		glm::vec3(7.0f, 0.0f, -1.0f), //07
		glm::vec3(10.0f, 0.0f, -1.0f), //10
		glm::vec3(11.0f, 0.0f, -1.0f), //11
		glm::vec3(12.0f, 0.0f, -1.0f), //12
		glm::vec3(17.0f, 0.0f, -1.0f), //17
		glm::vec3(18.0f, 0.0f, -1.0f), //18
		glm::vec3(19.0f, 0.0f, -1.0f), //19
		glm::vec3(22.0f, 0.0f, -1.0f), //22
	//////////ROW #2
		glm::vec3(5.0f, 0.0f, -2.0f), //05
		glm::vec3(7.0f, 0.0f, -2.0f), //07
		glm::vec3(8.0f, 0.0f, -2.0f), //08
		glm::vec3(11.0f, 0.0f, -2.0f), //11
		glm::vec3(13.0f, 0.0f, -2.0f), //13
		glm::vec3(14.0f, 0.0f, -2.0f), //14
		glm::vec3(21.0f, 0.0f, -2.0f), //21
	//////////ROW #3
		glm::vec3(3.0f, 0.0f, -3.0f), //03
		glm::vec3(6.0f, 0.0f, -3.0f), //06
		glm::vec3(8.0f, 0.0f, -3.0f), //08
		glm::vec3(9.0f, 0.0f, -3.0f), //09
		glm::vec3(12.0f, 0.0f, -3.0f), //12
		glm::vec3(13.0f, 0.0f, -3.0f), //13
		glm::vec3(14.0f, 0.0f, -3.0f), //14
		glm::vec3(15.0f, 0.0f, -3.0f), //15
		glm::vec3(16.0f, 0.0f, -3.0f), //16
		glm::vec3(17.0f, 0.0f, -3.0f), //17
		glm::vec3(20.0f, 0.0f, -3.0f), //20
		glm::vec3(22.0f, 0.0f, -3.0f), //22
		glm::vec3(23.0f, 0.0f, -3.0f), //23
	//////////ROW #4
		glm::vec3(0.0f, 0.0f, -4.0f), //00
		glm::vec3(1.0f, 0.0f, -4.0f), //01
		glm::vec3(2.0f, 0.0f, -4.0f), //02
		glm::vec3(5.0f, 0.0f, -4.0f), //05
		glm::vec3(8.0f, 0.0f, -4.0f), //08
		glm::vec3(9.0f, 0.0f, -4.0f), //09
		glm::vec3(10.0f, 0.0f, -4.0f), //10
		glm::vec3(11.0f, 0.0f, -4.0f), //11
		glm::vec3(14.0f, 0.0f, -4.0f), //14
		glm::vec3(15.0f, 0.0f, -4.0f), //15
		glm::vec3(16.0f, 0.0f, -4.0f), //16
		glm::vec3(20.0f, 0.0f, -4.0f), //20
		glm::vec3(21.0f, 0.0f, -4.0f), //21
		glm::vec3(22.0f, 0.0f, -4.0f), //22
	//////////ROW #5
		glm::vec3(3.0f, 0.0f, -5.0f), //03
		glm::vec3(4.0f, 0.0f, -5.0f), //04
		glm::vec3(5.0f, 0.0f, -5.0f), //05
		glm::vec3(6.0f, 0.0f, -5.0f), //06
		glm::vec3(7.0f, 0.0f, -5.0f), //07
		glm::vec3(11.0f, 0.0f, -5.0f), //11
		glm::vec3(12.0f, 0.0f, -5.0f), //12
		glm::vec3(16.0f, 0.0f, -5.0f), //16
		glm::vec3(17.0f, 0.0f, -5.0f), //17
		glm::vec3(19.0f, 0.0f, -5.0f), //19
	//////////ROW #6
		glm::vec3(1.0f, 0.0f, -6.0f), //01
		glm::vec3(2.0f, 0.0f, -6.0f), //02
		glm::vec3(4.0f, 0.0f, -6.0f), //04
		glm::vec3(7.0f, 0.0f, -6.0f), //07
		glm::vec3(8.0f, 0.0f, -6.0f), //08
		glm::vec3(20.0f, 0.0f, -6.0f), //20
		glm::vec3(21.0f, 0.0f, -6.0f), //21
	//////////ROW #7
		glm::vec3(2.0f, 0.0f, -7.0f), //02
		glm::vec3(3.0f, 0.0f, -7.0f), //03
		glm::vec3(6.0f, 0.0f, -7.0f), //06
		glm::vec3(8.0f, 0.0f, -7.0f), //08
		glm::vec3(9.0f, 0.0f, -7.0f), //09
		glm::vec3(10.0f, 0.0f, -7.0f), //10
		glm::vec3(11.0f, 0.0f, -7.0f), //11
		glm::vec3(13.0f, 0.0f, -7.0f), //13
		glm::vec3(16.0f, 0.0f, -7.0f), //16
		glm::vec3(17.0f, 0.0f, -7.0f), //17
		glm::vec3(19.0f, 0.0f, -7.0f), //19
		glm::vec3(20.0f, 0.0f, -7.0f), //20
	//////////ROW #8
		glm::vec3(5.0f, 0.0f, -8.0f), //05
		glm::vec3(7.0f, 0.0f, -8.0f), //07
		glm::vec3(9.0f, 0.0f, -8.0f), //09
		glm::vec3(12.0f, 0.0f, -8.0f), //12
		glm::vec3(14.0f, 0.0f, -8.0f), //14
		glm::vec3(15.0f, 0.0f, -8.0f), //15
		glm::vec3(18.0f, 0.0f, -8.0f), //18
		glm::vec3(21.0f, 0.0f, -8.0f), //21
		glm::vec3(22.0f, 0.0f, -8.0f), //22
	//////////ROW #9
		glm::vec3(2.0f, 0.0f, -9.0f), //02
		glm::vec3(3.0f, 0.0f, -9.0f), //03
		glm::vec3(6.0f, 0.0f, -9.0f), //06
		glm::vec3(7.0f, 0.0f, -9.0f), //07
		glm::vec3(8.0f, 0.0f, -9.0f), //08
		glm::vec3(10.0f, 0.0f, -9.0f), //10
		glm::vec3(11.0f, 0.0f, -9.0f), //11
		glm::vec3(12.0f, 0.0f, -9.0f), //12
		glm::vec3(13.0f, 0.0f, -9.0f), //13
		glm::vec3(16.0f, 0.0f, -9.0f), //16
		glm::vec3(17.0f, 0.0f, -9.0f), //17
		glm::vec3(19.0f, 0.0f, -9.0f), //19
		glm::vec3(20.0f, 0.0f, -9.0f), //20
		glm::vec3(23.0f, 0.0f, -9.0f), //23
	//////////ROW #10
		glm::vec3(1.0f, 0.0f, -10.0f), //01
		glm::vec3(3.0f, 0.0f, -10.0f), //03
		glm::vec3(4.0f, 0.0f, -10.0f), //04
		glm::vec3(5.0f, 0.0f, -10.0f), //05
		glm::vec3(9.0f, 0.0f, -10.0f), //09
		glm::vec3(10.0f, 0.0f, -10.0f), //10
		glm::vec3(11.0f, 0.0f, -10.0f), //11
		glm::vec3(14.0f, 0.0f, -10.0f), //14
		glm::vec3(17.0f, 0.0f, -10.0f), //17
		glm::vec3(20.0f, 0.0f, -10.0f), //20
		glm::vec3(21.0f, 0.0f, -10.0f), //21
		glm::vec3(22.0f, 0.0f, -10.0f), //22
	//////////ROW #11
		glm::vec3(1.0f, 0.0f, -11.0f), //01
		glm::vec3(8.0f, 0.0f, -11.0f), //08
		glm::vec3(12.0f, 0.0f, -11.0f), //12
		glm::vec3(13.0f, 0.0f, -11.0f), //13
		glm::vec3(14.0f, 0.0f, -11.0f), //14
		glm::vec3(15.0f, 0.0f, -11.0f), //15
		glm::vec3(17.0f, 0.0f, -11.0f), //17
		glm::vec3(18.0f, 0.0f, -11.0f), //18
		glm::vec3(23.0f, 0.0f, -11.0f), //23
	//////////ROW #12
		glm::vec3(2.0f, 0.0f, -12.0f), //02
		glm::vec3(3.0f, 0.0f, -12.0f), //03
		glm::vec3(4.0f, 0.0f, -12.0f), //04
		glm::vec3(6.0f, 0.0f, -12.0f), //06
		glm::vec3(7.0f, 0.0f, -12.0f), //07
		glm::vec3(9.0f, 0.0f, -12.0f), //09
		//glm::vec3(10.0f, 0.0f, -12.0f), //10
		//glm::vec3(11.0f, 0.0f, -12.0f), //11
		//glm::vec3(12.0f, 0.0f, -12.0f), //12
		glm::vec3(19.0f, 0.0f, -12.0f), //19
		glm::vec3(20.0f, 0.0f, -12.0f), //20
	//////////ROW #13
		glm::vec3(1.0f, 0.0f, -13.0f), //01
		glm::vec3(4.0f, 0.0f, -13.0f), //04
		glm::vec3(5.0f, 0.0f, -13.0f), //05
		//glm::vec3(10.0f, 0.0f, -13.0f), //10
		//glm::vec3(11.0f, 0.0f, -13.0f), //11
		//glm::vec3(12.0f, 0.0f, -13.0f), //12
		//glm::vec3(13.0f, 0.0f, -13.0f), //13
		glm::vec3(15.0f, 0.0f, -13.0f), //15
		glm::vec3(16.0f, 0.0f, -13.0f), //16
		glm::vec3(17.0f, 0.0f, -13.0f), //17
		glm::vec3(18.0f, 0.0f, -13.0f), //18
		glm::vec3(19.0f, 0.0f, -13.0f), //19
		glm::vec3(22.0f, 0.0f, -13.0f), //22
	//////////ROW #14
		glm::vec3(0.0f, 0.0f, -14.0f), //00
		glm::vec3(2.0f, 0.0f, -14.0f), //02
		glm::vec3(3.0f, 0.0f, -14.0f), //03
		glm::vec3(6.0f, 0.0f, -14.0f), //06
		glm::vec3(10.0f, 0.0f, -14.0f), //10
		glm::vec3(11.0f, 0.0f, -14.0f), //11
		glm::vec3(12.0f, 0.0f, -14.0f), //12
		glm::vec3(13.0f, 0.0f, -14.0f), //13
		glm::vec3(14.0f, 0.0f, -14.0f), //14
		glm::vec3(15.0f, 0.0f, -14.0f), //15
		glm::vec3(17.0f, 0.0f, -14.0f), //17
		glm::vec3(22.0f, 0.0f, -14.0f), //22
	//////////ROW #15
		glm::vec3(0.0f, 0.0f, -15.0f), //00
		glm::vec3(1.0f, 0.0f, -15.0f), //01
		glm::vec3(4.0f, 0.0f, -15.0f), //04
		glm::vec3(7.0f, 0.0f, -15.0f), //07
		glm::vec3(8.0f, 0.0f, -15.0f), //08
		glm::vec3(9.0f, 0.0f, -15.0f), //09
		glm::vec3(16.0f, 0.0f, -15.0f), //16
		glm::vec3(19.0f, 0.0f, -15.0f), //19
		glm::vec3(20.0f, 0.0f, -15.0f), //20
		glm::vec3(23.0f, 0.0f, -15.0f), //23
	//////////ROW #16
		glm::vec3(2.0f, 0.0f, -16.0f), //02
		glm::vec3(3.0f, 0.0f, -16.0f), //03
		glm::vec3(4.0f, 0.0f, -16.0f), //04
		glm::vec3(5.0f, 0.0f, -16.0f), //05
		glm::vec3(8.0f, 0.0f, -16.0f), //08
		glm::vec3(9.0f, 0.0f, -16.0f), //09
		glm::vec3(10.0f, 0.0f, -16.0f), //10
		glm::vec3(13.0f, 0.0f, -16.0f), //13
		glm::vec3(15.0f, 0.0f, -16.0f), //15
		glm::vec3(17.0f, 0.0f, -16.0f), //17
		glm::vec3(18.0f, 0.0f, -16.0f), //18
		glm::vec3(20.0f, 0.0f, -16.0f), //20
	//////////ROW #17
		glm::vec3(3.0f, 0.0f, -17.0f), //03
		glm::vec3(4.0f, 0.0f, -17.0f), //04
		glm::vec3(7.0f, 0.0f, -17.0f), //07
		glm::vec3(10.0f, 0.0f, -17.0f), //10
		glm::vec3(11.0f, 0.0f, -17.0f), //11
		glm::vec3(12.0f, 0.0f, -17.0f), //12
		glm::vec3(13.0f, 0.0f, -17.0f), //13
		glm::vec3(14.0f, 0.0f, -17.0f), //14
		glm::vec3(16.0f, 0.0f, -17.0f), //16
		glm::vec3(19.0f, 0.0f, -17.0f), //19
		glm::vec3(22.0f, 0.0f, -17.0f), //22
	//////////ROW #18
		glm::vec3(1.0f, 0.0f, -18.0f), //01
		glm::vec3(2.0f, 0.0f, -18.0f), //02
		glm::vec3(4.0f, 0.0f, -18.0f), //04
		glm::vec3(5.0f, 0.0f, -18.0f), //05
		glm::vec3(6.0f, 0.0f, -18.0f), //06
		glm::vec3(8.0f, 0.0f, -18.0f), //08
		glm::vec3(9.0f, 0.0f, -18.0f), //09
		glm::vec3(11.0f, 0.0f, -18.0f), //11
		glm::vec3(12.0f, 0.0f, -18.0f), //12
		glm::vec3(13.0f, 0.0f, -18.0f), //13
		glm::vec3(14.0f, 0.0f, -18.0f), //14
		glm::vec3(15.0f, 0.0f, -18.0f), //15
		glm::vec3(17.0f, 0.0f, -18.0f), //17
		glm::vec3(18.0f, 0.0f, -18.0f), //18
		glm::vec3(19.0f, 0.0f, -18.0f), //19
		glm::vec3(21.0f, 0.0f, -18.0f), //21
	//////////ROW #19
		glm::vec3(0.0f, 0.0f, -19.0f), //00
		glm::vec3(1.0f, 0.0f, -19.0f), //01
		glm::vec3(3.0f, 0.0f, -19.0f), //03
		glm::vec3(4.0f, 0.0f, -19.0f), //04
		glm::vec3(6.0f, 0.0f, -19.0f), //06
		glm::vec3(7.0f, 0.0f, -19.0f), //07
		glm::vec3(16.0f, 0.0f, -19.0f), //16
		glm::vec3(17.0f, 0.0f, -19.0f), //17
		glm::vec3(20.0f, 0.0f, -19.0f), //20
		glm::vec3(22.0f, 0.0f, -19.0f), //22
	//////////ROW #20
		glm::vec3(3.0f, 0.0f, -20.0f), //03
		glm::vec3(5.0f, 0.0f, -20.0f), //05
		glm::vec3(6.0f, 0.0f, -20.0f), //06
		glm::vec3(11.0f, 0.0f, -20.0f), //11
		glm::vec3(12.0f, 0.0f, -20.0f), //12
		glm::vec3(21.0f, 0.0f, -20.0f), //21
		glm::vec3(23.0f, 0.0f, -20.0f), //23
	//////////ROW #21
		glm::vec3(1.0f, 0.0f, -21.0f), //01
		glm::vec3(4.0f, 0.0f, -21.0f), //04
		glm::vec3(6.0f, 0.0f, -21.0f), //06
		glm::vec3(7.0f, 0.0f, -21.0f), //07
		glm::vec3(9.0f, 0.0f, -21.0f), //09
		glm::vec3(10.0f, 0.0f, -21.0f), //10
		glm::vec3(14.0f, 0.0f, -21.0f), //14
		glm::vec3(15.0f, 0.0f, -21.0f), //15
		glm::vec3(16.0f, 0.0f, -21.0f), //16
		glm::vec3(17.0f, 0.0f, -21.0f), //17
		glm::vec3(18.0f, 0.0f, -21.0f), //18
		glm::vec3(22.0f, 0.0f, -21.0f), //22
	//////////ROW #22
		glm::vec3(2.0f, 0.0f, -22.0f), //02
		glm::vec3(4.0f, 0.0f, -22.0f), //04
		glm::vec3(8.0f, 0.0f, -22.0f), //08
		glm::vec3(11.0f, 0.0f, -22.0f), //11
		glm::vec3(12.0f, 0.0f, -22.0f), //12
		glm::vec3(13.0f, 0.0f, -22.0f), //13
		glm::vec3(14.0f, 0.0f, -22.0f), //14
		glm::vec3(15.0f, 0.0f, -22.0f), //15
		glm::vec3(16.0f, 0.0f, -22.0f), //16
		glm::vec3(17.0f, 0.0f, -22.0f), //17
		glm::vec3(18.0f, 0.0f, -22.0f), //18
		glm::vec3(19.0f, 0.0f, -22.0f), //19
		glm::vec3(21.0f, 0.0f, -22.0f), //21
		glm::vec3(22.0f, 0.0f, -22.0f), //22
		glm::vec3(23.0f, 0.0f, -22.0f), //23
	//////////ROW #23
		glm::vec3(1.0f, 0.0f, -23.0f), //01
		glm::vec3(3.0f, 0.0f, -23.0f), //03
		glm::vec3(5.0f, 0.0f, -23.0f), //05
		glm::vec3(6.0f, 0.0f, -23.0f), //06
		glm::vec3(7.0f, 0.0f, -23.0f), //07
		glm::vec3(8.0f, 0.0f, -23.0f), //08
		glm::vec3(9.0f, 0.0f, -23.0f), //09
		glm::vec3(10.0f, 0.0f, -23.0f), //10
		glm::vec3(11.0f, 0.0f, -23.0f), //11
		glm::vec3(20.0f, 0.0f, -23.0f), //20
		glm::vec3(22.0f, 0.0f, -23.0f), //22
	//////////ROW #24
		glm::vec3(0.0f, 0.0f, -24.0f), //00
		glm::vec3(1.0f, 0.0f, -24.0f), //01
		glm::vec3(2.0f, 0.0f, -24.0f), //02
		glm::vec3(3.0f, 0.0f, -24.0f), //03
		glm::vec3(4.0f, 0.0f, -24.0f), //04
		glm::vec3(5.0f, 0.0f, -24.0f), //05
		glm::vec3(6.0f, 0.0f, -24.0f), //06
		glm::vec3(7.0f, 0.0f, -24.0f), //07
		glm::vec3(8.0f, 0.0f, -24.0f), //08
		glm::vec3(9.0f, 0.0f, -24.0f), //09
		glm::vec3(10.0f, 0.0f, -24.0f), //10
		//11
		glm::vec3(12.0f, 0.0f, -24.0f), //12
		glm::vec3(13.0f, 0.0f, -24.0f), //13
		glm::vec3(14.0f, 0.0f, -24.0f), //14
		glm::vec3(15.0f, 0.0f, -24.0f), //15
		glm::vec3(16.0f, 0.0f, -24.0f), //16
		glm::vec3(17.0f, 0.0f, -24.0f), //17
		glm::vec3(18.0f, 0.0f, -24.0f), //18
		glm::vec3(19.0f, 0.0f, -24.0f), //19
		glm::vec3(20.0f, 0.0f, -24.0f), //20
		glm::vec3(21.0f, 0.0f, -24.0f), //21
		glm::vec3(22.0f, 0.0f, -24.0f), //22
		glm::vec3(23.0f, 0.0f, -24.0f) //23
};
glm::vec3 kMazeVerticalCoords[kMazeVerticalSize] = {
	//////////COL #0
		glm::vec3(0.0f, 0.0f, -0.0f), //00
		glm::vec3(0.0f, 0.0f, -1.0f), //01
		glm::vec3(0.0f, 0.0f, -2.0f), //02
		glm::vec3(0.0f, 0.0f, -3.0f), //03
		glm::vec3(0.0f, 0.0f, -4.0f), //04
		glm::vec3(0.0f, 0.0f, -5.0f), //05
		glm::vec3(0.0f, 0.0f, -6.0f), //06
		glm::vec3(0.0f, 0.0f, -7.0f), //07
		glm::vec3(0.0f, 0.0f, -8.0f), //08
		glm::vec3(0.0f, 0.0f, -9.0f), //09
		glm::vec3(0.0f, 0.0f, -10.0f), //10
		glm::vec3(0.0f, 0.0f, -11.0f), //11
		glm::vec3(0.0f, 0.0f, -12.0f), //12
		glm::vec3(0.0f, 0.0f, -13.0f), //13
		glm::vec3(0.0f, 0.0f, -14.0f), //14
		glm::vec3(0.0f, 0.0f, -15.0f), //15
		glm::vec3(0.0f, 0.0f, -16.0f), //16
		glm::vec3(0.0f, 0.0f, -17.0f), //17
		glm::vec3(0.0f, 0.0f, -18.0f), //18
		glm::vec3(0.0f, 0.0f, -19.0f), //19
		glm::vec3(0.0f, 0.0f, -20.0f), //20
		glm::vec3(0.0f, 0.0f, -21.0f), //21
		glm::vec3(0.0f, 0.0f, -22.0f), //22
		glm::vec3(0.0f, 0.0f, -23.0f), //23
	//////////COL #1
		glm::vec3(1.0f, 0.0f, -1.0f), //01
		glm::vec3(1.0f, 0.0f, -2.0f), //02
		glm::vec3(1.0f, 0.0f, -5.0f), //05
		glm::vec3(1.0f, 0.0f, -6.0f), //06
		glm::vec3(1.0f, 0.0f, -7.0f), //07
		glm::vec3(1.0f, 0.0f, -8.0f), //08
		glm::vec3(1.0f, 0.0f, -11.0f), //11
		glm::vec3(1.0f, 0.0f, -12.0f), //12
		glm::vec3(1.0f, 0.0f, -16.0f), //16
		glm::vec3(1.0f, 0.0f, -17.0f), //17
		glm::vec3(1.0f, 0.0f, -20.0f), //20
		glm::vec3(1.0f, 0.0f, -21.0f), //21
		glm::vec3(1.0f, 0.0f, -22.0f), //22
	//////////COL #2
		glm::vec3(2.0f, 0.0f, -1.0f), //01
		glm::vec3(2.0f, 0.0f, -2.0f), //02
		glm::vec3(2.0f, 0.0f, -3.0f), //03
		glm::vec3(2.0f, 0.0f, -4.0f), //04
		glm::vec3(2.0f, 0.0f, -7.0f), //07
		glm::vec3(2.0f, 0.0f, -8.0f), //08
		glm::vec3(2.0f, 0.0f, -9.0f), //09
		glm::vec3(2.0f, 0.0f, -10.0f), //10
		glm::vec3(2.0f, 0.0f, -13.0f), //13
		glm::vec3(2.0f, 0.0f, -14.0f), //14
		glm::vec3(2.0f, 0.0f, -16.0f), //16
		glm::vec3(2.0f, 0.0f, -19.0f), //19
		glm::vec3(2.0f, 0.0f, -23.0f), //23
	//////////COL #3
		glm::vec3(3.0f, 0.0f, -0.0f), //00
		glm::vec3(3.0f, 0.0f, -1.0f), //01
		glm::vec3(3.0f, 0.0f, -5.0f), //05
		glm::vec3(3.0f, 0.0f, -7.0f), //07
		glm::vec3(3.0f, 0.0f, -10.0f), //10
		glm::vec3(3.0f, 0.0f, -11.0f), //11
		glm::vec3(3.0f, 0.0f, -12.0f), //12
		glm::vec3(3.0f, 0.0f, -15.0f), //15
		glm::vec3(3.0f, 0.0f, -17.0f), //17
		glm::vec3(3.0f, 0.0f, -18.0f), //18
		glm::vec3(3.0f, 0.0f, -19.0f), //19
		glm::vec3(3.0f, 0.0f, -20.0f), //20
		glm::vec3(3.0f, 0.0f, -21.0f), //21
		glm::vec3(3.0f, 0.0f, -22.0f), //22
	//////////COL #4
		glm::vec3(4.0f, 0.0f, -1.0f), //01
		glm::vec3(4.0f, 0.0f, -2.0f), //02
		glm::vec3(4.0f, 0.0f, -3.0f), //03
		glm::vec3(4.0f, 0.0f, -4.0f), //04
		glm::vec3(4.0f, 0.0f, -6.0f), //06
		glm::vec3(4.0f, 0.0f, -8.0f), //08
		glm::vec3(4.0f, 0.0f, -11.0f), //11
		glm::vec3(4.0f, 0.0f, -13.0f), //13
		glm::vec3(4.0f, 0.0f, -14.0f), //14
	//////////COL #5
		glm::vec3(5.0f, 0.0f, -2.0f), //02
		glm::vec3(5.0f, 0.0f, -3.0f), //03
		glm::vec3(5.0f, 0.0f, -7.0f), //07
		glm::vec3(5.0f, 0.0f, -8.0f), //08
		glm::vec3(5.0f, 0.0f, -9.0f), //09
		glm::vec3(5.0f, 0.0f, -10.0f), //10
		glm::vec3(5.0f, 0.0f, -14.0f), //14
		glm::vec3(5.0f, 0.0f, -18.0f), //18
		glm::vec3(5.0f, 0.0f, -20.0f), //20
		glm::vec3(5.0f, 0.0f, -21.0f), //21
		glm::vec3(5.0f, 0.0f, -22.0f), //22
	//////////COL #6
		glm::vec3(6.0f, 0.0f, -0.0f), //00
		glm::vec3(6.0f, 0.0f, -1.0f), //01
		glm::vec3(6.0f, 0.0f, -6.0f), //06
		glm::vec3(6.0f, 0.0f, -10.0f), //10
		glm::vec3(6.0f, 0.0f, -11.0f), //11
		glm::vec3(6.0f, 0.0f, -12.0f), //12
		glm::vec3(6.0f, 0.0f, -14.0f), //14
		glm::vec3(6.0f, 0.0f, -15.0f), //15
		glm::vec3(6.0f, 0.0f, -16.0f), //16
		glm::vec3(6.0f, 0.0f, -17.0f), //17
		glm::vec3(6.0f, 0.0f, -19.0f), //19
		glm::vec3(6.0f, 0.0f, -21.0f), //21
	//////////COL #7
		glm::vec3(7.0f, 0.0f, -2.0f), //02
		glm::vec3(7.0f, 0.0f, -4.0f), //04
		glm::vec3(7.0f, 0.0f, -7.0f), //07
		glm::vec3(7.0f, 0.0f, -9.0f), //09
		glm::vec3(7.0f, 0.0f, -10.0f), //10
		glm::vec3(7.0f, 0.0f, -13.0f), //13
		glm::vec3(7.0f, 0.0f, -15.0f), //15
		glm::vec3(7.0f, 0.0f, -16.0f), //16
		glm::vec3(7.0f, 0.0f, -17.0f), //17
		glm::vec3(7.0f, 0.0f, -22.0f), //22
	//////////COL #8
		glm::vec3(8.0f, 0.0f, -3.0f), //03
		glm::vec3(8.0f, 0.0f, -4.0f), //04
		glm::vec3(8.0f, 0.0f, -6.0f), //06
		glm::vec3(8.0f, 0.0f, -7.0f), //07
		glm::vec3(8.0f, 0.0f, -10.0f), //10
		glm::vec3(8.0f, 0.0f, -11.0f), //11
		glm::vec3(8.0f, 0.0f, -12.0f), //12
		glm::vec3(8.0f, 0.0f, -13.0f), //13
		glm::vec3(8.0f, 0.0f, -18.0f), //18
		glm::vec3(8.0f, 0.0f, -19.0f), //19
		glm::vec3(8.0f, 0.0f, -20.0f), //20
		glm::vec3(8.0f, 0.0f, -22.0f), //22
	//////////COL #9
		glm::vec3(9.0f, 0.0f, -0.0f), //00
		glm::vec3(9.0f, 0.0f, -1.0f), //01
		glm::vec3(9.0f, 0.0f, -5.0f), //05
		glm::vec3(9.0f, 0.0f, -8.0f), //08
		glm::vec3(9.0f, 0.0f, -9.0f), //09
		glm::vec3(9.0f, 0.0f, -12.0f), //12
		glm::vec3(9.0f, 0.0f, -14.0f), //14
		glm::vec3(9.0f, 0.0f, -17.0f), //17
		glm::vec3(9.0f, 0.0f, -19.0f), //19
		glm::vec3(9.0f, 0.0f, -20.0f), //20
	//////////COL #10
		glm::vec3(10.0f, 0.0f, -1.0f), //01
		glm::vec3(10.0f, 0.0f, -2.0f), //02
		glm::vec3(10.0f, 0.0f, -4.0f), //04
		glm::vec3(10.0f, 0.0f, -5.0f), //05
		glm::vec3(10.0f, 0.0f, -6.0f), //06
		glm::vec3(10.0f, 0.0f, -10.0f), //10
		glm::vec3(10.0f, 0.0f, -11.0f), //11
		glm::vec3(10.0f, 0.0f, -13.0f), //13
		glm::vec3(10.0f, 0.0f, -14.0f), //14
		glm::vec3(10.0f, 0.0f, -17.0f), //17
		glm::vec3(10.0f, 0.0f, -18.0f), //18
		glm::vec3(10.0f, 0.0f, -19.0f), //19
		glm::vec3(10.0f, 0.0f, -20.0f), //20
		glm::vec3(10.0f, 0.0f, -21.0f), //21
		glm::vec3(10.0f, 0.0f, -22.0f), //22
	//////////COL #11
		glm::vec3(11.0f, 0.0f, -3.0f), //03
		glm::vec3(11.0f, 0.0f, -5.0f), //05
		glm::vec3(11.0f, 0.0f, -7.0f), //07
		glm::vec3(11.0f, 0.0f, -8.0f), //08
		glm::vec3(11.0f, 0.0f, -10.0f), //10
		glm::vec3(11.0f, 0.0f, -15.0f), //15
		glm::vec3(11.0f, 0.0f, -18.0f), //18
		glm::vec3(11.0f, 0.0f, -19.0f), //19
		glm::vec3(11.0f, 0.0f, -23.0f), //23
	//////////COL #12
		glm::vec3(12.0f, 0.0f, -1.0f), //01
		glm::vec3(12.0f, 0.0f, -2.0f), //02
		glm::vec3(12.0f, 0.0f, -6.0f), //06
		glm::vec3(12.0f, 0.0f, -7.0f), //07
		//glm::vec3(12.0f, 0.0f, -11.0f), //11
		glm::vec3(12.0f, 0.0f, -15.0f), //15
		glm::vec3(12.0f, 0.0f, -16.0f), //16
		glm::vec3(12.0f, 0.0f, -18.0f), //18
		glm::vec3(12.0f, 0.0f, -20.0f), //20
		glm::vec3(12.0f, 0.0f, -21.0f), //21
	//////////COL #13
		glm::vec3(13.0f, 0.0f, -3.0f), //03
		glm::vec3(13.0f, 0.0f, -4.0f), //04
		glm::vec3(13.0f, 0.0f, -5.0f), //05
		glm::vec3(13.0f, 0.0f, -9.0f), //09
		glm::vec3(13.0f, 0.0f, -10.0f), //10
		glm::vec3(13.0f, 0.0f, -14.0f), //14
		glm::vec3(13.0f, 0.0f, -19.0f), //19
		glm::vec3(13.0f, 0.0f, -20.0f), //20
		glm::vec3(13.0f, 0.0f, -22.0f), //22
		glm::vec3(13.0f, 0.0f, -23.0f), //23
	//////////COL #14
		glm::vec3(14.0f, 0.0f, -1.0f), //01
		glm::vec3(14.0f, 0.0f, -4.0f), //04
		glm::vec3(14.0f, 0.0f, -5.0f), //05
		glm::vec3(14.0f, 0.0f, -6.0f), //06
		glm::vec3(14.0f, 0.0f, -7.0f), //07
		glm::vec3(14.0f, 0.0f, -9.0f), //09
		//glm::vec3(14.0f, 0.0f, -12.0f), //12
		glm::vec3(14.0f, 0.0f, -15.0f), //15
		glm::vec3(14.0f, 0.0f, -16.0f), //16
		glm::vec3(14.0f, 0.0f, -18.0f), //18
		glm::vec3(14.0f, 0.0f, -20.0f), //20
		glm::vec3(14.0f, 0.0f, -23.0f), //23
	//////////COL #15
		glm::vec3(15.0f, 0.0f, -0.0f), //00
		glm::vec3(15.0f, 0.0f, -1.0f), //01
		glm::vec3(15.0f, 0.0f, -4.0f), //04
		glm::vec3(15.0f, 0.0f, -5.0f), //05
		glm::vec3(15.0f, 0.0f, -6.0f), //06
		glm::vec3(15.0f, 0.0f, -8.0f), //08
		glm::vec3(15.0f, 0.0f, -11.0f), //11
		glm::vec3(15.0f, 0.0f, -12.0f), //12
		glm::vec3(15.0f, 0.0f, -14.0f), //14
		glm::vec3(15.0f, 0.0f, -15.0f), //15
		glm::vec3(15.0f, 0.0f, -19.0f), //19
		glm::vec3(15.0f, 0.0f, -20.0f), //20
		glm::vec3(15.0f, 0.0f, -22.0f), //22
	//////////COL #16
		glm::vec3(16.0f, 0.0f, -1.0f), //01
		glm::vec3(16.0f, 0.0f, -2.0f), //02
		glm::vec3(16.0f, 0.0f, -5.0f), //05
		glm::vec3(16.0f, 0.0f, -7.0f), //07
		glm::vec3(16.0f, 0.0f, -9.0f), //09
		glm::vec3(16.0f, 0.0f, -10.0f), //10
		glm::vec3(16.0f, 0.0f, -12.0f), //12
		glm::vec3(16.0f, 0.0f, -14.0f), //14
		glm::vec3(16.0f, 0.0f, -16.0f), //16
		glm::vec3(16.0f, 0.0f, -17.0f), //17
		glm::vec3(16.0f, 0.0f, -18.0f), //18
		glm::vec3(16.0f, 0.0f, -19.0f), //19
		glm::vec3(16.0f, 0.0f, -22.0f), //22
	//////////COL #17
		glm::vec3(17.0f, 0.0f, -0.0f), //00
		glm::vec3(17.0f, 0.0f, -1.0f), //01
		glm::vec3(17.0f, 0.0f, -6.0f), //06
		glm::vec3(17.0f, 0.0f, -8.0f), //08
		glm::vec3(17.0f, 0.0f, -10.0f), //10
		glm::vec3(17.0f, 0.0f, -11.0f), //11
		glm::vec3(17.0f, 0.0f, -13.0f), //13
		glm::vec3(17.0f, 0.0f, -15.0f), //15
		glm::vec3(17.0f, 0.0f, -20.0f), //20
		glm::vec3(17.0f, 0.0f, -23.0f), //23
	//////////COL #18
		glm::vec3(18.0f, 0.0f, -2.0f), //02
		glm::vec3(18.0f, 0.0f, -3.0f), //03
		glm::vec3(18.0f, 0.0f, -4.0f), //04
		glm::vec3(18.0f, 0.0f, -5.0f), //05
		glm::vec3(18.0f, 0.0f, -7.0f), //07
		glm::vec3(18.0f, 0.0f, -8.0f), //08
		glm::vec3(18.0f, 0.0f, -12.0f), //12
		glm::vec3(18.0f, 0.0f, -14.0f), //14
		glm::vec3(18.0f, 0.0f, -17.0f), //17
		glm::vec3(18.0f, 0.0f, -19.0f), //19
		glm::vec3(18.0f, 0.0f, -22.0f), //22
	//////////COL #19
		glm::vec3(19.0f, 0.0f, -1.0f), //01
		glm::vec3(19.0f, 0.0f, -2.0f), //02
		glm::vec3(19.0f, 0.0f, -3.0f), //03
		glm::vec3(19.0f, 0.0f, -4.0f), //04
		glm::vec3(19.0f, 0.0f, -6.0f), //06
		glm::vec3(19.0f, 0.0f, -7.0f), //07
		glm::vec3(19.0f, 0.0f, -9.0f), //09
		glm::vec3(19.0f, 0.0f, -10.0f), //10
		glm::vec3(19.0f, 0.0f, -11.0f), //11
		glm::vec3(19.0f, 0.0f, -14.0f), //14
		glm::vec3(19.0f, 0.0f, -15.0f), //15
		glm::vec3(19.0f, 0.0f, -16.0f), //16
		glm::vec3(19.0f, 0.0f, -18.0f), //18
		glm::vec3(19.0f, 0.0f, -19.0f), //19
		glm::vec3(19.0f, 0.0f, -20.0f), //20
		glm::vec3(19.0f, 0.0f, -23.0f), //23
	//////////COL #20
		glm::vec3(20.0f, 0.0f, -1.0f), //01
		glm::vec3(20.0f, 0.0f, -5.0f), //05
		glm::vec3(20.0f, 0.0f, -8.0f), //08
		glm::vec3(20.0f, 0.0f, -10.0f), //10
		glm::vec3(20.0f, 0.0f, -13.0f), //13
		glm::vec3(20.0f, 0.0f, -17.0f), //17
		glm::vec3(20.0f, 0.0f, -19.0f), //19
		glm::vec3(20.0f, 0.0f, -20.0f), //20
		glm::vec3(20.0f, 0.0f, -21.0f), //21
	//////////COL #21
		glm::vec3(21.0f, 0.0f, -1.0f), //01
		glm::vec3(21.0f, 0.0f, -3.0f), //03
		glm::vec3(21.0f, 0.0f, -4.0f), //04
		glm::vec3(21.0f, 0.0f, -7.0f), //07
		glm::vec3(21.0f, 0.0f, -11.0f), //11
		glm::vec3(21.0f, 0.0f, -12.0f), //12
		glm::vec3(21.0f, 0.0f, -13.0f), //13
		glm::vec3(21.0f, 0.0f, -14.0f), //14
		glm::vec3(21.0f, 0.0f, -16.0f), //16
		glm::vec3(21.0f, 0.0f, -17.0f), //17
		glm::vec3(21.0f, 0.0f, -18.0f), //18
		glm::vec3(21.0f, 0.0f, -20.0f), //20
		glm::vec3(21.0f, 0.0f, -21.0f), //21
		glm::vec3(21.0f, 0.0f, -22.0f), //22
	//////////COL #22
		glm::vec3(22.0f, 0.0f, -0.0f), //00
		glm::vec3(22.0f, 0.0f, -2.0f), //02
		glm::vec3(22.0f, 0.0f, -5.0f), //05
		glm::vec3(22.0f, 0.0f, -6.0f), //06
		glm::vec3(22.0f, 0.0f, -8.0f), //08
		glm::vec3(22.0f, 0.0f, -9.0f), //09
		glm::vec3(22.0f, 0.0f, -10.0f), //10
		glm::vec3(22.0f, 0.0f, -11.0f), //11
		glm::vec3(22.0f, 0.0f, -12.0f), //12
		glm::vec3(22.0f, 0.0f, -14.0f), //14
		glm::vec3(22.0f, 0.0f, -15.0f), //15
		glm::vec3(22.0f, 0.0f, -16.0f), //16
		glm::vec3(22.0f, 0.0f, -19.0f), //19
		glm::vec3(22.0f, 0.0f, -20.0f), //20
		glm::vec3(22.0f, 0.0f, -23.0f), //23
	//////////COL #23
		glm::vec3(23.0f, 0.0f, -1.0f), //01
		glm::vec3(23.0f, 0.0f, -4.0f), //04
		glm::vec3(23.0f, 0.0f, -5.0f), //05
		glm::vec3(23.0f, 0.0f, -6.0f), //06
		glm::vec3(23.0f, 0.0f, -7.0f), //07
		glm::vec3(23.0f, 0.0f, -11.0f), //11
		glm::vec3(23.0f, 0.0f, -15.0f), //15
		glm::vec3(23.0f, 0.0f, -17.0f), //17
		glm::vec3(23.0f, 0.0f, -18.0f), //18
	//////////COL #24
		glm::vec3(24.0f, 0.0f, -0.0f), //00
		glm::vec3(24.0f, 0.0f, -1.0f), //01
		glm::vec3(24.0f, 0.0f, -2.0f), //02
		glm::vec3(24.0f, 0.0f, -3.0f), //03
		glm::vec3(24.0f, 0.0f, -4.0f), //04
		glm::vec3(24.0f, 0.0f, -5.0f), //05
		glm::vec3(24.0f, 0.0f, -6.0f), //06
		glm::vec3(24.0f, 0.0f, -7.0f), //07
		glm::vec3(24.0f, 0.0f, -8.0f), //08
		glm::vec3(24.0f, 0.0f, -9.0f), //09
		glm::vec3(24.0f, 0.0f, -10.0f), //10
		glm::vec3(24.0f, 0.0f, -11.0f), //11
		glm::vec3(24.0f, 0.0f, -12.0f), //12
		glm::vec3(24.0f, 0.0f, -13.0f), //13
		glm::vec3(24.0f, 0.0f, -14.0f), //14
		glm::vec3(24.0f, 0.0f, -15.0f), //15
		glm::vec3(24.0f, 0.0f, -16.0f), //16
		glm::vec3(24.0f, 0.0f, -17.0f), //17
		glm::vec3(24.0f, 0.0f, -18.0f), //18
		glm::vec3(24.0f, 0.0f, -19.0f), //19
		glm::vec3(24.0f, 0.0f, -20.0f), //20
		glm::vec3(24.0f, 0.0f, -21.0f), //21
		glm::vec3(24.0f, 0.0f, -22.0f), //22
		glm::vec3(24.0f, 0.0f, -23.0f) //23
};

// MAZE ROOM
const float kMazeRoomWallThickness = 0.2f;
const int kMazeRoomHorizontalSize = 10;
const int kMazeRoomVerticalSize = 4;
const int kMazeRoomRoofSize = 15;
const int kMazeRoomSize = 14;
Wall* kMazeRoomHorizontal = new Wall[kMazeRoomHorizontalSize];
Wall* kMazeRoomVertical = new Wall[kMazeRoomVerticalSize];
Wall* kMazeRoomRoof = new Wall[kMazeRoomRoofSize];
Wall* kMazeRoomFloor = new Wall[kMazeRoomRoofSize];
glm::vec3 kMazeRoomHorizontalCoords[kMazeRoomHorizontalSize] = {
	//////////ROW #11
		glm::vec3(10.0f, 0.0f, -11.0f), //10
		glm::vec3(11.0f, 0.0f, -11.0f), //11
		glm::vec3(12.0f, 0.0f, -11.0f), //12
		glm::vec3(13.0f, 0.0f, -11.0f), //13
		glm::vec3(14.0f, 0.0f, -11.0f), //14
	//////////ROW #14
		glm::vec3(10.0f, 0.0f, -14.0f), //10
		glm::vec3(11.0f, 0.0f, -14.0f), //11
		glm::vec3(12.0f, 0.0f, -14.0f), //12
		glm::vec3(13.0f, 0.0f, -14.0f), //13
		glm::vec3(14.0f, 0.0f, -14.0f), //14
};
glm::vec3 kMazeRoomVerticalCoords[kMazeRoomVerticalSize] = {
	//////////COL #10
		glm::vec3(10.0f, 0.0f, -11.0f), //11
		glm::vec3(10.0f, 0.0f, -13.0f), //13
	//////////COL #15
		glm::vec3(15.0f, 0.0f, -11.0f), //11
		glm::vec3(15.0f, 0.0f, -12.0f), //12
};
glm::vec3 kMazeRoomRoofCoords[kMazeRoomRoofSize] = {
	//////////ROW #11
		glm::vec3(10.0f, 0.0f, -11.0f), //10
		glm::vec3(11.0f, 0.0f, -11.0f), //11
		glm::vec3(12.0f, 0.0f, -11.0f), //12
		glm::vec3(13.0f, 0.0f, -11.0f), //13
		glm::vec3(14.0f, 0.0f, -11.0f), //14
	//////////ROW #12
		glm::vec3(10.0f, 0.0f, -12.0f), //10
		glm::vec3(11.0f, 0.0f, -12.0f), //11
		glm::vec3(12.0f, 0.0f, -12.0f), //12
		glm::vec3(13.0f, 0.0f, -12.0f), //13
		glm::vec3(14.0f, 0.0f, -12.0f), //14
	//////////ROW #13
		glm::vec3(10.0f, 0.0f, -13.0f), //10
		glm::vec3(11.0f, 0.0f, -13.0f), //11
		glm::vec3(12.0f, 0.0f, -13.0f), //12
		glm::vec3(13.0f, 0.0f, -13.0f), //13
		glm::vec3(14.0f, 0.0f, -13.0f), //14
};

// CASTLE WALL TOPS
const float kCastleWallTopThickness = 0.1f;
const int kCastleWallTopSize = 28;
const int kCastleWallTopMerlonSize = 84;
Wall kCastleWallTop(kCastleWallTopThickness, 1);

//******************************************************************** 
// FUNCTIONS 
//********************************************************************
void timer(int);

void resetView()
{
	position = glm::vec3(15.0f, 10.0f, 10.0f);
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = -30.0f;
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

	// ROOM TEXTURE
	unsigned char* image3 = stbi_load("TexturesCom_BrickMessy0332_2_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!image3) cout << "Unable to load ROOM TEXTURE file!" << endl;
	glGenTextures(1, &kRoomTexture);
	glBindTexture(GL_TEXTURE_2D, kRoomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image3);

	// CRYSTAL TEXTURE
	unsigned char* crystal_tex = stbi_load("alien-skin-organic-seamless-generated-hires-texture-photo.jpg", &width, &height, &bitDepth, 0);
	if (!crystal_tex) cout << "Unable to load ROOM TEXTURE file!" << endl;
	glGenTextures(1, &kCrystalTexture);
	glBindTexture(GL_TEXTURE_2D, kCrystalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, crystal_tex);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(crystal_tex);

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

	// STAIR TEXTURE
	unsigned char* stairTex = stbi_load("TexturesCom_BrickOldOvergrown0006_1_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!stairTex) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kStairTexture);
	glBindTexture(GL_TEXTURE_2D, kStairTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, stairTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(stairTex);

	// TOWER TEXTURE
	unsigned char* towerTex = stbi_load("TexturesCom_BrickOldMixedSize0064_1_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!towerTex) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kTowerBaseTexture);
	glBindTexture(GL_TEXTURE_2D, kTowerBaseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, towerTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(towerTex);

	// TOWER ROOF TEXTURE
	unsigned char* towerRoofTex = stbi_load("TexturesCom_RooftilesSlate0159_5_seamless_S.jpg", &width, &height, &bitDepth, 0);
	if (!towerRoofTex) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kTowerRoofTexture);
	glBindTexture(GL_TEXTURE_2D, kTowerRoofTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, towerRoofTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(towerRoofTex);
	
	// GATE TEXTURE
	unsigned char* gateTex = stbi_load("TexturesCom_DoorsMedieval0557_2_M.jpg", &width, &height, &bitDepth, 0);
	if (!gateTex) cout << "Unable to load file!" << endl;
	glGenTextures(1, &kGateTexture);
	glBindTexture(GL_TEXTURE_2D, kGateTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, gateTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(gateTex);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);
}

void SetUpLights() {
	// Setting ambient Light.
	glUniform3f(glGetUniformLocation(program, "aLight.ambientColour"), aLight.ambientColour.x, aLight.ambientColour.y, aLight.ambientColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.ambientStrength"), aLight.ambientStrength);

	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColour"), pLights[0].diffuseColour.x, pLights[0].diffuseColour.y, pLights[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].exponent"), pLights[0].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColour"), pLights[1].diffuseColour.x, pLights[1].diffuseColour.y, pLights[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[1].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[1].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[1].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].exponent"), pLights[1].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[2].base.diffuseColour"), pLights[2].diffuseColour.x, pLights[2].diffuseColour.y, pLights[2].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[2].base.diffuseStrength"), pLights[2].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[2].position"), pLights[2].position.x, pLights[2].position.y, pLights[2].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[2].constant"), pLights[2].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[2].linear"), pLights[2].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[2].exponent"), pLights[2].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[3].base.diffuseColour"), pLights[3].diffuseColour.x, pLights[3].diffuseColour.y, pLights[3].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[3].base.diffuseStrength"), pLights[3].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[3].position"), pLights[3].position.x, pLights[3].position.y, pLights[3].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[3].constant"), pLights[3].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[3].linear"), pLights[3].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[3].exponent"), pLights[3].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[4].base.diffuseColour"), pLights[4].diffuseColour.x, pLights[4].diffuseColour.y, pLights[4].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[4].base.diffuseStrength"), pLights[4].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[4].position"), pLights[4].position.x, pLights[4].position.y, pLights[4].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[4].constant"), pLights[4].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[4].linear"), pLights[4].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[4].exponent"), pLights[4].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[5].base.diffuseColour"), pLights[5].diffuseColour.x, pLights[5].diffuseColour.y, pLights[5].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[5].base.diffuseStrength"), pLights[5].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[5].position"), pLights[5].position.x, pLights[5].position.y, pLights[5].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[5].constant"), pLights[5].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[5].linear"), pLights[5].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[5].exponent"), pLights[5].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[6].base.diffuseColour"), pLights[6].diffuseColour.x, pLights[6].diffuseColour.y, pLights[6].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[6].base.diffuseStrength"), pLights[6].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[6].position"), pLights[6].position.x, pLights[6].position.y, pLights[6].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[6].constant"), pLights[6].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[6].linear"), pLights[6].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[6].exponent"), pLights[6].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[7].base.diffuseColour"), pLights[7].diffuseColour.x, pLights[7].diffuseColour.y, pLights[7].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[7].base.diffuseStrength"), pLights[7].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[7].position"), pLights[7].position.x, pLights[7].position.y, pLights[7].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[7].constant"), pLights[7].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[7].linear"), pLights[7].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[7].exponent"), pLights[7].exponent);

	// Setting spot light.
	glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColour"), sLight.diffuseColour.x, sLight.diffuseColour.y, sLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);
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

	// SETUP LIGHTS
	SetUpLights();

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

	// MAZE SETTINGS
	for (int i = 0; i < kMazeHorizontalSize; i++) {
		kMazeHorizontal[i] = Wall(kMazeWallThickness, 1);
	}
	for (int i = 0; i < kMazeVerticalSize; i++) {
		kMazeVertical[i] = Wall(kMazeWallThickness, 1);
	}

	for (int i = 0; i < kMazeRoomHorizontalSize; i++) {
		kMazeRoomHorizontal[i] = Wall(kMazeRoomWallThickness, 1);
	}
	for (int i = 0; i < kMazeRoomVerticalSize; i++) {
		kMazeRoomVertical[i] = Wall(kMazeRoomWallThickness, 1);
	}
	for (int i = 0; i < kMazeRoomRoofSize; i++) {
		kMazeRoomRoof[i] = Wall(kMazeRoomWallThickness, 1);
	}
	for (int i = 0; i < kMazeRoomRoofSize; i++) {
		kMazeRoomFloor[i] = Wall(kMazeRoomWallThickness, 1);
	}

	timer(0);
}

//---------------------------------------------------------------------
//
// Distance Check
//
//glm::vec3 GetDistance(glm::vec3 source_pos, glm::vec3 dest_pos) {
//	
//}

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
void DisplayGround() {
	glBindTexture(GL_TEXTURE_2D, kGroundTexture);
	kGroundShape.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, kGroundShape.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void DisplayMaze(float x_offset = 0.0f, float z_offset = 0.0f) {
	glBindTexture(GL_TEXTURE_2D, kHedgeTexture);

	for (int i = 0; i < kMazeHorizontalSize; i++) {
		float z_local_offset = 0.0f; // value is half thickness of wall, to center wall
		kMazeHorizontal[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(kMazeHorizontalCoords[i].x + x_offset,
			kMazeHorizontalCoords[i].y,
			kMazeHorizontalCoords[i].z + z_offset + z_local_offset));
		glDrawElements(GL_TRIANGLES, kMazeHorizontal[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
	for (int i = 0; i < kMazeVerticalSize; i++) {
		float x_local_offset = -kMazeWallThickness / 2; // value is half thickness of wall, to center wall
		kMazeVertical[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, 90.0f, glm::vec3(kMazeVerticalCoords[i].x + x_offset + x_local_offset,
			kMazeVerticalCoords[i].y,
			kMazeVerticalCoords[i].z + z_offset));
		glDrawElements(GL_TRIANGLES, kMazeVertical[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
}

void DisplayMazeRoom(float x_offset = 0.0f, float z_offset = 0.0f) {
	glBindTexture(GL_TEXTURE_2D, kRoomTexture);
	
	float scale = 1.25f;
	for (int i = 0; i < kMazeRoomHorizontalSize; i++) {
		float z_local_offset = 0.0f; // offset to prevent overlaps, vertical maze walls are centered
		kMazeRoomHorizontal[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		if (i < (kMazeRoomHorizontalSize >> 1)) { //bottom walls 
			z_local_offset = -kMazeWallThickness; //-0.2
		}
		else { //top walls
			z_local_offset = kMazeWallThickness; //0.2
		}
		transformObject(glm::vec3(1.0f, scale, 1.0f), X_AXIS, 0.0f, glm::vec3(kMazeRoomHorizontalCoords[i].x + x_offset,
			kMazeRoomHorizontalCoords[i].y,
			kMazeRoomHorizontalCoords[i].z + z_offset + z_local_offset));
		glDrawElements(GL_TRIANGLES, kMazeRoomHorizontal[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
	for (int i = 0; i < kMazeRoomVerticalSize; i++) {
		float x_local_offset = 0.0f; // value is half thickness of wall, to center wall
		float z_local_offset = kMazeRoomWallThickness / 2; // offset to prevent overlaps, vertical maze walls are centered
		kMazeRoomVertical[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		if (i < (kMazeRoomVerticalSize >> 1)) { //left walls 
			x_local_offset = kMazeWallThickness / 2; //-0.2d
		}
		else { //right walls
			x_local_offset = -kMazeWallThickness - kMazeWallThickness / 2; //0.2
		}
		transformObject(glm::vec3(1.0f, scale, 1.0f), Y_AXIS, 90.0f, glm::vec3(kMazeRoomVerticalCoords[i].x + x_offset + x_local_offset,
			kMazeRoomVerticalCoords[i].y,
			kMazeRoomVerticalCoords[i].z + z_offset + z_local_offset));
		glDrawElements(GL_TRIANGLES, kMazeRoomVertical[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
	for (int i = 0; i < kMazeRoomRoofSize; i++) {
		float z_local_offset = kMazeRoomWallThickness / 2; // offset to prevent overlaps, vertical maze walls are centered
		kMazeRoomRoof[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(kMazeRoomRoofCoords[i].x + x_offset,
			kMazeRoomRoofCoords[i].y + scale,
			kMazeRoomRoofCoords[i].z + z_offset + z_local_offset));
		glDrawElements(GL_TRIANGLES, kMazeRoomRoof[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
	for (int i = 0; i < kMazeRoomRoofSize; i++) {
		float z_local_offset = kMazeRoomWallThickness / 2; // offset to prevent overlaps, vertical maze walls are centered
		kMazeRoomFloor[i].BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(kMazeRoomRoofCoords[i].x + x_offset,
			kMazeRoomRoofCoords[i].y,
			kMazeRoomRoofCoords[i].z + z_offset + z_local_offset));
		glDrawElements(GL_TRIANGLES, kMazeRoomFloor[i].NumIndices(), GL_UNSIGNED_SHORT, 0);
	}
}

void DisplayTowers() {
	glBindTexture(GL_TEXTURE_2D, kTowerRoofTexture);
	roof.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	// Tower roofs
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(-0.5f, 3.0f, -1.5f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(28.5f, 3.0f, -1.5f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(-0.5f, 3.0f, -30.5f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(28.5f, 3.0f, -30.5f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Gatehouse roofs
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(12.5f, 3.0f, -1.85f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(2.0f, 0.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(15.5f, 3.0f, -1.85f));
	glDrawElements(GL_TRIANGLES, roof.NumIndices(), GL_UNSIGNED_SHORT, 0);

	glBindTexture(GL_TEXTURE_2D, kTowerBaseTexture);
	// Tower upper base
	baseTower.SetTextureScale(1.0f, 3.0f);
	baseTower.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.5f, 1.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(-0.25f, 2.0f, -1.25f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.5f, 1.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(28.75f, 2.0f, -1.25f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.5f, 1.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(-0.25f, 2.0f, -30.25f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.5f, 1.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(28.75f, 2.0f, -30.25f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Gatehouse base
	baseGatehouse.SetTextureScale(3.0f, 3.0f);
	baseGatehouse.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.5f, 3.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(12.75f, 0.0f, -1.6f));
	glDrawElements(GL_TRIANGLES, baseGatehouse.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.5f, 3.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(15.75f, 0.0f, -1.6f));
	glDrawElements(GL_TRIANGLES, baseGatehouse.NumIndices(), GL_UNSIGNED_SHORT, 0);

	// Tower lower base
	baseTower.SetTextureScale(2.0f, 2.5f);
	baseTower.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(29.0f, 0.0f, -1.0f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -30.0f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(29.0f, 0.0f, -30.0f));
	glDrawElements(GL_TRIANGLES, baseTower.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void DisplayCastleWallsGatehouse()
{
	glBindTexture(GL_TEXTURE_2D, kTowerBaseTexture);
	// Left and right wall
	castleWall.SetTextureScale(0.6f, 2.0f, 29.0f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(0.6f, 2.0f, 29.0f), X_AXIS, 0.0f, glm::vec3(0.2f, 0.0f, -29.5f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(0.6f, 2.0f, 29.0f), X_AXIS, 0.0f, glm::vec3(29.2f, 0.0f, -29.5f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Back wall
	castleWall.SetTextureScale(29.0f, 2.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(29.0f, 2.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(0.5f, 0.0f, -29.8f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Front wall
	castleWall.SetTextureScale(13.0f, 2.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(13.0f, 2.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(0.5f, 0.0f, -0.8f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	transformObject(glm::vec3(13.0f, 2.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(16.5f, 0.0f, -0.8f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Gatehouse wall
	castleWall.SetTextureScale(3.0f, 1.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 1.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(13.5f, 1.5f, -1.6f));  // Upper
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.5f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.5f, 0.6f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -1.6f));  // Lower
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(1.0f, 1.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.5f, -1.6f));  // Middle Left
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(1.0f, 1.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(15.5f, 0.5f, -1.6f));  // Middle Right
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	// Gate
	glBindTexture(GL_TEXTURE_2D, kGateTexture);
	castleWall.SetTextureScale(1.0f, 1.0f, 0.6f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 0.6f), X_AXIS, 0.0f, glm::vec3(14.5f, 0.5f, -1.6f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void DisplayCastleWallTop() {
	glBindTexture(GL_TEXTURE_2D, kWallTexture);
	
	// FRONT OUTER
	float x_offset = 1.0f;
	float y_offset = 2.0f;
	float z_offset = -0.3f;
	float x_coord = 0.0f;
	float y_coord = 0.0f;
	float z_coord = 0.0f;
	float y_merlon_offset = 0.25f; // wall height
	for (int i = 0; i < kCastleWallTopSize; i++) {
		if (i < ((kCastleWallTopSize >> 1) - 2) || i > ((kCastleWallTopSize >> 1) + 1)) { // avoid flickering at center, overlaps w/ gate
			kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
			transformObject(glm::vec3(1.0f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
				y_coord + y_offset,
				z_coord + z_offset));
			glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		}
		
		x_coord += 1.0f;
	}
	x_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		if (i < ((kCastleWallTopMerlonSize >> 1) - 6) || i >((kCastleWallTopMerlonSize >> 1) + 4)) { // avoid flickering at center, overlaps w/ gate
			kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
			transformObject(glm::vec3(0.25f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
				y_coord + y_offset + y_merlon_offset,
				z_coord + z_offset));
			glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		}
		x_coord += 0.33333f;
	}


	// FRONT INNER
	x_coord = 0.0f;
	z_offset = -0.8f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		if (i < ((kCastleWallTopSize >> 1) - 2) || i >((kCastleWallTopSize >> 1) + 1)) { // avoid flickering at center, overlaps w/ gate
			kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
			transformObject(glm::vec3(1.0f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
				y_coord + y_offset,
				z_coord + z_offset));
			glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		}
		x_coord += 1.0f;
	}
	x_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		if (i < ((kCastleWallTopMerlonSize >> 1) - 6) || i >((kCastleWallTopMerlonSize >> 1) + 4)) { // avoid flickering at center, overlaps w/ gate
			kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
			transformObject(glm::vec3(0.25f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
				y_coord + y_offset + y_merlon_offset,
				z_coord + z_offset));
			glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		}
		x_coord += 0.33333f;
	}
	
	// BACK OUTER
	x_coord = 0.0f;
	z_offset = -29.8f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		x_coord += 1.0f;
	}
	x_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		x_coord += 0.33333f;
	}
	
	// BACK INNER
	x_coord = 0.0f;
	z_offset = -29.3f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		x_coord += 1.0f;
	}
	x_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(x_coord + x_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		x_coord += 0.33333f;
	}
	
	// LEFT OUTER
	x_offset = 0.25f;
	z_offset = -1.0f;
	x_coord = 0.0f;
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 1.0f;
	}
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 0.33333f;
	}
	
	// LEFT INNER
	x_offset = 0.75f;
	x_coord = 0.0f;
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 1.0f;
	}
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 0.33333f;
	}
	
	// RIGHT OUTER
	x_offset =29.25f;
	x_coord = 0.0f;
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 1.0f;
	}
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 0.33333f;
	}
	
	// RIGHT INNER
	x_offset = 29.75f;
	x_coord = 0.0f;
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(1.0f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 1.0f;
	}
	z_coord = 0.0f;
	for (int i = 0; i < kCastleWallTopMerlonSize; i++) {
		float x_local_offset = -kCastleWallTopThickness / 2; // value is half thickness of wall, to center wall
		kCastleWallTop.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.25f, 0.25f, 1.0f), Y_AXIS, 90.0f, glm::vec3(x_coord + x_offset + x_local_offset,
			y_coord + y_offset + y_merlon_offset,
			z_coord + z_offset));
		glDrawElements(GL_TRIANGLES, kCastleWallTop.NumIndices(), GL_UNSIGNED_SHORT, 0);
		z_coord -= 0.33333f;
	}


}

void DisplayStairs()
{
	glBindTexture(GL_TEXTURE_2D, kStairTexture);
	// Front side
	castleWall.SetTextureScale(3.0f, 0.5f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.5f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -1.0f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.4f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.4f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -0.8f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.3f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.3f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -0.6f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.2f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.2f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -0.4f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.1f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.1f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -0.2f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);

	// Back side
	castleWall.SetTextureScale(3.0f, 0.5f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.5f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -1.8f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.4f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.4f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -2.0f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.3f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.3f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -2.2f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.2f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.2f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -2.4f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
	castleWall.SetTextureScale(3.0f, 0.1f, 0.2f);
	castleWall.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(3.0f, 0.1f, 0.2f), X_AXIS, 0.0f, glm::vec3(13.5f, 0.0f, -2.6f));
	glDrawElements(GL_TRIANGLES, castleWall.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void DisplayCrystal() {
	glBindTexture(GL_TEXTURE_2D, kCrystalTexture);
	kCrystalKey.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	kCrystalRotAngle += 1.0f;
	transformObject(kCrystalScale, Y_AXIS, kCrystalRotAngle, kCrystalPos);
	//transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, kCrystalRotAngle, glm::vec3(26.5f, 0.5f, 3.0f));
	glDrawElements(GL_TRIANGLES, kCrystalKey.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

void DisplayReceptor(){
	glBindTexture(GL_TEXTURE_2D, kBlankTexture);
	kReceptor.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(0.5f, 0.5f, 0.5f), Y_AXIS, 0.0f, kReceptorPos);
	glDrawElements(GL_TRIANGLES, kReceptor.NumIndices(), GL_UNSIGNED_SHORT, 0);
}

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

	// GROUND
	DisplayGround();

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	// MAZE
	float x_offset = 3.0f; //spacing for castle walls
	float z_offset = -3.0f; //spacing for castle walls
	DisplayMaze(x_offset, z_offset);

	// MAZE ROOM
	DisplayMazeRoom(x_offset, z_offset);

	// TOWERS
	DisplayTowers();

	// CASTLE WALLS
	DisplayCastleWallsGatehouse();
	DisplayCastleWallTop();

	// STAIRS
	DisplayStairs();

	// CRYSTAL
	if (kIsCrystalActive || kIsReceptorActive) {
		DisplayCrystal();
	}
	
	// RECEPTOR
	DisplayReceptor();

	glBindVertexArray(0); // Done writing.

	// UPDATE LOOP
	if (kIsCrystalActive && !kIsReceptorActive) {
		if (glm::distance(position, kExitPoint) < 1.0f) {
			cout << ">>> Voice of Kos: You cannot exit the maze!" << endl;
		}
	}
	if (kIsCrystalActive) {
		if (glm::distance(position, kCrystalPos) < 1.0f) {
			kIsCrystalActive = false;
			cout << ">>> Voice of Kos: Crystal collected..." << endl;
		}
	}
	if (!kIsCrystalActive && !kIsReceptorActive) {
		if (glm::distance(position, kReceptorPos) < 1.0f) {
			kIsReceptorActive = true;
			kCrystalPos = kReceptorPos;
			kCrystalPos.y = 0.75f;
			kCrystalScale = glm::vec3(0.1f, 0.1f, 0.1f);
			cout << ">>> Voice of Kos: Gate has been unlocked!" << endl;
		}
	}

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
	glutTimerFunc(1000 / FPS, timer, 0); // 60 FPS or 16.67ms.
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

	glDeleteTextures(1, &kBlankTexture);
	glDeleteTextures(1, &kGroundTexture);
	glDeleteTextures(1, &kHedgeTexture);
	glDeleteTextures(1, &kRoomTexture);
	glDeleteTextures(1, &kWallTexture);
	glDeleteTextures(1, &kCrystalTexture);
	glDeleteTextures(1, &kStairTexture);
	glDeleteTextures(1, &kTowerRoofTexture);
	glDeleteTextures(1, &kTowerBaseTexture);

	delete[] kMazeHorizontal;
	delete[] kMazeVertical;

	delete[] kMazeRoomHorizontal;
	delete[] kMazeRoomVertical;
	delete[] kMazeRoomRoof;
	delete[] kMazeRoomFloor;
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
