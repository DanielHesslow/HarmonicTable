// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

// GL3W/GLFW
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif
#include "math.h"
// Data
//static GLFWwindow*  g_Window = NULL;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
//static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
//static int          g_AttribLocationTex = 0; , g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0;//, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_tris_handle = 0;
static int g_unif_time;
static int g_unif_res;
static int g_unif_translation;
static int g_unif_scale;
static int g_unif_mouse_clicks;
static int g_unif_notes;
static int g_unif_entities;
static int g_unif_tile_states;
static int g_unif_harmonics;

#pragma pack(push, 1)
struct vec2
{
	float x, y;

	vec2 operator *(vec2 o)  { return{ x * o.x, y * o.y }; }
	vec2 operator +(vec2 o)  { return{ x + o.x, y + o.y }; }
	vec2 operator -(vec2 o)  { return{ x - o.x, y - o.y }; }
	vec2 operator /(vec2 o)  { return{ x / o.x, y / o.y }; }
	bool operator ==(vec2 o) { return x == o.x && y == o.y; }

	vec2 operator *(float f) { return{ x * f, y * f }; }
	vec2 operator +(float f) { return{ x + f, y + f };}
	vec2 operator -(float f) { return{ x - f, y - f }; }
	vec2 operator /(float f) { return{ x / f, y / f }; }

};

struct Vert
{
	vec2 pos;
};

struct SMouseClick
{
	vec2 pos;
	float time;
};

struct STileState
{
	vec2 pos;
	float type;
};

struct SEntity
{
	vec2 pos;
	vec2 dir;
};

struct SNote
{
	vec2 pos;
	float time;
	float freq;
};


#pragma pack(pop)

float dot(vec2 a, vec2 b) { return a.x *b.x + a.y *b.y; };
float length(vec2 a) { return sqrt(dot(a, a)); };


void check_errors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		int q = err;
	}
}

struct hex_point
{
	vec2 coords;
	float dist_border;
	float dist_center;
};


float step(float edge, float f)
{
	return (f < edge) ? 0.0 : 1.0;
}
vec2 step(vec2 edge, vec2 v)
{
	return{step(edge.x,v.x),step(edge.y,v.y)};
}


vec2 floor(vec2 v)
{
	return{ (float)floor(v.x), (float)floor(v.y)};
}

float fract(float f)
{
	return f-floor(f);
}
vec2  fract(vec2 v) { return { fract(v.x),fract(v.y) }; }


float actual_mod(float f, float mod)
{
	if (f >= 0.0)return fmod(f,mod);
	else return mod - fmod(-f,mod);
}


hex_point hexagon(vec2 p)
{
	vec2 q = {p.x*2.0*0.5773503,p.y + p.x*0.5773503};

	vec2 pi = floor(q);
	vec2 pf = fract(q);

	float v = actual_mod(pi.x + pi.y, 3.0);
	float ca = step(1.0, v);
	float cb = step(2.0, v);
	vec2 _pf = { pf.y,pf.x };
	vec2 ma = step(pf, _pf);

	// distance to borders
	vec2 unit = { 1.0,1.0 };
	float e = dot(ma,unit - _pf + ca*(pf.x + pf.y - 1.0) + (_pf - pf*2.0)*cb);

	// distance to center	
	p = { q.x + (float)floor(0.5f + p.y / 1.5f), 4.0f*p.y / 3.0f };
	p = p*0.5 + 0.5;

	vec2 scale = { 1.0,0.85 };
	float f = length((fract(p) - 0.5)*scale);
	hex_point ret = {};
	ret.coords = (pi + ca - ma*cb);
	ret.dist_border = e;
	ret.dist_center= f;
	return ret;
}



vec2 to_gl(vec2 p, vec2 res, vec2 scale, vec2 translation)
{
	p = p / res;
	p = p * 2.;
	p = p - 1.;
	p.y = -p.y;
	p = p * scale;
	p = p + translation;
	p = p*res / 1000.;
	return p;
}


#include "logic.h"

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void render()
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	glUseProgram(g_ShaderHandle);

	vec2 res = vec2{ (float)fb_width,(float)fb_height };
	vec2 scale = vec2{ 10.f,10.f };
	float t = glfwGetTime();
	vec2 translation = vec2{ 30.0f,30.0f };

