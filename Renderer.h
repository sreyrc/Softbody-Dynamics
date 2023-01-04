#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <string>

#include "Camera.h"
#include "Shader.h"
#include "SoftBody.h"
#include "SphereMesh.h"


class Renderer
{
public:
	Renderer(Camera* cam, SoftBody* softBody, int SCREEN_WIDTH, int SCREEN_HEIGHT);

	void Draw();

    float m_LineWidth;

    glm::vec3 m_ModelPos;

    // Background color
    glm::vec3 m_BgColor;

    // View position
    glm::vec3 m_ViewPos;

    // Diffuse and specular colors for the model
    glm::vec3 m_DiffuseColor;
    glm::vec3 m_SpecularColor;

    // Light properties
    glm::vec3 m_LightPos;
    glm::vec3 m_LightColor;
    glm::vec3 m_LineColor;

    // Shaders: 
    // One for coloring and lighting for the model
    // Second one for drawing skeleton and grid lines
    Shader* m_ModelShader, *m_LineShader, 
        *m_SoftBodyShader, *m_SkyBoxShader;

    // A ref to the scene camera
    Camera* m_Camera;

    // Transformation matrices
    glm::mat4 m_ProjMat, m_ViewMat, m_ModelMat;
    glm::mat4 m_RotMat;

    // The softbody
    SoftBody* m_Softbody;    

    // Mesh for sphere
    SphereMesh* m_SphereMesh;

    unsigned int m_CubeMapTexID;
    unsigned int m_SkyBoxVAO;
    unsigned int m_SkyBoxVBO;

    bool m_DrawSprings = true;
    bool m_DrawSkin = false;
    bool m_DrawMassPoints = true;

private:
    void SetupMatrices(glm::vec3 pos);
    void SetModelShaderVars();
    void SetupLineShaderVars();
    void DrawGrid();

};
