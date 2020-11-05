#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> //03
#include <string> //03
#include <math.h>

#include <GLM/glm.hpp> //04
#include <glm/gtc/matrix_transform.hpp> //04

#include "ObjLoader.h" 
#include <GLM\gtc\type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
/// <summary> 
/*
The Project is done by:
Jin Chan 100593238 &
Maxwell Zhang 100692556
*/


/// </summary> 
/// 
GLFWwindow* window;

int Playerscore1 = 0;
int Playerscore2 = 0;


GLfloat playerSize = 0.16f;
GLfloat BallSize = 0.17f;

GLfloat rotY = 0.0f;
GLfloat rotX = 0.0f;

GLfloat tranX = 1.0f;
GLfloat tranY = 0.0f;
GLfloat tranX2 = -1.0f;
GLfloat tranY2 = 0.0f;

GLfloat BalltranX = 0.0f;
GLfloat BalltranY = 0.001f;

glm::vec3 ObjectPosition1(tranX, tranY, 0.0f);
glm::vec3 ObjectPosition2(tranX2, tranY2, 0.0f);
glm::vec3 BallPosition(BalltranX, BalltranY, 0.0f);

glm::vec3 Object1Center(0, 0, 0);
glm::vec3 Object2Center(0, 0, 0);
glm::vec3 BallCenter(0, 0, 0);

GLfloat boundspeed = 0.021 / 2;


GLfloat BallSpeed = 0.08;
GLfloat BallSpeedX = 0;
GLfloat BallSpeedY = 0;
GLint MoveDir = 0;//stop
GLfloat angle;
bool CanBound = true;

bool Hit1;
//Does it hit the Player2?
bool Hit2;

GLfloat speed = 0.02 / 2;
GLfloat speedX = 0.02 / 2;
GLfloat speedY = 0.02 / 2;

GLfloat speed2 = 0.02 / 2;
GLfloat speedX2 = 0.02 / 2;
GLfloat speedY2 = 0.02 / 2;

GLfloat stopspeed = 0.0002 / 2;

GLuint shader_program;
GLuint shader_program2;

unsigned char* image;
int width, height;

void loadImage() { // pass the filepath string as an argument
	int channels;
	stbi_set_flip_vertically_on_load(true);

	image = stbi_load("board.png", &width, &height, &channels, 0);

	if (image)
		std::cout << "Image loaded: " << width << " x " << height << std::endl;
	else std::cout << "Failed to load image!!!" << std::endl;

}

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(1000, 800, "INFR2670", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}




