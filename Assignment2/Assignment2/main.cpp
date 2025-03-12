#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget = false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

#define PI 3.141592
float upperArmLen = 0.4;
float lowerArmLen = 0.5;
float lowerArmOffset = -0.05;

void myInit()
{
    cube.Init();
    pyramid.Init();

    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
}

float g_time = 0;

void drawRobotArm(float ang1, float ang2, float ang3)
{
    mat4 temp = CTM;

    // BASE
    mat4 M(1.0);

    M = Translate(0, 0, 0.075) * Scale(0.3, 0.2, 0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    pyramid.Draw(program);

    M = Translate(0, 0, -0.075) * Scale(0.3, 0.2, 0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    pyramid.Draw(program);

    M = Translate(0, 0.00, -0.1) * Scale(0.05, 0.05, 0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, 0.00, 0.1) * Scale(0.05, 0.05, -0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);


    // Upper Arm
    CTM *= RotateZ(ang1);
    M = Translate(0, upperArmLen / 2.0f, 0) * Scale(0.1, upperArmLen, 0.1);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);


    // Lower Arm
    CTM *= Translate(0, upperArmLen + lowerArmOffset, 0) * RotateZ(ang2);
    M = Translate(0, lowerArmLen / 2.0f, -0.075) * Scale(0.1, lowerArmLen, 0.03);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, lowerArmLen / 2.0f, 0.075) * Scale(0.1, lowerArmLen, 0.03);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, 0, 0.1) * Scale(0.05, 0.05, 0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, 0, -0.1) * Scale(0.05, 0.05, -0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);
    
    
    // Hand
    CTM *= Translate(0, lowerArmLen, 0) * RotateZ(ang3);
    M = Scale(0.3, 0.15, 0.1);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, 0, 0.1) * Scale(0.05, 0.05, 0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);

    M = Translate(0, 0, -0.1) * Scale(0.05, 0.05, -0.05);
    glUniformMatrix4fv(uMat, 1, true, CTM * M);
    cube.Draw(program);
    
    CTM = temp;
}


float clamp(float value, float minVal, float maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}


float calculateRotationBetween(vec3 from, vec3 to)
{
    // 두 벡터를 단위 벡터로 변환
    from = normalize(from);
    to = normalize(to);

    // 내적을 사용하여 각도 계산
    float dotProduct = dot(from, to);
    dotProduct = clamp(dotProduct, -1.0f, 1.0f);

    // 각도를 acos으로 계산
    float angle = acos(dotProduct);

    // 외적을 사용하여 회전 방향 조정
    float crossProduct = from.x * to.y - from.y * to.x;
    if (crossProduct < 0)
    {
        angle = -angle;
    }

    // 라디안을 각도로 변환
    return angle * (180.0f / PI);
}

vec3 calculateEndEffectorPos(float ang1, float ang2, float ang3)
{
    float x = - upperArmLen* sin(ang1) - lowerArmLen * sin(ang1 + ang2) - 0.15 * sin(ang1 + ang2 + ang3 - PI / 2);
    float y = upperArmLen * cos(ang1) + lowerArmLen * cos(ang2 + ang1) + 0.15 * cos(ang3 + ang2 + ang1 - PI / 2);
    return vec3(x, y, 0);
}

vec3 calculateJointPos(int jointIndex, float ang1, float ang2, float ang3)
{
    if (jointIndex == 1)
    {
        return vec3(0.0f, 0.0f, 0.0f); // 첫 번째 관절 (베이스) 위치
    }
    else if (jointIndex == 2)
    {
        float x = - upperArmLen * sin(ang1);
        float y = upperArmLen * cos(ang1);
        return vec3(x, y, 0); // 두 번째 관절 위치
    }
    else if (jointIndex == 3)
    {
        float x = - upperArmLen * sin(ang1) - lowerArmLen * sin(ang2 + ang1);
        float y = upperArmLen * cos(ang1) + lowerArmLen * cos(ang2 + ang1);
        return vec3(x, y, 0); // 세 번째 관절 위치
    }
    return vec3(0.0f, 0.0f, 0.0f);
}

void computeAngle()
{
    int max_iteration = 150; // 반복 횟수 설정

    for (int r = 0; r < max_iteration; r++)
    {
        // 각 반복에서 End Effector에서 베이스 방향으로 관절의 회전을 차례대로 수행 
        for (int i = 3; i > 0; i--)
        {
            float ang1ToRad = ang1 * (PI / 180);
            float ang2ToRad = ang2 * (PI / 180);
            float ang3ToRad = ang3 * (PI / 180);

            // End Effector 위치
            vec3 endEffectorPos = calculateEndEffectorPos(ang1ToRad, ang2ToRad, ang3ToRad);

            // 현재 관절 위치 계산
            vec3 jointPos = calculateJointPos(i, ang1ToRad, ang2ToRad, ang3ToRad);

            // 목표 위치
            vec3 targetPos = target.GetPosition(g_time);

            // 현재 선택한 관절에서 End Effector까지의 벡터
            vec3 toEndEffector = endEffectorPos - jointPos;

            // 관절에서 목표 위치까지의 벡터
            vec3 toTarget = targetPos - jointPos;

            // 두 벡터 사이의 회전 계산
            float rotation = calculateRotationBetween(toEndEffector, toTarget);

            // 현재 관절의 각도 업데이트
            if (i == 3)
            {
                ang3 += rotation;
            }
            else if (i == 2)
            {
                ang2 += rotation;
            }
            else if (i == 1)
            {
                ang1 += rotation;
            }
        }
    }
}


void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    GLuint uColor = glGetUniformLocation(program, "uColor");
    glUniform4f(uColor, -1, -1, -1, -1);

    uMat = glGetUniformLocation(program, "uMat");
    CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
    drawRobotArm(ang1, ang2, ang3);

    glUniform4f(uColor, 1, 0, 0, 1);
    if (bDrawTarget == true)
        target.Draw(program, CTM, g_time);

    glutSwapBuffers();
}

void myIdle()
{
    if (bPlay)
    {
        g_time += 1 / 60.0f;
        Sleep(1 / 60.0f * 1000);

        if (bChasingTarget == false)
        {
            ang1 = 45 * sin(g_time * 3.141592);
            ang2 = 60 * sin(g_time * 2 * 3.141592);
            ang3 = 30 * sin(g_time * 3.141592);
        }
        else
            computeAngle();

        glutPostRedisplay();
    }
}

void myKeyboard(unsigned char c, int x, int y)
{
    switch (c)
    {
    case '1':
        bChasingTarget = !bChasingTarget;
        break;
    case '2':
        bDrawTarget = !bDrawTarget;
        break;
    case '3':
        target.toggleRandom();
        break;
    case ' ':
        bPlay = !bPlay;
        break;
    default:
        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Simple Robot Arm");

    glewExperimental = true;
    glewInit();

    myInit();
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutIdleFunc(myIdle);

    glutMainLoop();

    return 0;
}