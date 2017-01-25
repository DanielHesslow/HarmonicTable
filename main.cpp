// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "hex_render.h"
#include "Sound.h"
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
#include <thread>
#include <chrono>
int gcd(int a, int b)
{
	int c;
	while (a != 0) {
		c = a; a = b%a;  b = c;
	}
	return b;
}

#include "notes.h"
#include "note_names.h"

static void Menu()
{
	ImGui::MenuItem("Musics", NULL, false, false);
	radio_buttons();
}



int cmp_notes(const void *va, const void *vb)
{
	int *a = (int *)va;
	int *b = (int *)vb;
	int diff_a = 0;
	int diff_b = 0;
	float size_a=0;
	float size_b=0;

	for (int i = 0; i < 6; i+=2)
	{
		diff_a += abs(a[i + 1] - a[i]);
		diff_b += abs(b[i + 1] - b[i]);
		size_a += a[i + 1];
		size_b += b[i + 1];
	}
	if (size_a - size_b != 0)
		return (size_a - size_b);
	else
		return diff_a - diff_b;

}

int main(int, char**)
{

#if 0
	//auto generation for notes.h
	#define FOR(i,j) for (int i = j+1; i < 40;i++)
	int buffer[600][6];
	int b_len = 0;
	FOR(a,1)FOR(b,a)FOR(c,a)FOR(d,c)FOR(e,c)FOR(f,e)
	{
		if (a == b || c == d || e == f)continue;
		
		if (gcd(a, b) != 1)continue;
		if (gcd(c, d) != 1)continue;
		if (gcd(e, f) != 1)continue;

		if ((float)(a) / b < 0.5) continue;
		if ((float)(c) / d < 0.5) continue;
		if ((float)(e) / f < 0.5) continue;

		if (a * d * f == b * c * e)
		{
			buffer[b_len][1] = a;
			buffer[b_len][0] = b;
			buffer[b_len][3] = c;
			buffer[b_len][2] = d;
			buffer[b_len][5] = e;
			buffer[b_len][4] = f;
			b_len++;
		}
	}

	FILE *notes = fopen("notes.h", "w");
	fprintf(notes, "// --- AUTO GENERATED, changes here will be overwritten! ---\n\n");
	qsort(buffer, b_len, sizeof(buffer[0]), cmp_notes);
	fprintf(notes, "enum Harmonics : int\n");
	fprintf(notes, "{\n");
	for (int i = 0; i < b_len; i++)
	{
		fprintf(notes, "    harmonics_%d_%d__%d_%d__%d_%d,\n", buffer[i][0], buffer[i][1], 
															   buffer[i][2], buffer[i][3],
															   buffer[i][4], buffer[i][5]);
	}
	fprintf(notes, "};\n\n");
	
	fprintf(notes, "static Harmonics harmonics;\n\n");


	fprintf(notes, "void get_harmonic_fractions(float *frac1, float *frac2)\n");
	fprintf(notes, "{\n");
	fprintf(notes, "    switch(harmonics)\n");
	fprintf(notes, "    {\n");

	for (int i = 0; i < b_len; i++)
	{
		fprintf(notes, "    case harmonics_%d_%d__%d_%d__%d_%d:\n",
															buffer[i][0], buffer[i][1],
															buffer[i][2], buffer[i][3],
															buffer[i][4], buffer[i][5]);
		fprintf(notes, "    *frac1 = %d./%d;\n", buffer[i][0], buffer[i][1]);
		fprintf(notes, "    *frac2 = %d./%d;\n", buffer[i][2], buffer[i][3]);
		

		fprintf(notes, "    return;\n");
	}
	fprintf(notes, "    }\n");
	fprintf(notes, "}\n");

	fprintf(notes, "void radio_buttons()\n");
	fprintf(notes, "{\n");
	for (int i = 0; i < b_len; i++)
	{
		fprintf(notes, "    ImGui::RadioButton(\"%d/%d %d/%d %d/%d\", (int *)&harmonics, %d);\n", 
															buffer[i][0],buffer[i][1],
															buffer[i][2],buffer[i][3],
															buffer[i][4],buffer[i][5],
															i);
		fprintf(notes, "if (ImGui::IsItemHovered()) ImGui::SetTooltip(\"%s, %s, %s\"); \n", 
															note_name(buffer[i][0], buffer[i][1]),
															note_name(buffer[i][2], buffer[i][3]),
															note_name(buffer[i][4], buffer[i][5]));
	}
	fprintf(notes, "}\n");
	fclose(notes);

#endif

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);


	create();
	init_sound();
    // Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}
		//if (menu)ImGui::OpenPopup("super_popup");

		if (ImGui::BeginPopupAlways("super_popup"))
		{
			Menu();
			ImGui::EndPopup();
		}

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		render();
		ImGui::Render();
		glfwSwapBuffers(window);
		
	}
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
	invalidate();
	glfwTerminate();
	sound_close();


    return 0;
}


