/* EXERCICIO 01
Essa função deve criar um triângulo com as coordenadas dos vértices v0, v1 e v2 passadas por parâmetro
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

//Protótipos das funções
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
int setupShader();
int setupGeometry();

//Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = "#version 400\n"
								   "layout (location = 0) in vec3 position;\n"
								   "uniform mat4 projection;\n"
								   "uniform mat4 model;\n"
								   "void main()\n"
								   "{\n"
								   //...pode ter mais linhas de código aqui!
								   "gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
								   "}\0";

//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = "#version 400\n"
									 "uniform vec4 inputColor;\n"
									 "out vec4 color;\n"
									 "void main()\n"
									 "{\n"
									 "color = inputColor;\n"
									 "}\n\0";

int main()
{
	//Inicialização da GLFW
	glfwInit();

	//Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Rossana", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	//GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	//Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	//Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

    //Instanciando triângulo com coordenadas diferentes
	vector<GLuint> VAOs;
	VAOs.push_back(createTriangle(-0.65, 0.33, -0.27, 0.53, -0.61, 0.79));
    
	glUseProgram(shaderID);

	//Enviando a cor desejada (vec4) para o fragment shader
	//Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	//Matriz de projeção paralela ortográfica
	//mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
	mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	//Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); //matriz identidade

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
	glUniform4f(colorLoc, 0.0f, 0.0f, 1.0, 1.0f); //enviando cor para variável uniform inputColor

	//Loop da aplicação - "game loop"
	while(!glfwWindowShouldClose(window))
	{
		//Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		//Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		for (int i = 0; i < VAOs.size(); i++)
		{
			glBindVertexArray(VAOs[i]); //Conectando ao buffer de geometria
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
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
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
	if (!success)
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
	if(!success)
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

	//Desvincula o VAO
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

	//Desvincula o VAO 
    glBindVertexArray(0);

	return VAO;
}
