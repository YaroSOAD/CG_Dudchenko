//\internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Texture.h"
#define PI 3.14159265 
//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>


static const GLsizei WIDTH = 1080, HEIGHT = 720; //размеры окна
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;
static int g_shaderProgram = 0;


GLfloat deltaTime = 0.0f;  // что это? что-то дефолтное
GLfloat lastFrame = 0.0f;

Camera camera(float3(0.0f, 10.0f, 30.0f));  // исходные положение камеры

//Функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE: //на Esc выходим из программы
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
        if (action == GLFW_PRESS)
        {
            if (filling == 0)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                filling = 1;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                filling = 0;
            }
        }
        break;
    case GLFW_KEY_1:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case GLFW_KEY_2:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    default:
        if (action == GLFW_PRESS)
            keys[key] = true;   // это чё?
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        g_captureMouse = !g_captureMouse;


    if (g_captureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_capturedMouseJustNow = true;
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - lastX;
    GLfloat yoffset = lastY - float(ypos);

    lastX = float(xpos);
    lastY = float(ypos);

    if (g_captureMouse)
        camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera& camera, GLfloat deltaTime)
{
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

GLsizei CreatePlane(GLuint& vao)  // opengl презентация
{
    std::vector<float> vertices = { 20.5f, 0.0f, 20.5f, 1.0f,  // зачем ? 1
                                 20.5f, 0.0f, -20.5f, 1.0f,
                                -20.5f, 0.0f, 20.5f, 1.0f,
                                -20.5f, 0.0f, -20.5f, 1.0f };

    std::vector<float> normals = { 0.0f, 0.0f, -1.0f, 1.0f,
                                   0.0f, 0.0f, -1.0f, 1.0f,
                                   0.0f, 0.0f, -1.0f, 1.0f,
                                   0.0f, 0.0f, -1.0f, 1.0f };

    std::vector<uint32_t> indices = { 0u, 1u, 2u,
                                      1u, 2u, 3u };  // что за иднексы это группа

    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}

GLsizei CreateCube(GLuint& vao)
{

    std::vector<GLfloat> vertices_vec = { -1.0f, -1.0f, -1.0f,
                                         -1.0f, -1.0f, +1.0f,
                                         +1.0f, -1.0f, +1.0f,
                                         +1.0f, -1.0f, -1.0f,

                                         -1.0f, +1.0f, -1.0f,
                                         -1.0f, +1.0f, +1.0f,
                                         +1.0f, +1.0f, +1.0f,
                                         +1.0f, +1.0f, -1.0f,

                                         -1.0f, -1.0f, -1.0f,
                                         -1.0f, +1.0f, -1.0f,
                                         +1.0f, +1.0f, -1.0f,
                                         +1.0f, -1.0f, -1.0f,

                                         -1.0f, -1.0f, +1.0f,
                                         -1.0f, +1.0f, +1.0f,
                                         +1.0f, +1.0f, +1.0f,
                                         +1.0f, -1.0f, +1.0f,

                                         -1.0f, -1.0f, -1.0f,
                                         -1.0f, -1.0f, +1.0f,
                                         -1.0f, +1.0f, +1.0f,
                                         -1.0f, +1.0f, -1.0f,

                                         +1.0f, -1.0f, -1.0f,
                                         +1.0f, -1.0f, +1.0f,
                                         +1.0f, +1.0f, +1.0f,
                                         +1.0f, +1.0f, -1.0f };

    std::vector<GLfloat> normals_vec = { 0.0f, -1.0f, 0.0f,
                                            0.0f, -1.0f, 0.0f,
                                            0.0f, -1.0f, 0.0f,
                                            0.0f, -1.0f, 0.0f,

                                            0.0f, +1.0f, 0.0f,
                                            0.0f, +1.0f, 0.0f,
                                            0.0f, +1.0f, 0.0f,
                                            0.0f, +1.0f, 0.0f,

                                            0.0f, 0.0f, -1.0f,
                                            0.0f, 0.0f, -1.0f,
                                            0.0f, 0.0f, -1.0f,
                                            0.0f, 0.0f, -1.0f,

                                            0.0f, 0.0f, +1.0f,
                                            0.0f, 0.0f, +1.0f,
                                            0.0f, 0.0f, +1.0f,
                                            0.0f, 0.0f, +1.0f,

                                            -1.0f, 0.0f, 0.0f,
                                            -1.0f, 0.0f, 0.0f,
                                            -1.0f, 0.0f, 0.0f,
                                            -1.0f, 0.0f, 0.0f,

                                            +1.0f, 0.0f, 0.0f,
                                            +1.0f, 0.0f, 0.0f,
                                            +1.0f, 0.0f, 0.0f,
                                            +1.0f, 0.0f, 0.0f };

    std::vector<GLfloat> texcoords_vec = { 0.0f, 0.0f,
                                             0.0f, 1.0f,
                                             1.0f, 1.0f,
                                             1.0f, 0.0f,

                                             1.0f, 0.0f,
                                             1.0f, 1.0f,
                                             0.0f, 1.0f,
                                             0.0f, 0.0f,

                                             0.0f, 0.0f,
                                             0.0f, 1.0f,
                                             1.0f, 1.0f,
                                             1.0f, 0.0f,

                                             0.0f, 0.0f,
                                             0.0f, 1.0f,
                                             1.0f, 1.0f,
                                             1.0f, 0.0f,

                                             0.0f, 0.0f,
                                             0.0f, 1.0f,
                                             1.0f, 1.0f,
                                             1.0f, 0.0f,

                                             0.0f, 0.0f,
                                             0.0f, 1.0f,
                                             1.0f, 1.0f,
                                             1.0f, 0.0f, };

    std::vector<GLuint> indices_vec = { 0, 2, 1,
                                           0, 3, 2,

                                           4, 5, 6,
                                           4, 6, 7,

                                            8, 9, 10,
                                            8, 10, 11,

                                           12, 15, 14,
                                           12, 14, 13,

                                           16, 17, 18,
                                           16, 18, 19,

                                           20, 23, 22,
                                           20, 22, 21 };


    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboIndices);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboTexCoords);

    glBindVertexArray(vao); GL_CHECK_ERRORS;
    {
        //передаем в шейдерную программу атрибут координат вершин
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GLfloat), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

        //передаем в шейдерную программу атрибут нормалей
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GLfloat), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

        //передаем в шейдерную программу атрибут текстурных координат
        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GLfloat), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (nullptr)); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

        //передаем в шейдерную программу индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return indices_vec.size();
}

