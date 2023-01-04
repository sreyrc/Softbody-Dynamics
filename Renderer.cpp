#include <glad/glad.h>
#include <stb/stb_image.h>

#include "Renderer.h"

unsigned int LoadCubemap(std::vector<std::string> faces);

Renderer::Renderer(Camera* cam, SoftBody* softBody, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    m_Softbody = softBody;

    // Setting initial params for scene and lighting
    m_BgColor = glm::vec3(0.0f, 0.0f, 0.0f);
    m_ViewPos = glm::vec3(0.0f, 0.1f, 0.6f);
    m_DiffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_LightPos = glm::vec3(0.0f, 2.5f, 1.5f);
    m_LightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    m_LineWidth = 1.0f;
    m_LineColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // Loading shaders: one for the model and the other for line drawing
    m_ModelShader = new Shader("ModelShader.vert", "ModelShader.frag");
    m_LineShader = new Shader("LineShader.vert", "LineShader.frag"); 
    m_SoftBodyShader = new Shader("SoftBody.vert", "SoftBody.frag");
    m_SkyBoxShader = new Shader("Skybox.vert", "Skybox.frag");

    m_SkyBoxShader->Use();
    m_SkyBoxShader->SetInt("skybox", 0);

    // Set up camera
    m_Camera = cam;

    // Initialize matrices
    m_ModelMat = glm::mat4(1.0f);
    m_ViewMat = glm::mat4(1.0f);

    m_ModelPos = glm::vec3(0.0f);

    m_RotMat = glm::mat4(1.0f);

    m_SphereMesh = new SphereMesh();

    m_ProjMat = glm::perspective(glm::radians(m_Camera->m_Zoom),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

    m_SkyBoxVAO = 0; m_SkyBoxVBO = 0;

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // first, configure the cube's VAO (and VBO)
    glGenVertexArrays(1, &m_SkyBoxVAO);
    glGenBuffers(1, &m_SkyBoxVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindVertexArray(m_SkyBoxVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<std::string> faces
    {
        "../skybox/right.jpg",
        "../skybox/left.jpg",
        "../skybox/top.jpg",
        "../skybox/bottom.jpg",
        "../skybox/front.jpg",
        "../skybox/back.jpg"
    };
    m_CubeMapTexID = LoadCubemap(faces);
}


// Draw skeleton and model if opted for
void Renderer::Draw()
{
    glEnable(GL_DEPTH_TEST);
 
    // Set BG color and clear buffers
    glClearColor(m_BgColor.x, m_BgColor.y, m_BgColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    m_SkyBoxShader->Use();


    m_ViewMat = m_Camera->GetViewMatrix();

    glm::mat4 view = glm::mat4(glm::mat3(m_ViewMat));
    m_SkyBoxShader->SetMat4("view", view);
    m_SkyBoxShader->SetMat4("proj", m_ProjMat);
    // skybox cube
    glBindVertexArray(m_SkyBoxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    if (m_DrawSkin) {
        m_SoftBodyShader->Use();
        m_SoftBodyShader->SetMat4("view", m_ViewMat);
        m_SoftBodyShader->SetMat4("proj", m_ProjMat);
        m_Softbody->UpdateVertexDataAndDraw();
    }

    m_ModelShader->Use();
    SetModelShaderVars();

    if (m_DrawMassPoints) {
        auto& massPoints = m_Softbody->GetMassPoints();

        // Draw mass points

        for (auto& [key, val] : massPoints) {
            SetupMatrices(val->GetPosition());

            m_SphereMesh->BindVAO();
            glDrawElements(GL_TRIANGLE_STRIP,
                m_SphereMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        }
    }

    // Draw anchor points

    auto& anchorPoints = m_Softbody->GetAnchorPoints();

    SetupMatrices(anchorPoints.first->GetPosition());
    m_SphereMesh->BindVAO();
    glDrawElements(GL_TRIANGLE_STRIP,
        m_SphereMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);

    SetupMatrices(anchorPoints.second->GetPosition());
    m_SphereMesh->BindVAO();
    glDrawElements(GL_TRIANGLE_STRIP,
        m_SphereMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);


    glLineWidth(2.0f);

    // Draw floor
    SetupLineShaderVars();

    glEnable(GL_DEPTH_TEST);

    // Display spring connections
    auto& springList = m_Softbody->GetSprings();

    if (m_DrawSprings) {

        // Draw springs between vertices of the softbody

        m_LineShader->Use();
        m_LineShader->SetVec3("lineColor", 1.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);

        // Display spring connections
        auto& springList = m_Softbody->GetSprings();

        for (int i = 0; i < springList.size() - 2; i++) {
            auto spring = springList[i];

            auto massPointA = springList[i]->a->GetPosition();
            glVertex3f(massPointA.x, massPointA.y, massPointA.z);

            auto massPointB = springList[i]->b->GetPosition();
            glVertex3f(massPointB.x, massPointB.y, massPointB.z);
        };
        glEnd();

        glDisable(GL_LINE_SMOOTH);
    }

    // Spings from softbody corners to anchor points
    glBegin(GL_LINES);
        auto massPointA = springList[springList.size() - 2]->a->GetPosition();
        glVertex3f(massPointA.x, massPointA.y, massPointA.z);
        auto massPointB = springList[springList.size() - 2]->b->GetPosition();
        glVertex3f(massPointB.x, massPointB.y, massPointB.z);

        massPointA = springList[springList.size() - 1]->a->GetPosition();
        glVertex3f(massPointA.x, massPointA.y, massPointA.z);
        massPointB = springList[springList.size() - 1]->b->GetPosition();
        glVertex3f(massPointB.x, massPointB.y, massPointB.z);
    glEnd();

}


// Setup model, view, projection and final bone transform matrices
void Renderer::SetupMatrices(glm::vec3 pos) {
    
    glm::mat4 translate(1.0f);
    translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
    m_ModelMat = translate * scale;

    // Set matrices
    m_ModelShader->SetMat4("proj", m_ProjMat);
    m_ModelShader->SetMat4("view", m_ViewMat);
    m_ModelShader->SetMat4("model", m_ModelMat);

}

// Set variables in the shader for the model
void Renderer::SetModelShaderVars() {

    // Set Material properties
    m_ModelShader->SetVec3("material.diffuse", m_DiffuseColor);
    m_ModelShader->SetVec3("material.specular", m_SpecularColor);

    // Set light position and color (for now, just one light source)
    m_ModelShader->SetVec3("lights[0].position", m_LightPos);
    m_ModelShader->SetVec3("lights[0].color", m_LightColor);

    // Set other vars
    m_ModelShader->SetInt("numberOfLights", 1);
    m_ModelShader->SetVec3("viewPos", m_ViewPos);
}


// Pass variables to the line shader
void Renderer::SetupLineShaderVars() {
    // Set variables for line drawing shader
    m_LineShader->Use();
    m_LineShader->SetMat4("view", m_ViewMat);
    m_LineShader->SetMat4("model", 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));
    m_LineShader->SetMat4("proj", m_ProjMat);
    m_LineShader->SetVec3("lineColor", m_LineColor);
}

// Draw the grid lines
void Renderer::DrawGrid() {

    glEnable(GL_DEPTH_TEST);
    glBegin(GL_LINES);

    for (int i = -5; i <= 5; i++) {
        glVertex3f(i, 0, -5);
        glVertex3f(i, 0, 5);
        glVertex3f(-5, 0, i);
        glVertex3f(5, 0, i);
    };
    glEnd();

    glDisable(GL_LINE_SMOOTH);
}

unsigned int LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}