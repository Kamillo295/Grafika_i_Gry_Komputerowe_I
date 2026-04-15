#pragma comment(lib, "opengl32.lib")

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stack>

// Biblioteki matematyczne 3D
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Zmienne sterujące
int object = 1;
float rotX = 0.0f, rotY = 0.0f;

// Shadery
const char* vertexSrc = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;
    layout(location = 2) in float aPS;
    
    out vec3 color;
    uniform mat4 MVP;
    
    void main() {
        gl_Position = MVP * vec4(aPos, 1.0);
        gl_PointSize = aPS;
        color = aColor;
    }
)";

const char* fragmentSrc = R"(
    #version 330 core
    in vec3 color;
    out vec4 fragColor;
    
    void main() {
        // Zamiana punktów w okręgi
        vec2 c = gl_PointCoord - vec2(0.5);
        if (dot(c, c) > 0.25) discard;
        fragColor = vec4(color, 1.0);
    }
)";

// Kompilacja shaderów
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
    }
    return shader;
}

// Tworzenie programu shaderów
GLuint createProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Obsługa zmiany rozmiaru okna
void onResize(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

// Inicjalizacja okna
GLFWwindow* initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Zadanie 1 i 2 - Kolo", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onResize);
    glewExperimental = GL_TRUE;
    glewInit();
    return window;
}

// Obsługa klawiatury
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_1) object = 1;
        if (key == GLFW_KEY_2) object = 2;
        if (key == GLFW_KEY_UP) rotX -= 5.0f;
        if (key == GLFW_KEY_DOWN) rotX += 5.0f;
        if (key == GLFW_KEY_LEFT) rotY -= 5.0f;
        if (key == GLFW_KEY_RIGHT) rotY += 5.0f;
    }
}

// Tworzenie VAO i VBO
GLuint createVAO(const std::vector<float>& data) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    // Atrybuty: pozycja, kolor, rozmiar punktu
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    return VAO;
}

// Program główny
int main() {
    GLFWwindow* window = initWindow();
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);         // Test głębokości
    glEnable(GL_PROGRAM_POINT_SIZE); // Rozmiar punktów w shaderze

    GLuint program = createProgram();
    GLint uMVP = glGetUniformLocation(program, "MVP");

    // Dane korkociągu
    std::vector<float> corkData;
    for (int i = 0; i < 300; ++i) {
        float t = i * 0.1f;
        corkData.push_back(0.5f * cos(t));
        corkData.push_back(-0.8f + i * 0.005f);
        corkData.push_back(0.5f * sin(t));
        corkData.push_back(0.2f); corkData.push_back(0.8f); corkData.push_back(0.2f);
        corkData.push_back(2.0f + i * 0.05f);
    }
    GLuint vaoCork = createVAO(corkData);

    // Dane stożka
    int N = 500;
    float angleStep = 360.0f / N;

    std::vector<float> faceData = {
        0.0f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.5f, 1.0f,  1.0f,
        0.5f * cos(glm::radians(angleStep)), -0.5f,
        0.5f * sin(glm::radians(angleStep)), 0.0f, 1.0f, 0.5f, 1.0f
    };
    GLuint vaoFace = createVAO(faceData);

    std::vector<float> baseData;
    baseData.push_back(0.0f); baseData.push_back(-0.5f); baseData.push_back(0.0f);
    baseData.push_back(0.3f); baseData.push_back(0.3f); baseData.push_back(0.3f);
    baseData.push_back(1.0f);
    for (int i = 0; i <= N; ++i) {
        float a = glm::radians(i * angleStep);
        baseData.push_back(0.5f * cos(a));
        baseData.push_back(-0.5f);
        baseData.push_back(0.5f * sin(a));
        baseData.push_back(0.3f); baseData.push_back(0.3f); baseData.push_back(0.3f);
        baseData.push_back(1.0f);
    }
    GLuint vaoBase = createVAO(baseData);

    // Pętla renderująca
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 50.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(rotX), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0));

        glm::mat4 MVP = proj * view * model;
        glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(MVP));

        if (object == 1) {
            glBindVertexArray(vaoCork);
            glDrawArrays(GL_POINTS, 0, 300);
        }
        else if (object == 2) {
            std::stack<glm::mat4> ms;
            ms.push(MVP);

            // Podstawa stożka
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(ms.top()));
            glBindVertexArray(vaoBase);
            glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);

            // Ściany boczne
            for (int i = 0; i < N; ++i) {
                ms.push(ms.top());
                ms.top() = glm::rotate(ms.top(), glm::radians(i * angleStep), glm::vec3(0, 1, 0));

                glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(ms.top()));
                glBindVertexArray(vaoFace);
                glDrawArrays(GL_TRIANGLES, 0, 3);

                ms.pop();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}