GLsizei CreateSphere(float radius, int numberSlices, GLuint& vao)
{
    int i, j;

    int numberParallels = numberSlices;
    int numberVertices = (numberParallels + 1) * (numberSlices + 1); // вершины задаём
    int numberIndices = numberParallels * numberSlices * 3;

    float angleStep = (2.0f * PI) / ((float)numberSlices);

    std::vector<float> pos(numberVertices * 4, 0.0f);
    std::vector<float> norm(numberVertices * 4, 0.0f);
    std::vector<float> texcoords(numberVertices * 2, 0.0f);

    std::vector<int> indices(numberIndices, -1);

    for (i = 0; i < numberParallels + 1; i++)
    {
        for (j = 0; j < numberSlices + 1; j++)
        {
            int vertexIndex = (i * (numberSlices + 1) + j) * 4;
            int normalIndex = (i * (numberSlices + 1) + j) * 4;
            int texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

            pos.at(vertexIndex + 0) = radius * sinf(angleStep * (float)i) * sinf(angleStep * (float)j);
            pos.at(vertexIndex + 1) = radius * cosf(angleStep * (float)i);
            pos.at(vertexIndex + 2) = radius * sinf(angleStep * (float)i) * cosf(angleStep * (float)j);
            pos.at(vertexIndex + 3) = 1.0f;

            norm.at(normalIndex + 0) = pos.at(vertexIndex + 0) / radius;
            norm.at(normalIndex + 1) = pos.at(vertexIndex + 1) / radius;
            norm.at(normalIndex + 2) = pos.at(vertexIndex + 2) / radius;
            norm.at(normalIndex + 3) = 1.0f;

            texcoords.at(texCoordsIndex + 0) = (float)j / (float)numberSlices;
            texcoords.at(texCoordsIndex + 1) = (1.0f - (float)i) / (float)(numberParallels - 1);
        }
    }

    int* indexBuf = &indices[0];

    for (i = 0; i < numberParallels; i++)
    {
        for (j = 0; j < numberSlices; j++)
        {
            *indexBuf++ = i * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);

            *indexBuf++ = i * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);
            *indexBuf++ = i * (numberSlices + 1) + (j + 1);

            int diff = int(indexBuf - &indices[0]);
            if (diff >= numberIndices)
                break;
        }
        int     diff = int(indexBuf - &indices[0]);
        if (diff >= numberIndices)
            break;
    }

    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}