bool loadShaders(const char* v_shader, const char* f_shader, GLuint& sharderProgram) {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream(v_shader, std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream(f_shader, std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	sharderProgram = glCreateProgram();
	glAttachShader(sharderProgram, fs);
	glAttachShader(sharderProgram, vs);
	glLinkProgram(sharderProgram);

	return true;
}


glm::vec3 FindCenter(glm::vec3* point) {


	GLfloat Xmax = 0;
	GLfloat Ymax = 0;
	GLfloat Zmax = 0;

	GLfloat Xmin = 1000000;
	GLfloat Ymin = 1000000;
	GLfloat Zmin = 1000000;
	for (int i = 0; i < sizeof(*point) / sizeof(point[0]); i += 3) {
		if (point[i].x > Xmax) {
			Xmax = point[i].x;
		}
		if (point[i].y > Ymax) {
			Ymax = point[i].y;
		}
		if (point[i].z > Zmax) {
			Zmax = point[i].z;
		}
		if (point[i].x < Xmin) {
			Xmin = point[i].x;
		}
		if (point[i].y < Ymin) {
			Ymin = point[i].y;
		}
		if (point[i].z < Zmin) {
			Zmin = point[i].z;
		}
	}
	GLfloat centerX = abs((Xmax + Xmin) /4) ;
	GLfloat centerY = abs((Ymax + Ymin) /4) ;
	GLfloat centerZ = abs((Zmax + Zmin) /4) ;
	return glm::vec3(centerX, centerY, centerZ);
}

glm::vec3 FindBallCenter(glm::vec3* point) {

	GLfloat Xmax = 0;
	GLfloat Ymax = 0;
	GLfloat Zmax = 0;

	GLfloat Xmin = 1000000;
	GLfloat Ymin = 1000000;
	GLfloat Zmin = 1000000;
	for (int i = 0; i < sizeof(*point) / sizeof(point[0]); i += 3) {
		if (point[i].x > Xmax) {
			Xmax = point[i].x;
		}
		if (point[i].y > Ymax) {
			Ymax = point[i].y;
		}
		if (point[i].z > Zmax) {
			Zmax = point[i].z;
		}
		if (point[i].x < Xmin) {
			Xmin = point[i].x;
		}
		if (point[i].y < Ymin) {
			Ymin = point[i].y;
		}
		if (point[i].z < Zmin) {
			Zmin = point[i].z;
		}
	}
	GLfloat centerX = abs((Xmax + Xmin) /6) ;
	GLfloat centerY = abs((Ymax + Ymin) /6) ;
	GLfloat centerZ = abs((Zmax + Zmin) /6) ;
	return glm::vec3(centerX, centerY, centerZ);
}



bool IsBallHittingBound() {
	if (BalltranX <= -1.2 || BalltranX >= 1.2 || BalltranY <= -1.0 || BalltranY >= 1.0) {
		return true;
	}
	else {
		return false;
	}
}

void hitcheck() {
	if (tranX <= 0) {
		tranX += boundspeed;
	}
	if (tranX >= 1.2) {
		tranX -= boundspeed;
	}
	if (tranY <= -0.9) {
		tranY += boundspeed;
	}
	if (tranY >= 0.9) {
		tranY -= boundspeed;
	}

	if (tranX2 <= -1.2) {
		tranX2 += boundspeed;
	}
	if (tranX2 >= 0) {
		tranX2 -= boundspeed;
	}
	if (tranY2 <= -0.9) {
		tranY2 += boundspeed;
	}
	if (tranY2 >= 0.9) {
		tranY2 -= boundspeed;
	}

	if (BalltranX <= -1.28&& (BalltranY>0.30|| BalltranY < -0.30) || BalltranX >= 1.28 && (BalltranY > 0.30 || BalltranY < -0.30)) {
		BallSpeedX = BalltranX < 0 ? abs(BallSpeedX) : -abs(BallSpeedX);
	}
	if (BalltranY <= -1 || BalltranY >= 1) {
		BallSpeedY = BalltranY < 0 ? abs(BallSpeedY) : -abs(BallSpeedY);
	}
	if (BalltranX <= -1.4 ) {
		BalltranX = 0.5f;
		BalltranY = 0.001f;
		BallSpeedX = 0.0f;
		BallSpeedY = 0.0f;
		Playerscore1 = Playerscore1 + 1;
		tranX = 1.0f;
		tranY = 0.0f;
		tranX2 = -1.0f;
		tranY2 = 0.0f;
	}
	if (BalltranX >= 1.4) {
		BalltranX = -0.5f;
		BalltranY = 0.001f;
		BallSpeedX = 0.0f;
		BallSpeedY = 0.0f;
		Playerscore2 = Playerscore2 + 1;
		tranX = 1.0f;
		tranY = 0.0f;
		tranX2 = -1.0f;
		tranY2 = 0.0f;
	}
	if (Playerscore1 > 3 || Playerscore2 > 3) {
		Playerscore1 = 0;
		Playerscore2 = 0;
		BalltranX = 0.0f;
		BalltranY = 0.001f;
		BallSpeedX = 0.0f;
		BallSpeedY = 0.0f;
		tranX = 1.0f;
		tranY = 0.0f;
		tranX2 = -1.0f;
		tranY2 = 0.0f;
	}
}

bool hitbox(glm::vec3 position1, glm::vec3 position2, glm::vec3 center1, glm::vec3 center2) {
	float dis = sqrt(pow(position1.x - position2.x, 2) + pow(position1.y - position2.y, 2));
	float sumOfCenter = center1.x + center2.x;
	//std::cout << "dis" <<dis <<std::endl;
	//std::cout << "Sum" <<sumOfCenter << std::endl;
	if (dis - sumOfCenter <= 0) {
		return true;
	}
	return false;
}


void keyboard() {
	bool Ishit = hitbox(ObjectPosition1, ObjectPosition2, Object1Center, Object2Center);

	if (Ishit == false) {
		speedX = 0;
		speedY = 0;

		speedX2 = 0;
		speedY2 = 0;

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			speedX = -speed;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			speedX = speed;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			speedY = speed;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			speedY = -speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			speedX2 = -speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			speedX2 = speed;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			speedY2 = speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			speedY2 = -speed;
		}
	}
	else {
		speedX = speed+0.005;
		speedX2 = -speed-0.005;
		speedY = tranY > tranY2 ? speed + 0.005 : -speed - 0.005;
		speedY2 = tranY2 > tranY ? speed + 0.005 : -speed - 0.005;
	}

	tranX = tranX + speedX;
	tranY = tranY + speedY;

	tranX2 = tranX2 + speedX2;
	tranY2 = tranY2 + speedY2;

	hitcheck();
}

float FindAngle(glm::vec3 p1, glm::vec3 p2) {
	float x = abs(p1.x - p2.x);
	float y = abs(p1.y - p2.y);
	return atan(y / x);
}

void BallMove() {
	//Does it hit the Player1?
	Hit1 = hitbox(BallPosition, ObjectPosition1, BallCenter, Object1Center);
	//Does it hit the Player2?
	Hit2 = hitbox(BallPosition, ObjectPosition2, BallCenter, Object2Center);
	//std::cout << BallPosition.x << "," << BallPosition.y << std::endl;
	//std::cout << ObjectPosition1.x << "," << ObjectPosition1.y << std::endl;
	//std::cout  << std::endl;
	if (Hit1) {
		angle = FindAngle(BallPosition, ObjectPosition1);
		//Hit angle, Is the player hit from the top or bottom?
		if (ObjectPosition1.y > BallPosition.y) {//if it is from the top the ball will move in negitive
			//Hit angle, Is the player hit from the left or right?
			if (ObjectPosition1.x > BallPosition.x) {//if it is from the left the ball will move in negitive
				BallSpeedX = -(BallSpeed * cos(angle));
			}
			else if (ObjectPosition1.x < BallPosition.x) {//if it is from the right the ball will move in positive
				BallSpeedX = (BallSpeed * cos(angle));
			}
			BallSpeedY = -(BallSpeed * sin(angle));

		}
		else if (ObjectPosition1.y < BallPosition.y) {
			//Hit angle, Is the player hit from the left or right?
			if (ObjectPosition1.x > BallPosition.x) {//if it is from the left the ball will move in negitive
				BallSpeedX = -(BallSpeed * cos(angle));
			}
			else if (ObjectPosition1.x < BallPosition.x) {//if it is from the right the ball will move in positive
				BallSpeedX = (BallSpeed * cos(angle));
			}
			BallSpeedY = (BallSpeed * sin(angle));
		}
	}
	if (Hit2) {
		angle = FindAngle(BallPosition, ObjectPosition2);
		//Hit angle, Is the player hit from the top or bottom?
		if (ObjectPosition2.y > BallPosition.y) {//if it is from the top the ball will move in negitive
			//Hit angle, Is the player hit from the left or right?
			if (ObjectPosition2.x > BallPosition.x) {//if it is from the left the ball will move in negitive
				BallSpeedX = -(BallSpeed * cos(angle));
			}
			else if (ObjectPosition2.x < BallPosition.x) {//if it is from the right the ball will move in positive
				BallSpeedX = (BallSpeed * cos(angle));
			}
			BallSpeedY = -(BallSpeed * sin(angle));

		}
		else if (ObjectPosition2.y < BallPosition.y) {
			//Hit angle, Is the player hit from the left or right?
			if (ObjectPosition2.x > BallPosition.x) {//if it is from the left the ball will move in negitive
				BallSpeedX = -(BallSpeed * cos(angle));
			}
			else if (ObjectPosition2.x < BallPosition.x) {//if it is from the right the ball will move in positive
				BallSpeedX = (BallSpeed * cos(angle));
			}
			BallSpeedY = (BallSpeed * sin(angle));
		}
	}
	BallSpeedX = BallSpeedX > 0 ? BallSpeedX - 0.0001 : BallSpeedX + 0.0001;
	BallSpeedY = BallSpeedY > 0 ? BallSpeedY - 0.0001 : BallSpeedY + 0.0001;
	BalltranX = BalltranX + BallSpeedX;
	BalltranY = BalltranY + BallSpeedY;
}



// Color data
static const GLfloat colors[] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f
};

