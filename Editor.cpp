#include "Editor.h"

#include <glm/glm.hpp>

Editor::Editor(GLFWwindow* window) {

    // Dear Imgui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void Editor::Update(SoftBody* softBody, Renderer* renderer) {

    // Imgui new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Camera controls instructions
    ImGui::Begin("Camera Instructions"); {
        ImGui::Text("Use WASD to move around");
        ImGui::Text("Use the Arrow Keys to rotate the camera");
        ImGui::Text("Press I to zoom in and O to zoom out");
        ImGui::Text("Press R to reset to camera defaults");
    }
    ImGui::End();


    // Camera controls instructions
    ImGui::Begin("Anchor points"); {
        ImGui::SliderFloat3("Anchor Point 1", &softBody->GetAnchorPoints().first->m_Position.x, -5.0f, 5.0f);
        ImGui::SliderFloat3("Anchor Point 2", &softBody->GetAnchorPoints().second->m_Position.x, -5.0f, 5.0f);
        ImGui::SliderFloat3("Light Position", &renderer->m_LightPos.x, -5.0f, 5.0f);
        ImGui::SliderFloat("Spring Constant", &softBody->m_Ks, 10.0f, 200.0f);
        ImGui::Checkbox("Draw Springs", &renderer->m_DrawSprings);
        ImGui::Checkbox("Draw Mass Points", &renderer->m_DrawMassPoints);
        ImGui::Checkbox("Draw Skin", &renderer->m_DrawSkin);
    }

    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}