GLsizei CreateCuboid(float lenght , float width, float height, GLuint& vao) 
{
    std::vector <float> vertex = { 0.0f,0.0f,0.0f,1.0f,  0.0f,lenght,0.0f, 1.0f,  width,lenght,0.0f,1.0f,   width,0.0f,0.0f,1.0f,
                                   width,0.0f,height,1.0f,  width,lenght,height,1.0f,  0.0f,lenght,height,1.0f,  0.0f,0.0f,height,1.0f };
    std::vector <int> indices = { 0,1,2, 0,2,3, 2,3,4, 2,4,5, 4,5,6, 4,6,7, 7,0,4, 0,4,3, 6,7,1, 0,1,7, 1,2,6, 5,2,6 };

    std::vector <float> normal = { 0.0f,0.0f,-1.0f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f,
                                  0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f,
                                  -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f };



    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}

GLsizei CreateConus(float radius, float height, int numberSlices, GLuint& vao)
{
    std::vector <float> vertex;//вершины
    std::vector <int> faces;//треугольники по индексам
    std::vector <float> normal;//нормали

    float xpos = 0.0f;
    float ypos = 0.0f;

    float angle = PI * 2.f / float(numberSlices); //делим окружность на секции

    //центр дна
    vertex.push_back(xpos); vertex.push_back(0.0f);
    vertex.push_back(ypos);
    vertex.push_back(1.0f); //w

    //расчёт всех точек дна
    for (int i = 1; i <= numberSlices; i++)
    {
        float newX = radius * sinf(angle * i);
        float newY = -radius * cosf(angle * i);

        //для дна
        vertex.push_back(newX); vertex.push_back(0.0f);
        vertex.push_back(newY);

        vertex.push_back(1.0f); //w
    }

    //координата вершины конуса
    vertex.push_back(xpos); vertex.push_back(height);
    vertex.push_back(ypos);

    vertex.push_back(1.0f); //w

    //ИТОГО: вершины: центр основания, точка основания 1, точка основания 2,
    // и т.д., точка-вершина (четыре координаты)

    //расчёт поверхности дна + нормали
    for (int i = 1; i <= numberSlices; i++)
    {
        faces.push_back(0); //центр основания
        faces.push_back(i); //текущая точка

        if (i != numberSlices) //если не крайняя точка основания
        {
            faces.push_back(i + 1);//то соединяем со следующей по индексу
        }
        else
        {
            faces.push_back(1);//замыкаем с 1ой
        }

        //нормали у дна смотрят вниз
        normal.push_back(0.0f);
        normal.push_back(0.0f);
        normal.push_back(-1.0f);
    }
    //боковые поверхности + нормали
    for (int i = 1; i <= numberSlices; i++)
    {
        int k = 0;//нужно для нормалей

        faces.push_back(i);//текущая

        if (i != numberSlices) //если не крайняя точка основания
        {
            faces.push_back(i + 1);//то соединяем со следующей по индексу
            k = i + 1;
        }
        else
        {
            faces.push_back(1);//замыкаем с 1ой
            k = 1;
        }

        faces.push_back(numberSlices + 1);//вершина

        //расчет нормали к боковой
        float3 a, b, normalVec;
        //вектор а = координаты текущей - координаты вершины
        a.x = vertex[i * 4 - 3] - vertex[vertex.size() - 1 - 3];
        a.y = vertex[i * 4 - 2] - vertex[vertex.size() - 1 - 2];;
        a.z = vertex[i * 4 - 1] - vertex[vertex.size() - 1 - 1];;

        //вектор б = координаты седующей текущей (или 1 при последней итерации)
        // - координаты вершины)
        b.x = vertex[k * 4 - 3] - vertex[vertex.size() - 1 - 3];
        b.x = vertex[k * 4 - 2] - vertex[vertex.size() - 1 - 2];
        b.x = vertex[k * 4 - 1] - vertex[vertex.size() - 1 - 1];

        //нормаль как векторное произведение
        normalVec = cross(a, b);

        //запись нормаль в вектор
        normal.push_back(normalVec.x);
        normal.push_back(normalVec.y);
        normal.push_back(normalVec.z);
    }


    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return faces.size();
}

