/* EXERCÍCIO 03
O programa deverá criar novos triângulos posicionados a partir do clique do mouse na tela. 
A cor de cada triângulo deve variar, sorteando-se valores para as componentes RGB da cor
*/

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

using namespace std;

//GLAD
#include <glad/glad.h>

//GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <cmath>

//Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//Protótipos das funções
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
int setupShader();
int setupGeometry();

//Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main()	
{
	//...pode ter mais linhas de código aqui!
	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
}
)";

//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main()
{
	color = inputColor;
}
)";

struct Triangle 
{
	vec3 position;
	vec3 dimensions;
	vec3 color;
};

vector<Triangle> triangles;

vector <vec3> colors;
int iColor = 0;

int main()
{
	//Inicialização da GLFW
	glfwInit();

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Rossana", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//GLAD: carrega todos os ponteiros de funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	//Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); 
	const GLubyte *version = glGetString(GL_VERSION);	
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;


	//Inicializando paleta de cores
	colors.push_back(vec3(200, 191, 231));
	colors.push_back(vec3(174, 217, 224));
	colors.push_back(vec3(181, 234, 215));
	colors.push_back(vec3(255, 241, 182));
	colors.push_back(vec3(255, 188, 188));
	colors.push_back(vec3(246, 193, 199));
	colors.push_back(vec3(255, 216, 190));
	colors.push_back(vec3(220, 198, 224));
	colors.push_back(vec3(208, 230, 165));
	colors.push_back(vec3(183, 201, 226));

	//Normalizar as cores entre 0 e 1
	for(int i=0; i<colors.size(); i++)
	{
		colors[i].r /= 255.0;
		colors[i].g /= 255.0;
		colors[i].b /= 255.0;
	}

	//Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	
	GLuint VAO = createTriangle(-0.5,-0.5,0.5,-0.5,0.0,0.5);
	
	Triangle tri;
	tri.position = vec3(400.0,300.0,0.0);
	tri.dimensions = vec3(100.0,100.0,1.0);
	tri.color = vec3(colors[iColor].r, colors[iColor].g, colors[iColor].b);
	iColor = (iColor + 1) % colors.size();
	triangles.push_back(tri);


	glUseProgram(shaderID);
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");
	mat4 projection = ortho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	//Loop da aplicação - "game loop"
	while(!glfwWindowShouldClose(window))
	{
		//Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		//Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glBindVertexArray(VAO); //Conectando ao buffer de geometria

		for (int i = 0; i < triangles.size(); i++)
		{
			
			mat4 model = mat4(1); //matriz identidade
			model = translate(model,vec3(triangles[i].position.x,triangles[i].position.y,0.0));
			model = rotate(model,radians(180.0f),vec3(0.0,0.0,1.0));
			model = scale(model,vec3(triangles[i].dimensions.x,triangles[i].dimensions.y,1.0));
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
			glUniform4f(colorLoc, triangles[i].color.r, triangles[i].color.g, triangles[i].color.b, 1.0f); // enviando cor para variável uniform inputColor
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		glBindVertexArray(0); //Desconectando o buffer de geometria

		//Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
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
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	//Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//Checando por erros de linkagem
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

int setupGeometry()
{
	GLfloat vertices[] = {
		-0.5, -0.5, 0.0, //v0
		0.5, -0.5, 0.0,	 //v1
		0.0, 0.5, 0.0,	 //v2
	};

	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
	GLuint VAO;

	GLfloat vertices[] = {
		x0, y0, 0.0, //v0
		x1, y1, 0.0, //v1
		x2, y2, 0.0, //v2
	};

	GLuint VBO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		cout << xpos << "  " << ypos << endl;

		Triangle tri;
		tri.position = vec3(xpos,ypos,0.0);
		tri.dimensions = vec3(100.0,100.0,1.0);
		tri.color = vec3(colors[iColor].r, colors[iColor].g, colors[iColor].b);
		iColor = (iColor + 1) % colors.size();
		triangles.push_back(tri);
		
	}
}