void Bind(GLuint buffer,GLuint pos_buffer, GLuint  color_buffer, GLuint normal_buffer, GLuint UV_buffer, int triangles) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, NULL);
}



int main() {
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;



	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Load shaders 
	if (!loadShaders("vertex_shader.glsl","frag_shader.glsl",shader_program))
		return 1;



	/////////////////Player 1//////////////////
	ObjLoader Player;
	Player.LoadFromFile("Player.obj");

	int nv = (int)Player.mesh.Position.size();
	int nn = (int)Player.mesh.Normal.size();
	int ni = (int)Player.mesh.Indices.size();
	int nt = (int)Player.mesh.UV.size();
	int triangles = 0;
	glm::vec3* vertics;
	glm::vec3* normals;
	glm::vec2* tex;
	GLuint* indices;

	vertics = new glm::vec3[nv];
	for (int i = 0; i < nv; i++) {
		vertics[i] = Player.mesh.Position[i];
	}

	normals = new glm::vec3[nn];
	for (int i = 0; i < nn; i++) {
		normals[i] = Player.mesh.Normal[i];
	}

	tex = new glm::vec2[nt];
	for (int i = 0; i < nt; i++) {
		tex[i] = Player.mesh.UV[i];
	}

	triangles = ni / 3;
	indices = new GLuint[ni];
	for (int i = 0; i < ni; i++) {
		indices[i] = Player.mesh.Indices[i];
	}


	//PLayer 1 VBO   
	GLuint pos_buffer = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
	glBufferData(GL_ARRAY_BUFFER, (nv) * sizeof(glm::vec3), vertics, GL_STATIC_DRAW);

	GLuint color_buffer = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, (nn) * sizeof(glm::vec3), normals, GL_STATIC_DRAW);

	GLuint UV_buffer = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer);
	glBufferData(GL_ARRAY_BUFFER, (nt) * sizeof(glm::vec2), tex, GL_STATIC_DRAW);

	GLuint ibuffer = 4;
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv * sizeof(GLuint) * 3, indices, GL_STATIC_DRAW);



	////////////////Player 2////////////////
	
	ObjLoader Player2;
	Player2.LoadFromFile("Player2.obj");

	int nv2 = (int)Player2.mesh.Position.size();
	int nn2 = (int)Player2.mesh.Normal.size();
	int ni2 = (int)Player2.mesh.Indices.size();
	int nt2 = (int)Player2.mesh.UV.size();
	int triangles2 = 0;
	glm::vec3* vertics2;
	glm::vec3* normals2;
	glm::vec2* tex2;
	GLuint* indices2;

	vertics2 = new glm::vec3[nv2];
	for (int i = 0; i < nv2; i++) {
		vertics2[i] = Player2.mesh.Position[i];
	}

	normals2 = new glm::vec3[nn2];
	for (int i = 0; i < nn2; i++) {
		normals2[i] = Player2.mesh.Normal[i];
	}

	tex2 = new glm::vec2[nt2];
	for (int i = 0; i < nt2; i++) {
		tex2[i] = Player2.mesh.UV[i];
	}

	triangles2 = ni2 / 3;
	indices2 = new GLuint[ni2];
	for (int i = 0; i < ni2; i++) {
		indices2[i] = Player2.mesh.Indices[i];
	}
	
	//Player 2 VBO

	GLuint pos_buffer2 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer2);
	glBufferData(GL_ARRAY_BUFFER, (nv2) * sizeof(glm::vec3), vertics2, GL_STATIC_DRAW);

	GLuint color_buffer2 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer2 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer2);
	glBufferData(GL_ARRAY_BUFFER, (nn2) * sizeof(glm::vec3), normals2, GL_STATIC_DRAW);

	GLuint UV_buffer2 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer2);
	glBufferData(GL_ARRAY_BUFFER, (nt2) * sizeof(glm::vec2), tex2, GL_STATIC_DRAW);

	GLuint ibuffer2 = 4;
	glGenBuffers(1, &ibuffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv2 * sizeof(GLuint) * 3, indices2, GL_STATIC_DRAW);


	//Ball
	ObjLoader Ball;
	Ball.LoadFromFile("Ball.obj");

	int nv3 = (int)Ball.mesh.Position.size();
	int nn3 = (int)Ball.mesh.Normal.size();
	int ni3 = (int)Ball.mesh.Indices.size();
	int nt3 = (int)Ball.mesh.UV.size();
	int triangles3 = 0;
	glm::vec3* vertics3;
	glm::vec3* normals3;
	glm::vec2* tex3;
	GLuint* indices3;

	vertics3 = new glm::vec3[nv3];
	for (int i = 0; i < nv3; i++) {
		vertics3[i] = Ball.mesh.Position[i];
	}

	normals3 = new glm::vec3[nn3];
	for (int i = 0; i < nn3; i++) {
		normals3[i] = Ball.mesh.Normal[i];
	}

	tex3 = new glm::vec2[nt3];
	for (int i = 0; i < nt3; i++) {
		tex3[i] = Ball.mesh.UV[i];
	}

	triangles3 = ni3 / 3;
	indices3 = new GLuint[ni3];
	for (int i = 0; i < ni3; i++) {
		indices3[i] = Ball.mesh.Indices[i];
	}

	//Ball VBO

	GLuint pos_buffer3 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer3);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer3);
	glBufferData(GL_ARRAY_BUFFER, (nv3) * sizeof(glm::vec3), vertics3, GL_STATIC_DRAW);

	GLuint color_buffer3 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer3);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer3 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer3);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer3);
	glBufferData(GL_ARRAY_BUFFER, (nn3) * sizeof(glm::vec3), normals3, GL_STATIC_DRAW);

	GLuint UV_buffer3 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer3);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer3);
	glBufferData(GL_ARRAY_BUFFER, (nt3) * sizeof(glm::vec2), tex3, GL_STATIC_DRAW);

	GLuint ibuffer3 = 4;
	glGenBuffers(1, &ibuffer3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv3 * sizeof(GLuint) * 3, indices3, GL_STATIC_DRAW);


	//Board
	ObjLoader Board;
	Board.LoadFromFile("Board.obj");

	int nv4 = (int)Board.mesh.Position.size();
	int nn4 = (int)Board.mesh.Normal.size();
	int ni4 = (int)Board.mesh.Indices.size();
	int nt4 = (int)Board.mesh.UV.size();
	int triangles4 = 0;
	glm::vec3* vertics4;
	glm::vec3* normals4;
	glm::vec2* tex4;
	GLuint* indices4;

	vertics4 = new glm::vec3[nv4];
	for (int i = 0; i < nv4; i++) {
		vertics4[i] = Board.mesh.Position[i];
	}

	normals4 = new glm::vec3[nn4];
	for (int i = 0; i < nn4; i++) {
		normals4[i] = Board.mesh.Normal[i];
	}

	tex4 = new glm::vec2[nt4];
	for (int i = 0; i < nt4; i++) {
		tex4[i] = Board.mesh.UV[i];
	}

	triangles4 = ni4 / 3;
	indices4 = new GLuint[ni4];
	for (int i = 0; i < ni4; i++) {
		indices4[i] = Board.mesh.Indices[i];
	}
	
	//Board VBO

	GLuint pos_buffer4 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer4);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer4);
	glBufferData(GL_ARRAY_BUFFER, (nv4) * sizeof(glm::vec3), vertics4, GL_STATIC_DRAW);

	GLuint color_buffer4 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer4);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer4 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer4);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer4);
	glBufferData(GL_ARRAY_BUFFER, (nn4) * sizeof(glm::vec3), normals4, GL_STATIC_DRAW);

	GLuint UV_buffer4 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer4);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer4);
	glBufferData(GL_ARRAY_BUFFER, (nt4) * sizeof(glm::vec2), tex4, GL_STATIC_DRAW);

	GLuint ibuffer4 = 4;
	glGenBuffers(1, &ibuffer4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv4 * sizeof(GLuint) * 3, indices4, GL_STATIC_DRAW);

	//load texture 
	loadImage();

	//do texture buffer
	GLuint tex_Buffer4 = 5;
	glGenTextures(1,&tex_Buffer4);
	glBindTexture(GL_TEXTURE_2D, tex_Buffer4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 
				0, GL_RGBA, GL_UNSIGNED_BYTE, image);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

	// free image
	stbi_image_free(image);


	//Number 1
	ObjLoader Num1;
	Num1.LoadFromFile("1.obj");

	int nv5 = (int)Num1.mesh.Position.size();
	int nn5 = (int)Num1.mesh.Normal.size();
	int ni5 = (int)Num1.mesh.Indices.size();
	int nt5 = (int)Num1.mesh.UV.size();
	int triangles5 = 0;
	glm::vec3* vertics5;
	glm::vec3* normals5;
	glm::vec2* tex5;
	GLuint* indices5;

	vertics5 = new glm::vec3[nv5];
	for (int i = 0; i < nv5; i++) {
		vertics5[i] = Num1.mesh.Position[i];
	}

	normals5 = new glm::vec3[nn5];
	for (int i = 0; i < nn5; i++) {
		normals5[i] = Num1.mesh.Normal[i];
	}

	tex5 = new glm::vec2[nt5];
	for (int i = 0; i < nt5; i++) {
		tex5[i] = Num1.mesh.UV[i];
	}

	triangles5 = ni5 / 3;
	indices5 = new GLuint[ni5];
	for (int i = 0; i < ni5; i++) {
		indices5[i] = Num1.mesh.Indices[i];
	}

	//Num1 VBO

	GLuint pos_buffer5 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer5);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer5);
	glBufferData(GL_ARRAY_BUFFER, (nv5) * sizeof(glm::vec3), vertics5, GL_STATIC_DRAW);

	GLuint color_buffer5 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer5);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer5 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer5);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer5);
	glBufferData(GL_ARRAY_BUFFER, (nn5) * sizeof(glm::vec3), normals5, GL_STATIC_DRAW);

	GLuint UV_buffer5 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer5);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer5);
	glBufferData(GL_ARRAY_BUFFER, (nt5) * sizeof(glm::vec2), tex5, GL_STATIC_DRAW);

	GLuint ibuffer5 = 4;
	glGenBuffers(1, &ibuffer5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer5);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv5 * sizeof(GLuint) * 3, indices5, GL_STATIC_DRAW);


	//Number 2
	ObjLoader Num2;
	Num2.LoadFromFile("2.obj");

	int nv6 = (int)Num2.mesh.Position.size();
	int nn6 = (int)Num2.mesh.Normal.size();
	int ni6 = (int)Num2.mesh.Indices.size();
	int nt6 = (int)Num2.mesh.UV.size();
	int triangles6 = 0;
	glm::vec3* vertics6;
	glm::vec3* normals6;
	glm::vec2* tex6;
	GLuint* indices6;

	vertics6 = new glm::vec3[nv6];
	for (int i = 0; i < nv6; i++) {
		vertics6[i] = Num2.mesh.Position[i];
	}

	normals6 = new glm::vec3[nn6];
	for (int i = 0; i < nn6; i++) {
		normals6[i] = Num2.mesh.Normal[i];
	}

	tex6 = new glm::vec2[nt6];
	for (int i = 0; i < nt6; i++) {
		tex6[i] = Num2.mesh.UV[i];
	}

	triangles6 = ni6 / 3;
	indices6 = new GLuint[ni6];
	for (int i = 0; i < ni6; i++) {
		indices6[i] = Num2.mesh.Indices[i];
	}

	//Num2 VBO

	GLuint pos_buffer6 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer6);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer6);
	glBufferData(GL_ARRAY_BUFFER, (nv6) * sizeof(glm::vec3), vertics6, GL_STATIC_DRAW);

	GLuint color_buffer6 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer6);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer6 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer6);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer6);
	glBufferData(GL_ARRAY_BUFFER, (nn6) * sizeof(glm::vec3), normals6, GL_STATIC_DRAW);

	GLuint UV_buffer6 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer6);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer6);
	glBufferData(GL_ARRAY_BUFFER, (nt6) * sizeof(glm::vec2), tex6, GL_STATIC_DRAW);

	GLuint ibuffer6 = 4;
	glGenBuffers(1, &ibuffer6);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer6);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv6 * sizeof(GLuint) * 3, indices6, GL_STATIC_DRAW);


	//Number 3
	ObjLoader Num3;
	Num3.LoadFromFile("3.obj");

	int nv7 = (int)Num3.mesh.Position.size();
	int nn7 = (int)Num3.mesh.Normal.size();
	int ni7 = (int)Num3.mesh.Indices.size();
	int nt7 = (int)Num3.mesh.UV.size();
	int triangles7 = 0;
	glm::vec3* vertics7;
	glm::vec3* normals7;
	glm::vec2* tex7;
	GLuint* indices7;

	vertics7 = new glm::vec3[nv7];
	for (int i = 0; i < nv7; i++) {
		vertics7[i] = Num3.mesh.Position[i];
	}

	normals7 = new glm::vec3[nn7];
	for (int i = 0; i < nn7; i++) {
		normals7[i] = Num3.mesh.Normal[i];
	}

	tex7 = new glm::vec2[nt7];
	for (int i = 0; i < nt7; i++) {
		tex7[i] = Num3.mesh.UV[i];
	}

	triangles7 = ni7 / 3;
	indices7 = new GLuint[ni7];
	for (int i = 0; i < ni7; i++) {
		indices7[i] = Num3.mesh.Indices[i];
	}

	//Num3 VBO

	GLuint pos_buffer7 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer7);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer7);
	glBufferData(GL_ARRAY_BUFFER, (nv7) * sizeof(glm::vec3), vertics7, GL_STATIC_DRAW);

	GLuint color_buffer7 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer7);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer7 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer7);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer7);
	glBufferData(GL_ARRAY_BUFFER, (nn7) * sizeof(glm::vec3), normals7, GL_STATIC_DRAW);

	GLuint UV_buffer7 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer7);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer7);
	glBufferData(GL_ARRAY_BUFFER, (nt7) * sizeof(glm::vec2), tex7, GL_STATIC_DRAW);

	GLuint ibuffer7 = 4;
	glGenBuffers(1, &ibuffer7);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer7);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv7 * sizeof(GLuint) * 3, indices7, GL_STATIC_DRAW);

	//Num0
	ObjLoader Num0;
	Num0.LoadFromFile("0.obj");

	int nv9 = (int)Num0.mesh.Position.size();
	int nn9 = (int)Num0.mesh.Normal.size();
	int ni9 = (int)Num0.mesh.Indices.size();
	int nt9 = (int)Num0.mesh.UV.size();
	int triangles9 = 0;
	glm::vec3* vertics9;
	glm::vec3* normals9;
	glm::vec2* tex9;
	GLuint* indices9;

	vertics9 = new glm::vec3[nv9];
	for (int i = 0; i < nv9; i++) {
		vertics9[i] = Num0.mesh.Position[i];
	}

	normals9 = new glm::vec3[nn9];
	for (int i = 0; i < nn9; i++) {
		normals9[i] = Num0.mesh.Normal[i];
	}

	tex9 = new glm::vec2[nt9];
	for (int i = 0; i < nt9; i++) {
		tex9[i] = Num0.mesh.UV[i];
	}

	triangles9 = ni9 / 3;
	indices9 = new GLuint[ni9];
	for (int i = 0; i < ni9; i++) {
		indices9[i] = Num0.mesh.Indices[i];
	}

	//Num0 VBO

	GLuint pos_buffer9 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer9);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer9);
	glBufferData(GL_ARRAY_BUFFER, (nv9) * sizeof(glm::vec3), vertics9, GL_STATIC_DRAW);

	GLuint color_buffer9 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer9);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer9);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer9 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer9);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer9);
	glBufferData(GL_ARRAY_BUFFER, (nn9) * sizeof(glm::vec3), normals9, GL_STATIC_DRAW);

	GLuint UV_buffer9 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer9);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer9);
	glBufferData(GL_ARRAY_BUFFER, (nt9) * sizeof(glm::vec2), tex9, GL_STATIC_DRAW);

	GLuint ibuffer9 = 4;
	glGenBuffers(1, &ibuffer9);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer9);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv9 * sizeof(GLuint) * 3, indices9, GL_STATIC_DRAW);


	//crown
	ObjLoader Crown;
	Crown.LoadFromFile("crown.obj");

	int nv8 = (int)Crown.mesh.Position.size();
	int nn8 = (int)Crown.mesh.Normal.size();
	int ni8 = (int)Crown.mesh.Indices.size();
	int nt8 = (int)Crown.mesh.UV.size();
	int triangles8 = 0;
	glm::vec3* vertics8;
	glm::vec3* normals8;
	glm::vec2* tex8;
	GLuint* indices8;

	vertics8 = new glm::vec3[nv8];
	for (int i = 0; i < nv8; i++) {
		vertics8[i] = Crown.mesh.Position[i];
	}

	normals8 = new glm::vec3[nn8];
	for (int i = 0; i < nn8; i++) {
		normals8[i] = Crown.mesh.Normal[i];
	}

	tex8 = new glm::vec2[nt8];
	for (int i = 0; i < nt8; i++) {
		tex8[i] = Crown.mesh.UV[i];
	}

	triangles8 = ni8 / 3;
	indices8 = new GLuint[ni8];
	for (int i = 0; i < ni8; i++) {
		indices8[i] = Crown.mesh.Indices[i];
	}

	//crown VBO

	GLuint pos_buffer8 = glGetAttribLocation(shader_program, "inPosition");
	glGenBuffers(1, &pos_buffer8);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer8);
	glBufferData(GL_ARRAY_BUFFER, (nv8) * sizeof(glm::vec3), vertics8, GL_STATIC_DRAW);

	GLuint color_buffer8 = glGetAttribLocation(shader_program, "inColor");
	glGenBuffers(1, &color_buffer8);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint normal_buffer8 = glGetAttribLocation(shader_program, "inNormal");
	glGenBuffers(1, &normal_buffer8);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer8);
	glBufferData(GL_ARRAY_BUFFER, (nn8) * sizeof(glm::vec3), normals8, GL_STATIC_DRAW);

	GLuint UV_buffer8 = glGetAttribLocation(shader_program, "inUV");
	glGenBuffers(1, &UV_buffer8);
	glBindBuffer(GL_ARRAY_BUFFER, UV_buffer8);
	glBufferData(GL_ARRAY_BUFFER, (nt8) * sizeof(glm::vec2), tex8, GL_STATIC_DRAW);

	GLuint ibuffer8 = 4;
	glGenBuffers(1, &ibuffer8);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer8);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nv8 * sizeof(GLuint) * 3, indices8, GL_STATIC_DRAW);







	GLfloat cameraPos[] = { 0.0f, 0.0f, 3.0f };
	GLfloat lightPos[] = { 0.0f, 0.0f, 3.0f };


	Object1Center = FindCenter(vertics);
	Object2Center = FindCenter(vertics);
	BallCenter = FindBallCenter(vertics);



	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2); // Lecture 5
	glEnableVertexAttribArray(3);


	// Lecture 04
	// Projection matrix : 45� Field of View, ratio, display range : 0.1 unit <-> 100 units
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 Projection =
		glm::perspective(glm::radians(50.0f),
			(float)width / (float)height, 0.1f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 3), // Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID =
		glGetUniformLocation(shader_program, "MVP");
	GLuint ViewID =
		glGetUniformLocation(shader_program, "View");
	GLuint ModelID =
		glGetUniformLocation(shader_program, "Model");
	GLuint LightPosID =
		glGetUniformLocation(shader_program, "LightPos");
	GLuint ColourID =
		glGetUniformLocation(shader_program, "Colour");
	GLuint HasTexture =
		glGetUniformLocation(shader_program, "HasTexture");
	GLuint TextureID =
		glGetUniformLocation(shader_program, "texture1");

	glUniform1i(TextureID, 0);

	// /// ////////////////
	// GLuint MatrixID2 =
	// 	glGetUniformLocation(shader_program2, "MVP");
	// GLuint ViewID2 =
	// 	glGetUniformLocation(shader_program2, "View");
	// GLuint ModelID2 =
	// 	glGetUniformLocation(shader_program2, "Model");
	// GLuint LightPosID2 =
	// 	glGetUniformLocation(shader_program2, "LightPos");
	// GLuint ColourID2 =
	// 	glGetUniformLocation(shader_program2, "Colour");
	/// ////////////////

	glEnable(GL_DEPTH_TEST);

	// Face culling
	glEnable(GL_CULL_FACE);

	const double maxFPS = 60.0;
	const double maxPeriod = 1000 / maxFPS;

	double frameStart;
	double frameTime;
	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		frameStart = glfwGetTime();
		
		// Calculate the time since our last frame (dt)
			glfwPollEvents();
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_Buffer4);

			glUseProgram(shader_program);
			// Player 1 
			MatrixID =
				glGetUniformLocation(shader_program, "MVP");
			ViewID =
				glGetUniformLocation(shader_program, "View");
			ModelID =
				glGetUniformLocation(shader_program, "Model");
			LightPosID =
				glGetUniformLocation(shader_program, "LightPos");
			ColourID =
				glGetUniformLocation(shader_program, "Colour");

			Model = glm::mat4(1.0f);
			keyboard();												//X	    Y     Z
			//Model = glm::rotate(Model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			//Model = glm::rotate(Model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
			ObjectPosition1 = glm::vec3(tranX, tranY, 0.05f);
			Model = glm::translate(Model, ObjectPosition1);
			Model = glm::scale(Model, glm::vec3(playerSize, playerSize, playerSize));
			mvp = Projection * View * Model;


			glUniformMatrix4fv(MatrixID, 1,
				GL_FALSE, &mvp[0][0]);

			glUniformMatrix4fv(ViewID, 1,
				GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(ModelID, 1,
				GL_FALSE, &Model[0][0]);
			glUniform3fv(LightPosID, 1, &lightPos[0]);
			glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(1, 0, 0)));
			glUniform1i(HasTexture, 0);

			Bind(ibuffer, pos_buffer, color_buffer, normal_buffer, UV_buffer, triangles);


			// Player 2


			Model = glm::mat4(1.0f);
			keyboard();												//X	    Y     Z
			//Model = glm::rotate(Model, 10*glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			//Model = glm::rotate(Model, 10*glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
			ObjectPosition2 = glm::vec3(tranX2, tranY2, 0.05f);
			Model = glm::translate(Model, ObjectPosition2);
			Model = glm::scale(Model, glm::vec3(playerSize, playerSize, playerSize));
			mvp = Projection * View * Model;


			glUniformMatrix4fv(MatrixID, 1,
				GL_FALSE, &mvp[0][0]);

			glUniformMatrix4fv(ViewID, 1,
				GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(ModelID, 1,
				GL_FALSE, &Model[0][0]);
			glUniform3fv(LightPosID, 1, &lightPos[0]);
			glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 1)));
			glUniform1i(HasTexture, 0);

			Bind(ibuffer2, pos_buffer2, color_buffer2, normal_buffer2, UV_buffer2, triangles2);



			//BALL

			Model = glm::mat4(1.0f);
			BallMove();
			BallPosition = glm::vec3(BalltranX, BalltranY, 0.05f);
			Model = glm::translate(Model, BallPosition);
			Model = glm::scale(Model, glm::vec3(BallSize, BallSize, BallSize));
			mvp = Projection * View * Model;

			glUniformMatrix4fv(MatrixID, 1,
				GL_FALSE, &mvp[0][0]);
			// Lecture 5
			glUniformMatrix4fv(ViewID, 1,
				GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(ModelID, 1,
				GL_FALSE, &Model[0][0]);
			glUniform3fv(LightPosID, 1, &lightPos[0]);
			glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 1, 0)));
			glUniform1i(HasTexture, 0);

			Bind(ibuffer3, pos_buffer3, color_buffer3, normal_buffer3, UV_buffer3, triangles3);

			//ScordBoard
			if (Playerscore1 == 1) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer5, pos_buffer5, color_buffer5, normal_buffer5, UV_buffer5, triangles5);
			}
			else if (Playerscore1 == 2) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer6, pos_buffer6, color_buffer6, normal_buffer6, UV_buffer6, triangles6);
			}
			else if (Playerscore1 == 3) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer7, pos_buffer7, color_buffer7, normal_buffer7, UV_buffer7, triangles7);
			}
			else {
				Playerscore1 = 0;
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer9, pos_buffer9, color_buffer9, normal_buffer9, UV_buffer9, triangles9);
			}
			//Player2 Score
			if (Playerscore2 == 1) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(-1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer5, pos_buffer5, color_buffer5, normal_buffer5, UV_buffer5, triangles5);
			}
			else if (Playerscore2 == 2) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(-1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer6, pos_buffer6, color_buffer6, normal_buffer6, UV_buffer6, triangles6);
			}
			else if (Playerscore2 == 3) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(-1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer7, pos_buffer7, color_buffer7, normal_buffer7, UV_buffer7, triangles7);
			}
			else {
				Playerscore2 = 0;
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(-1.0, 0.8, 0.5));
				Model = glm::scale(Model, glm::vec3(0.01, 0.01, 0.01));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Player1 Score
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer9, pos_buffer9, color_buffer9, normal_buffer9, UV_buffer9, triangles9);
			}



			if (Playerscore1 == 3) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(tranX, tranY + 0.3, 0.1));
				Model = glm::scale(Model, glm::vec3(0.2, 0.2, 0.02));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Lecture 5
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(1, 1, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer8, pos_buffer8, color_buffer8, normal_buffer8, UV_buffer8, triangles8);
			}

			if (Playerscore2 == 3) {
				Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(tranX2, tranY2 + 0.3, 0.1));
				Model = glm::scale(Model, glm::vec3(0.2, 0.2, 0.02));
				mvp = Projection * View * Model;

				glUniformMatrix4fv(MatrixID, 1,
					GL_FALSE, &mvp[0][0]);
				// Lecture 5
				glUniformMatrix4fv(ViewID, 1,
					GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(ModelID, 1,
					GL_FALSE, &Model[0][0]);
				glUniform3fv(LightPosID, 1, &lightPos[0]);
				glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(1, 1, 0)));
				glUniform1i(HasTexture, 0);
				Bind(ibuffer8, pos_buffer8, color_buffer8, normal_buffer8, UV_buffer8, triangles8);
			}

			//Board

			Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(0, 0, 0));
			Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
			mvp = Projection * View * Model;

			glUniformMatrix4fv(MatrixID, 1,
				GL_FALSE, &mvp[0][0]);
			// Lecture 5
			glUniformMatrix4fv(ViewID, 1,
				GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(ModelID, 1,
				GL_FALSE, &Model[0][0]);
			glUniform3fv(LightPosID, 1, &lightPos[0]);
			glUniform3fv(ColourID, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
			glUniform1i(HasTexture, 1);

			Bind(ibuffer4, pos_buffer4, color_buffer4, normal_buffer4, UV_buffer4, triangles4);

			glfwSwapBuffers(window);
			frameTime = glfwGetTime()-frameStart;
			if (frameDelay > frameTime) {
				_sleep(frameDelay-frameTime);
			}
	}
	return 0;

}