GLsizei CreateCylinder(float radius, float height, int numberSlices, GLuint& vao)
{

    float angle = 360 / numberSlices; //в градусах


    std::vector<float> position =
    {
       0.0f, 0.0f, 0.0f, 1.0f, //низ
        0.0f, 0.0f + height , 0.0f  , 1.0f, //верх
    };


    std::vector<float> normals =
    { 0.0f, 0.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f };

    std::vector<uint32_t> indices;

    for (int i = 2; i < numberSlices + 2; i++) {
        // нижняя точка
        position.push_back(radius * cos(angle * i * PI / 180));  // x
        position.push_back(0.0f);                              // y
        position.push_back(radius * sin(angle * i * PI / 180)); // z
        position.push_back(1.0f);

        // верхняя точка
        position.push_back(radius * cos(angle * i * PI / 180));  // x
        position.push_back(height);                    // y
        position.push_back(radius * sin(angle * i * PI / 180)); // z
        position.push_back(1.0f);

        normals.push_back(position.at(i + 0) / numberSlices * 8.2);
        normals.push_back(position.at(i + 1) / numberSlices * 8.2);
        normals.push_back(position.at(i + 2) / numberSlices * 8.2);
        normals.push_back(1.0f);
        normals.push_back(position.at(i + 0) / numberSlices * 8.2);
        normals.push_back(position.at(i + 1) / numberSlices * 8.2);
        normals.push_back(position.at(i + 2) / numberSlices * 8.2);
        normals.push_back(1.0f);


    }

    for (int i = 2; i < numberSlices * 2 + 2; i = i + 2) {
        // нижнее основание
        indices.push_back(0);
        indices.push_back(i);
        if ((i) == numberSlices * 2)
        {
            indices.push_back(2);
        }
        else
        {
            indices.push_back(i + 2);
        }
        // верхнее основание
        indices.push_back(1);
        indices.push_back(i + 1);
        if ((i) == numberSlices * 2)
        {
            indices.push_back(3);
        }
        else
        {
            indices.push_back(i + 3);
        }

        indices.push_back(i);
        indices.push_back(i + 1);
        if ((i) == numberSlices * 2)
        {
            indices.push_back(2);
        }
        else
        {
            indices.push_back(i + 2);
        }

        indices.push_back(i + 1);
        if ((i) == numberSlices * 2)
        {
            indices.push_back(2);
            indices.push_back(3);
        }
        else
        {
            indices.push_back(i + 2);
            indices.push_back(i + 3);
        }

    }

    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(GLfloat), position.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}

