#include"Camera.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

const static float StepSize = 0.1f;

bool Camera::OnKeyboard(int Key)
{
    bool Ret = false;
    switch (Key) {
    case GLUT_KEY_UP: {
        m_pos += (m_target * StepSize);
        Ret = true;
    } break;
    case GLUT_KEY_DOWN: {
        m_pos -= (m_target * StepSize);
        Ret = true;
    } break;
    case GLUT_KEY_LEFT: {
        Vector3f Left = m_target.Cross(m_up);
        Left.Normalize();
        Left *= StepSize;
        m_pos += Left;
        Ret = true;
    } break;
    case GLUT_KEY_RIGHT: {
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right *= StepSize;
        m_pos += Right;
        Ret = true;
    } break;
    case GLUT_KEY_END:
    {
        m_pos.y -= StepSize;
        Ret = true;
    } break;
    case GLUT_KEY_HOME:
    {
        m_pos.y += StepSize;
        Ret = true;
    } break;

    }
    return Ret;
}

Camera::Camera()
{
    m_pos = Vector3f(0.0f, 0.0f, 0.0f);
    m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_up = Vector3f(0.0f, 1.0f, 0.0f);
}

Camera::Camera(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_pos = Pos;
    m_target = Target;
    m_target.Normalize();
    m_up = Up;
    m_up.Normalize();
}