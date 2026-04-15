#pragma comment(lib, "opengl32.lib")

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

// Shadery
const char* vSrc = R"(
    #version 330 core
    layout(location=0) in vec3 P;
    layout(location=1) in vec3 C;
    layout(location=2) in float S;
    uniform mat4 MVP;
    out vec3 col;
    void main() {
        gl_Position = MVP * vec4(P, 1.0);
        gl_PointSize = S;
        col = C;
    }
)";

const char* fSrc = R"(
    #version 330 core
    in vec3 col;
    out vec4 fCol;
    void main() {
        // Punkty jako okręgi
        if(dot(gl_PointCoord - vec2(0.5), gl_PointCoord - vec2(0.5)) > 0.25)
            discard;
        fCol = vec4(col, 1.0);
    }
)";

// Tworzenie VAO/VBO
GLuint makeVAO(std::vector<float>& data) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
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

int main() {
    // Inicjalizacja GLFW i okna
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(800, 600, "Najprostsza Wersja", NULL, NULL);
    glfwMakeContextCurrent(win);

    // Inicjalizacja GLEW i ustawienia OpenGL
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Tworzenie programu shaderów
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, NULL);
    glCompileShader(fs);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint uMVP = glGetUniformLocation(prog, "MVP");

    // Korkociąg
    std::vector<float> cork;
    for (float i = 0; i < 30; i += 0.1f) {
        cork.insert(cork.end(), {
            0.5f * cos(i), i * 0.06f - 0.8f, 0.5f * sin(i),
            0.2f, 0.8f, 0.2f,
            2.0f + i
            });
    }
    GLuint vaoCork = makeVAO(cork);

    // Jedna ściana stożka
    float a = glm::radians(10.0f); // 36 segmentów
    std::vector<float> face = {
        0.0f,  0.5f, 0.0f,          1,0,0, 1,  // Szczyt
        0.5f, -0.5f, 0.0f,          0,1,0, 1,  // Punkt podstawy
        0.5f * cos(a), -0.5f, 0.5f * sin(a), 0,0,1, 1
    };
    GLuint vaoFace = makeVAO(face);

    // Podstawa stożka
    std::vector<float> base = {
        0, -0.5f, 0,  0.3f,0.3f,0.3f, 1  // Środek
    };
    for (int i = 0; i <= 36; ++i) {
        base.insert(base.end(), {
            0.5f * cos(i * a), -0.5f, 0.5f * sin(i * a),
            0.3f,0.3f,0.3f, 1
            });
    }
    GLuint vaoBase = makeVAO(base);

    // Zmienne sterujące
    int obj = 1;
    float rotX = 0.0f, rotY = 0.0f;

    // Pętla renderująca
    while (!glfwWindowShouldClose(win)) {
        // Obsługa klawiatury
        if (glfwGetKey(win, GLFW_KEY_ESCAPE)) break;
        if (glfwGetKey(win, GLFW_KEY_1)) obj = 1;
        if (glfwGetKey(win, GLFW_KEY_2)) obj = 2;
        if (glfwGetKey(win, GLFW_KEY_UP)) rotX -= 0.1f;
        if (glfwGetKey(win, GLFW_KEY_DOWN)) rotX += 0.1f;
        if (glfwGetKey(win, GLFW_KEY_LEFT)) rotY -= 0.1f;
        if (glfwGetKey(win, GLFW_KEY_RIGHT)) rotY += 0.1f;

        // Czyszczenie buforów
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(prog);

        // Ustawienia kamery i transformacji
        glm::mat4 proj = glm::perspective(glm::radians(45.f), 800.f / 600.f, 0.1f, 50.f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 1, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f),
            glm::radians(rotX),
            glm::vec3(1, 0, 0));
        model = glm::rotate(model,
            glm::radians(rotY),
            glm::vec3(0, 1, 0));

        glm::mat4 MVP = proj * view * model;

        if (obj == 1) {
            // Rysowanie korkociągu
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(MVP));
            glBindVertexArray(vaoCork);
            glDrawArrays(GL_POINTS, 0, cork.size() / 7);
        }
        else {
            // Rysowanie podstawy stożka
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(MVP));
            glBindVertexArray(vaoBase);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 38);

            // Rysowanie ścian bocznych
            for (int i = 0; i < 36; ++i) {
                glm::mat4 localMVP = glm::rotate(MVP, i * a, glm::vec3(0, 1, 0));
                glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(localMVP));
                glBindVertexArray(vaoFace);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}