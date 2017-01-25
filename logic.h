#pragma once
#include "Math.h"
#include "Sound.h"
vec2 mu_hex(vec2 p)
{
	float a = (2.*p.y - p.x) / 3.;
	float b = (2.*p.x - p.y) / 3.;
	return{ b, a };
}

vec2 xy_hex(vec2 p)
{

	// from format	 to format
	//   			   
	//     /  \		     /  \
	//    |1, 2|	    |1, 1|
	//   / \  / \ 	   / \  / \ 
	//	|0,0||2,1|	  |0,0||1,0|
	//   \ /  \ /	   \ /  \ /
	//    |1,-1|	    |1,-1|
	//     \  /          \  /

	vec2 mu = mu_hex(p);
	return{ (float)ceil(mu.y / 2.) + mu.x, mu.y };
}

vec2 center(vec2 hex)
{
	hex = xy_hex(hex);
	float y = hex.y*1.5;

	float x_d = 0.5773503;
	float x = (hex.x - floor(fmod(hex.y, 2.)) / 2.) / x_d;
	return{ x, y };
}

struct ShaderState
{
	STileState *ts;
	int ts_len;
	
	SMouseClick *mc;
	int mc_len;

	SEntity *entities;
	int entity_len;

	SNote *notes;
	int note_len;

	vec2 resolution;
	vec2 *scale;
	vec2 *translation;
	ImGuiIO io;
};	

float frac1 = 5./4.;
float frac2 = 3./2.;



void get_harmonic_fractions(float *frac1, float *frac2);
void update(ShaderState *state, float t)
{
	get_harmonic_fractions(&frac1, &frac2);

	static float last_time = t;
	float dt = t - last_time;
	last_time = t;
	SMouseClick *mc = state->mc;
	STileState *ts = state->ts;
	SEntity *e = state->entities;
	SNote *n = state->notes;

	ImGuiIO io = state->io;

	if (io.MouseClicked[0])
	{
		if (io.WantCaptureMouse)return;

		for (int i = state->mc_len-2; i >= 0; i--) mc[i + 1] = mc[i];
		vec2 p = { io.MouseClickedPos[0].x,io.MouseClickedPos[0].y };
		hex_point hex = hexagon(to_gl(p, state->resolution, *state->scale, *state->translation));
		mc[0].pos = hex.coords;
		mc[0].time = t;

		vec2 mu = mu_hex(hex.coords);
		mu = mu - 15;


		float freq = 440 * pow(frac1,mu.y) * pow(frac2, mu.x);
		addNote(freq);

	}
	if (io.MouseClicked[1])
	{
		clear_notes();
		ImGui::OpenPopup("super_popup");
		/*
		vec2 p = { io.MouseClickedPos[1].x,io.MouseClickedPos[1].y };
		hex_point hex = hexagon(to_gl(p, state->resolution, *state->scale, *state->translation));
		bool done = false;
		for (int i = 0; i < state->ts_len; i++)
		{
			if (ts[i].pos == hex.coords)
			{
				++ts[i].type;
				if (ts[i].type == 3.) ts[i].type = 0.;
				done = true;
			}
		}
		if (!done)
		{
			for (int i = state->ts_len-2; i >= 0; i--) ts[i + 1] = ts[i];

			ts[0].pos = hex.coords;
			ts[0].type = 1.0;
		}
		*/

	}

	static float last_set=0.0f;

	if (t - last_set > 1.0f)
	{
		last_set = t;
		for (int i = 0; i < state->ts_len; i++)
		{
			if (ts[i].type == 1.)
			{
				for (int i = state->entity_len - 2; i >= 0; i--) e[i + 1] = e[i];
				e[0].pos = center(ts[i].pos);
				e[0].dir = { 1.0,0.0 };
			}
		}
	}


	for (int i = 0; i < state->entity_len; i++)
	{
		vec2 zero = { 0,0 };
		if ((e[i].dir == zero)) continue;
		e[i].pos = e[i].pos + e[i].dir *dt;
		vec2 ec = hexagon(e[i].pos).coords;

		for (int j = 0; j < state->ts_len; j++)
		{
			if (ec == ts[j].pos && ts[j].type == 2.)
			{
				e[i].pos = { 0, 0 };
				e[i].dir = { 0, 0 };

				for (int i = state->entity_len - 2; i >= 0; i--) n[i + 1] = n[i];
				n[0].freq = 440.;
				n[0].pos  = center(ts[j].pos);
				n[0].time = t;
			}
		}
	}
}


