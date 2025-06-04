#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int setupShader();
int setupSprite();
int loadTexture(string filePath);

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec2 texc;
 uniform mat4 model;
 uniform mat4 projection;
 out vec2 tex_coord;
 void main()
 {
    tex_coord = vec2(texc.s, texc.t);
    gl_Position = projection * model * vec4(position, 1.0);
 }
 )";

const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec2 tex_coord;
 out vec4 color;
 uniform sampler2D tex_buff;
 void main()
 {
	 color = texture(tex_buff,tex_coord);
 }
 )";

GLuint bgTex[5];
float parallax[6] = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
float playerX = 50.0f, playerY = 470.0f;
float speed = 8.0f;
glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

int main()
{
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Módulo 4 - Sprites", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	GLuint shaderID = setupShader();
	GLuint VAO = setupSprite();
    bgTex[0] = loadTexture("../src/Desafios/DesafioM4/assets/Ice.png");
    bgTex[1] = loadTexture("../src/Desafios/DesafioM4/assets/Mountain.png");
    bgTex[2] = loadTexture("../src/Desafios/DesafioM4/assets/Planet.png");
    bgTex[3] = loadTexture("../src/Desafios/DesafioM4/assets/Saturn.png");
    bgTex[4] = loadTexture("../src/Desafios/DesafioM4/assets/Space.png");
	GLuint playerTex = loadTexture("../src/Desafios/DesafioM4/assets/Nave.png");
	glUseProgram(shaderID);
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	GLint projLoc = glGetUniformLocation(shaderID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	double prev_s = glfwGetTime();
	double title_countdown_s = 0.1;
	float colorValue = 0.0;
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderID, "tex_buff"), 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		{
			double curr_s = glfwGetTime();
			double elapsed_s = curr_s - prev_s;
			prev_s = curr_s;
			title_countdown_s -= elapsed_s;
			if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
			{
				double fps = 1.0 / elapsed_s;
				char tmp[256];
				sprintf(tmp, "FPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);
				title_countdown_s = 0.1;
			}
		}
		glfwPollEvents();
		glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, bgTex[0]);
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(800.0f, 600.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        for (int i = 1; i < 5; i++)
        {
            glBindTexture(GL_TEXTURE_2D, bgTex[i]);
            float spriteX = 80.0f + (i-1) * 140.0f;
            float spriteY = 470.0f;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(spriteX, spriteY, 0.0f));
            model = glm::scale(model, glm::vec3(128.0f, 128.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
		glBindTexture(GL_TEXTURE_2D, playerTex);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(playerX, playerY, 0.0f));
		model = glm::scale(model, glm::vec3(128.0f, 128.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

int setupShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

int setupSprite()
{
	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return VAO;
}

int loadTexture(string filePath)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}