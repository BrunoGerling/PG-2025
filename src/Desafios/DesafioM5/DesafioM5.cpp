#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

// --- SHADER SOURCES ---
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform float offset;
    uniform float scale;
    void main() {
        vec3 pos = aPos * scale;
        gl_Position = vec4(pos, 1.0);
        TexCoord = vec2(aTexCoord.x - offset, aTexCoord.y);
    }
)";
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() { FragColor = texture(texture1, TexCoord); }
)";

// --- LAYER STRUCT ---
struct Layer {
    unsigned int textureID;
    float speed;
    float offset;
};

// --- WINDOW SIZE CONSTANTS ---
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

// --- TEXTURE LOADING FUNCTION ---
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {
        GLenum format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

int main() {
    // --- GLFW/GLAD/OPENGL INIT ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "parallax", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // --- BLENDING/SHADER/VAO/VBO SETUP ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float quadVertices[] = {
        // positions       // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // --- LAYER SETUP ---
    const char* layerPaths[4] = {
        "../src/Desafios/DesafioM5/assets/Double_Jump.png",
        "../src/Desafios/DesafioM5/assets/Platform.png",
        "../src/Desafios/DesafioM5/assets/Running.png",
        "../src/Desafios/DesafioM5/assets/Side_Jump.png"
    };
    const int layerCount = 4;
    float minSpeed = 0.05f;
    float maxSpeed = 1.50f;
    float scale    = 1.0f;
    Layer layers[layerCount];
    for (int i = 0; i < layerCount; ++i) {
        layers[i].textureID = loadTexture(layerPaths[i]);
        layers[i].speed     = minSpeed + (maxSpeed - minSpeed) * ((float)i / (layerCount - 1));
        layers[i].offset    = 0.0f;
    }

    // --- CHARACTER TEXTURE (apenas 1 textura para personagem) ---
    unsigned int charTexture = loadTexture("../src/Desafios/DesafioM5/assets/Crouch.png");

    // --- CHARACTER POSITION/TAMANHO ---
    float charX = 0.0f;
    float charY = -0.5f;
    float charW = 0.2f;
    float charH = 0.4f;

    // --- CHARACTER VAO/VBO SETUP ---
    unsigned int charVBO, charVAO;
    glGenVertexArrays(1, &charVAO);
    glGenBuffers(1, &charVBO);
    glBindVertexArray(charVAO);
    glBindBuffer(GL_ARRAY_BUFFER, charVBO);

    // Usamos um array fixo para os vertices, com coords completas (posição + textura)
    float charVertices[30];
    glBufferData(GL_ARRAY_BUFFER, sizeof(charVertices), charVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- MAIN LOOP ---
    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float delta = now - lastTime;
        lastTime = now;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        // --- INPUT HANDLING ---
        bool leftPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool rightPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        // --- MOVEMENT LOGIC ---
        bool isMoving = rightPressed || leftPressed;
        float moveDir = rightPressed ? 1.0f : (leftPressed ? -1.0f : 0.0f);
        float baseSpeed = 0.5f;

        if (isMoving) {
            float moveSpeed = baseSpeed * moveDir;
            for (int i = 0; i < layerCount; ++i) {
                layers[i].offset += layers[i].speed * moveSpeed * delta;
                if (layers[i].offset > 1.0f) layers[i].offset -= 1.0f;
                else if (layers[i].offset < 0.0f) layers[i].offset += 1.0f;
            }
            charX += moveSpeed * delta;
        }

        // --- CLEAR SCREEN ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- DRAW LAYERS ---
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        for (int i = 0; i < layerCount; ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, layers[i].textureID);
            glUniform1f(glGetUniformLocation(shaderProgram, "offset"), layers[i].offset);
            glUniform1f(glGetUniformLocation(shaderProgram, "scale"), scale);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // --- DRAW CHARACTER ---
        float left = charX - charW / 2.0f;
        float right = charX + charW / 2.0f;
        float top = charY + charH / 2.0f;
        float bottom = charY - charH / 2.0f;

        float characterVertices[] = {
            // positions         // tex coords
            left,  top,    0.0f, 0.0f, 1.0f,
            left,  bottom, 0.0f, 0.0f, 0.0f,
            right, bottom, 0.0f, 1.0f, 0.0f,
            left,  top,    0.0f, 0.0f, 1.0f,
            right, bottom, 0.0f, 1.0f, 0.0f,
            right, top,    0.0f, 1.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, charVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(characterVertices), characterVertices);
        glBindVertexArray(charVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, charTexture);
        glUniform1f(glGetUniformLocation(shaderProgram, "offset"), 0.0f); // personagem não usa offset
        glUniform1f(glGetUniformLocation(shaderProgram, "scale"), 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- SWAP BUFFERS/POLL EVENTS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
