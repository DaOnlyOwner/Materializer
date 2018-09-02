#if 1
#include "NodeFactory.h"
#include "BaseNode.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "Scene.h"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include "TestOperation.h"


void register_operations()
{
	NodeFactory& f = NodeFactory::Instance();
	f.Register<TestOperation>(NodeInformation{ "TestOperation",
								"Debugging purposes",
		{},
		{"out"},
		{} } , "Operation: Test");
}

GLFWwindow* init_all()
{
	if (!glfwInit())
		printf("Couldn't initialize window provider");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Materializer", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);

	// Setup style
	ImGui::StyleColorsDark();
	return window;
}

#include "CommandMenu.h"

void mainloop()
{
	GLFWwindow* window = init_all();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Scene s;
	//CommandMenu m;
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 0);

		s.Update();
		//ImGui::Begin("..");
		//std::string text;
		//if (m.Show(text)) std::cout << text.c_str() << std::endl;
		//ImGui::End();

		ImGui::PopStyleVar(5);

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
}


int main()
{
	register_operations();
	mainloop();
	return 0;
}

#endif