int initGL()
{
    int res = 0;

    //грузим функции opengl через glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    //выводим в консоль некоторую информацию о драйвере и контексте opengl
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: " << std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  " << std::endl;
    std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;

    //запрашиваем контекст opengl версии 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }




    glfwMakeContextCurrent(window);

    //регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
    glfwSetKeyCallback(window, OnKeyboardPressed);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback(window, OnMouseScroll);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (initGL() != 0)
        return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    //создание шейдерной программы из двух файлов с исходниками шейдеров
    //используется класс-обертка ShaderProgram
    std::unordered_map<GLenum, std::string> shaders;
    shaders[GL_VERTEX_SHADER] = "../shaders/vertex.glsl";
    shaders[GL_FRAGMENT_SHADER] = "../shaders/fragment.glsl";
    ShaderProgram fragmentShader(shaders); GL_CHECK_ERRORS;

    //Текстуры
    Texture2D skyboxTexture("../textures/skybox1.jpg");
    Texture2D ballTexture("../textures/hypno.jpg");
    Texture2D ballTexture2("../textures/ball.jpg");
    
    ////////////////////////////
    GLuint vaoSkybox;
    GLuint vaoSphere;
    GLuint vaoSphere2;
 

  

    ////////////////////////////
    GLsizei skybox = CreateCube(vaoSkybox);
    GLsizei sphereIndices = CreateSphere(1.0f, 50, vaoSphere);
    GLsizei sphereIndices2 = CreateSphere(1.0f, 50, vaoSphere2);

    ////////////////////////////////////////////////////////////////////////////////


    glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
    glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float timer = 0.0f;

    //цикл обработки сообщений и отрисовки сцены каждый кадр
    while (!glfwWindowShouldClose(window))
    {
        //считаем сколько времени прошло за кадр
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        doCameraMovement(camera, deltaTime);

        //очищаем экран каждый кадр
        glClearColor(0.9f, 0.95f, 0.97f, 1.0f); GL_CHECK_ERRORS;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

        fragmentShader.StartUseShader(); GL_CHECK_ERRORS;

        //обновляем матрицы камеры и проекции каждый кадр
        float4x4 view = camera.GetViewMatrix();
        float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);

        //модельная матрица, определяющая положение объекта в мировом пространстве
        //начинаем с единичной матрицы
        float4x4 model;

        //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
        fragmentShader.SetUniform("view", view);       GL_CHECK_ERRORS;
        fragmentShader.SetUniform("projection", projection); GL_CHECK_ERRORS;
        fragmentShader.SetUniform("model", model);
        fragmentShader.SetUniform("type", 1);
        fragmentShader.SetUniform("camPos", camera.pos);

        // Направленное освещение
        fragmentShader.SetUniform("directedLights[0].direction", make_float3(0.0f, -1.0f, 0.0f));
        fragmentShader.SetUniform("directedLights[0].color", make_float3(0.8f, 0.8f, 0.8f));
        fragmentShader.SetUniform("directedLights[0].ambient", make_float3(0.6f, 0.6f, 0.6f));
        fragmentShader.SetUniform("directedLights[0].specular", make_float3(1.0f, 1.0f, 1.0f));

        //Источник света
       
           fragmentShader.SetUniform("pointLights[0].position", make_float3(0.0f, 0.0f, 0.0f));
           fragmentShader.SetUniform("pointLights[0].color", make_float3(1.0f, 1.0f, 1.0f));
           fragmentShader.SetUniform("pointLights[0].ambient", make_float3(0.6f, 0.6f, 0.6f));
           fragmentShader.SetUniform("pointLights[0].specular", make_float3(1.0f, 1.0f, 1.0f));
           fragmentShader.SetUniform("pointLights[0].mult", 5.0f);
           fragmentShader.SetUniform("directedLights[0].mult", 0.5f);

        fragmentShader.SetUniform("material.shininess", 1.0f);

        fragmentShader.SetUniform("typeOfTexture", false);

        //----------------------------------------
        //СКАЙБОКС

        glBindVertexArray(vaoSkybox); GL_CHECK_ERRORS;
        {

            bindTexture(fragmentShader, 0, "material.texture_diffuse", skyboxTexture);
            model = scale4x4(float3(100.0f, 100.0f, 100.0f));
            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLES, skybox, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;

     

       
            glBindVertexArray(vaoSphere);
            {

                for (unsigned i = 0; i < 120; i++) {
                    if (i <= 10) {
                        bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                       

                        if (i <= 20 && i > 10) 
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 30 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
                        
                            if (i <= 30 && i > 20)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 60 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 40 && i > 30)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 90 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 50 && i > 40)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 120 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 60 && i > 50)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD +150 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 70 && i > 60)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 180 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 80 && i > 70)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 210 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 90 && i > 80)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 240 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 100 && i > 90)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 270 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 110 && i > 100)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 300 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                            if (i <= 120 && i > 110)
                                bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture);
                            model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 330 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                            fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                            glDrawElements(GL_TRIANGLE_STRIP, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;


                           fragmentShader.SetUniform("pointLights[0].position", make_float3(0.0f, i+0.0f, 0.0f));
                            fragmentShader.SetUniform("pointLights[0].color", make_float3(1.0f, 1.0f, 1.0f));
                            fragmentShader.SetUniform("pointLights[0].ambient", make_float3(0.6f, 0.6f, 0.6f));
                            fragmentShader.SetUniform("pointLights[0].specular", make_float3(1.0f, 1.0f, 1.0f));
                            fragmentShader.SetUniform("pointLights[0].mult", 5.0f);

                    }
                }

            }

            glBindVertexArray(vaoSphere2);
            {

                for (unsigned i = 0; i < 120; i++) {
                    if (i <= 10) {
                        bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 15 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;



                        if (i <= 20 && i > 10)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 45 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 30 && i > 20)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 75 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 40 && i > 30)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 105 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 50 && i > 40)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 135 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 60 && i > 50)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 165 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 70 && i > 60)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 195 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 80 && i > 70)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 225 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 90 && i > 80)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 255 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 100 && i > 90)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 285 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 110 && i > 100)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 315 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

                        if (i <= 120 && i > 110)
                            bindTexture(fragmentShader, 0, "material.texture_diffuse", ballTexture2);
                        model = transpose(mul(rotate_Z_4x4(timer * 30 * DEG_TO_RAD + 345 * DEG_TO_RAD), (mul(translate4x4(float3(0.0f, i + 0.0f, 0.0f)), scale4x4(float3(1.0f, 1.0f, 1.0f))))));
                        fragmentShader.SetUniform("model", model); GL_CHECK_ERRORS;
                        glDrawElements(GL_TRIANGLE_STRIP, sphereIndices2, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
   
                    }
                }

            }
        timer += 0.01f;

        glBindVertexArray(0); GL_CHECK_ERRORS;

        fragmentShader.StopUseShader(); GL_CHECK_ERRORS;
        glfwSwapBuffers(window);
    }



    glfwTerminate();
    return 0;
}
