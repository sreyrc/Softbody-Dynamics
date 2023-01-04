#pragma once

#include "SoftBody.h"
#include "Renderer.h"

#include "imgui/implot.h"


// Handling changing values in the Renderer with Dear Imgui
class Editor 
{
public:
	Editor(GLFWwindow* window);

	void Update(SoftBody* softbody, Renderer* renderer);

private:
	float prevKs, ks;
};