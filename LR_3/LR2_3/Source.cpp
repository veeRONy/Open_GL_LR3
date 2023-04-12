#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "Camera.h"

// размеры окна
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 700

GLuint VBO; // буфер вершин
GLuint IBO; // буфер индексов
GLuint gWVPLocation; // указатель для доступа к всемирной матрице
Vector3f Pos(0.0f, 4.0f, -5.0f);
Vector3f Target(0.0f, -0.4f, 1.0f);
Vector3f Up(0.0f, 1.0f, 0.0f);

Camera GameCamera(Pos,Target, Up);

// используем вершинный шейдер
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec4 Color;                                                                     \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    Color = vec4(clamp(Position, 0.0, 1.0), 1.0);                                   \n\
}";
// используем фрагментный шейдер
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec4 Color;                                                                      \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = Color;                                                              \n\
}";

static void SpecialKeyboardCB(int Key, int x, int y) {
    GameCamera.OnKeyboard(Key);
}

static void RenderSceneCB()
{
    // очистка буфера кадра
    glClear(GL_COLOR_BUFFER_BIT);

    static float SpeedRotate = 0.0f;
    static float SpeedX = 0.0f;
    static float SpeedY = 0.0f;
    static float Scale = 0.0f;
    // скорость вращения
    SpeedRotate += 0.2f;
    // скорость перемещения по Х
    SpeedX += 0.001f;
    // скорость перемещения по Y
    SpeedY += 0.002f;
    // размер
    Scale += 0.001f;

    Pipeline p;
    // вращение
    //p.Rotate(0.0f, SpeedRotate, 0.0f);
    // координаты
    p.WorldPos(0.0f, 0.0f, 0.0f);
    // изменяем размер
    //p.Scale(sinf(Scale), fabs(sinf(Scale)), sinf(Scale));

    //float cameraX = 0.0f;
    //cameraX += 1.0f;
   

    // позиция камеры - x,y,z
    //Vector3f CameraPos(0.2f, 4.0f, -5.0f);
    // вращение камеры
    // 1 - +вправо-влево (x), 2 - +вверх-вниз (y), 
    //3 - 1: по оси Z в полож направлении,0 - вверх, -1: в отрицательном направлении оси Z,
    //Vector3f CameraTarget(0.0f, -0.4f, 1.0f);
    // поворот камеры
    // 1- влево-вправо, 2 и 3- переворот вверх ногами
    //Vector3f CameraUp(0.0f, 1.0f, 0.0f);
   

    

    // установка камеры
    //p.SetCamera(CameraPos, CameraTarget, CameraUp);
    p.SetCamera(GameCamera.GetPos(), GameCamera.GetTarget(), GameCamera.GetUp());

    p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);


    // функция для загрузки данных в uniform-переменные шейдера
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

    glEnableVertexAttribArray(0);
    // привязываем массив вершин к VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Этот вызов говорит конвейеру как воспринимать данные внутри буфера
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // функция для отрисовки
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    // меняем фоновый буфер и буфер кадра местами
    glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
    // функция отображения на экран
    glutDisplayFunc(RenderSceneCB);
    // указываем функцию рендера в качестве ленивой
    glutIdleFunc(RenderSceneCB);
    //функция обратного вызова для получения специальных событий клавиатуры
    glutSpecialFunc(SpecialKeyboardCB);
}

static void CreateVertexBuffer()
{
    // массив точек
    Vector3f Vertices[4];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.5773f);
    Vertices[1] = Vector3f(0.0f, -1.0f, -1.15475);
    Vertices[2] = Vector3f(1.0f, -1.0f, 0.5773f);
    Vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);

    // функция для генерации объектов
    glGenBuffers(1, &VBO);
    // привязываем массив вершин к VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // передаем данные в массив вершин
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 2, 1 };

    // привязываем массив индексов к IBO
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // создаем шейдер
    GLuint ShaderObj = glCreateShader(ShaderType);
    // проверка
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
    // исходный код шейдера
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    // компиляция шейдера
    glCompileShader(ShaderObj);
    // проверка
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
    // присоединяем скомпилированный объект шейдера к объекту программы
    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    // создаем программу шейдера
    GLuint ShaderProgram = glCreateProgram();
    // проверка
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    // добавление шейдеров
    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    // проверка
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    // линковка программы шейдера
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // проверка
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    // для использования отлинкованной программы шейдеров мы назначаем её для конвейера
    glUseProgram(ShaderProgram);

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    // инициализируем GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    // задаем параметры окна
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 13");

    InitializeGlutCallbacks();

    // инициализируем GLEW
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    // цвет фона
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // сохдаем массив вершин
    CreateVertexBuffer();

    // создаем массив индексов
    CreateIndexBuffer();

    // компиляция шейдеров
    CompileShaders();

    // основной цикл
    glutMainLoop();

    return 0;
}