#define NUM_MOUSE_CLICKS 10
#define NUM_TILE_STATES  10
#define NUM_NOTES		 10
#define NUM_ENTITIES	 10

	static SMouseClick mc[NUM_MOUSE_CLICKS] = {};
	static STileState ts[NUM_TILE_STATES] = {};
	static SEntity e[NUM_ENTITIES] = {};
	static SNote n[NUM_NOTES] = {};

	ShaderState state = {};
	state.io = io;
	state.mc = mc;
	state.mc_len = NUM_MOUSE_CLICKS;
	state.ts = ts;
	state.ts_len = NUM_TILE_STATES;
	state.resolution = res;
	state.scale= &scale;
	state.translation= &translation;
	state.entities = e;
	state.entity_len = NUM_ENTITIES;
	state.notes = n;
	state.note_len = NUM_NOTES;

	update(&state,t);

	vec2 harmonics;

	get_harmonic_fractions(&harmonics.x, &harmonics.y);
	check_errors();

	glUniform1fv(g_unif_time, 1, (GLfloat *)&t);
	glUniform2fv(g_unif_res, 1, (GLfloat *)&res);
	glUniform2fv(g_unif_translation, 1, (GLfloat *)&translation);
	glUniform2fv(g_unif_scale, 1, (GLfloat *)&scale);
	glUniform2fv(g_unif_harmonics, 1, (GLfloat *)&harmonics);

	glUniform3fv(g_unif_mouse_clicks,	NUM_MOUSE_CLICKS, (GLfloat *)mc);
	glUniform3fv(g_unif_tile_states,	NUM_TILE_STATES, (GLfloat *)ts);
	glUniform4fv(g_unif_entities,		NUM_ENTITIES, (GLfloat *)e);
	glUniform4fv(g_unif_notes,			NUM_NOTES, (GLfloat *)n);

	check_errors();
	
	glBindVertexArray(g_VaoHandle);

	{
		Vert verts[4] = { { -1,-1 },{ 1,-1 },{ -1,1 },{1,1} };
		unsigned short tris[6] = { 0,1,2,  2,1,3};
		glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(verts), (GLvoid*)verts, GL_STREAM_DRAW);
			
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_tris_handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tris), (GLvoid*)tris, GL_STREAM_DRAW);
		glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_SHORT, 0);
	}

	// Restore modified GL state
	glUseProgram(last_program);
	glActiveTexture(last_active_texture);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
	check_errors();
}

void mouse_btn_call(GLFWwindow*, int button, int action, int /*mods*/)
{
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		g_MousePressed[button] = true;
}

void scroll_call(GLFWwindow*, double /*xoffset*/, double yoffset)
{
	g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
}

void key_call(GLFWwindow*, int key, int, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void char_call(GLFWwindow*, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}

#include "stdio.h"
#include "malloc.h"

void check_shader_err(int shader)
{
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		char *log= (char *)malloc(logSize);
		int len = 0;
		glGetShaderInfoLog(shader, logSize, &len, log);
		int q = 0;
	}

}

bool create()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"#version 330\n"
		"in vec2 Position;\n"
		"out vec2 pos;\n"
		"uniform vec2 translation;"
		"uniform vec2 scale;"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(Position.xy,0,1);\n"
		"	pos = Position.xy*scale+translation\n;"
		"}\n";

	static char buffer[5000];
	FILE *frag_file = fopen("C:\\Users\\Daniel\\Desktop\\imgui\\examples\\opengl3_example\\hex.frag","rb");
	int read = fread(buffer, 1, sizeof(buffer), frag_file);
	fclose(frag_file);
	buffer[read] = 0;
	const GLchar *fragment_shader = (GLchar *)buffer;
	g_ShaderHandle = glCreateProgram();
	g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
	glShaderSource(g_FragHandle, 1, &fragment_shader, 0);

	glCompileShader(g_VertHandle);
	check_shader_err(g_VertHandle);
	glCompileShader(g_FragHandle);
	check_shader_err(g_FragHandle);


	glAttachShader(g_ShaderHandle, g_VertHandle);
	
	glAttachShader(g_ShaderHandle, g_FragHandle);
	glLinkProgram(g_ShaderHandle);



	check_errors();

	g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
	g_unif_time = glGetUniformLocation(g_ShaderHandle, "time");
	g_unif_res = glGetUniformLocation(g_ShaderHandle, "res");
	g_unif_translation = glGetUniformLocation(g_ShaderHandle, "translation");
	g_unif_scale = glGetUniformLocation(g_ShaderHandle, "scale");
	g_unif_mouse_clicks = glGetUniformLocation(g_ShaderHandle, "mouse_clicks");
	g_unif_notes = glGetUniformLocation(g_ShaderHandle, "notes");
	g_unif_tile_states = glGetUniformLocation(g_ShaderHandle, "tile_states");
	g_unif_entities = glGetUniformLocation(g_ShaderHandle, "entities");
	g_unif_harmonics = glGetUniformLocation(g_ShaderHandle, "harmonics");
	//g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");

	glGenBuffers(1, &g_VboHandle);
	glGenBuffers(1, &g_tris_handle);
	check_errors();

	glGenVertexArrays(1, &g_VaoHandle);
	glBindVertexArray(g_VaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
	glEnableVertexAttribArray(g_AttribLocationPosition);
	//glEnableVertexAttribArray(g_AttribLocationUV);
	//glEnableVertexAttribArray(g_AttribLocationColor);
	check_errors();

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	//glVertexAttribPointer(g_AttribLocationUV,		sizeof(vec2),  GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)OFFSETOF(vert, uv));
#undef OFFSETOF

	check_errors();
	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);
	check_errors();

	return true;
}

void invalidate()
{
	if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
	if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
	if (g_tris_handle) glDeleteBuffers(1, &g_tris_handle);
	g_VaoHandle = g_VboHandle = g_tris_handle = 0;

	glDetachShader(g_ShaderHandle, g_VertHandle);
	glDeleteShader(g_VertHandle);
	g_VertHandle = 0;

	glDetachShader(g_ShaderHandle, g_FragHandle);
	glDeleteShader(g_FragHandle);
	g_FragHandle = 0;

	glDeleteProgram(g_ShaderHandle);
	g_ShaderHandle = 0;
}
