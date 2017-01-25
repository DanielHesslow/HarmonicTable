#version 330
vec4 hexagon(vec2 p)
{
	vec2 q = vec2(p.x*2.0*0.5773503, p.y + p.x*0.5773503);

	vec2 pi = floor(q);
	vec2 pf = fract(q);

	float v = mod(pi.x + pi.y, 3.0);

	float ca = step(1.0, v);
	float cb = step(2.0, v);
	vec2  ma = step(pf.xy, pf.yx);

	// distance to borders
	float e = dot(ma, 1.0 - pf.yx + ca*(pf.x + pf.y - 1.0) + cb*(pf.yx - 2.0*pf.xy));

	// distance to center	
	p = vec2(q.x + floor(0.5 + p.y / 1.5), 4.0*p.y / 3.0)*0.5 + 0.5;
	float f = length((fract(p) - 0.5)*vec2(1.0, 0.85));

	return vec4(pi + ca - cb*ma, e, f);
}

vec3 rainbow(float t) {
	return sin(t + vec3(0, .33, .66)*6.28)*.5 + .5;
}

#define NUM_MOUSE_CLICKS 10
#define NUM_TILE_STATES  10
#define NUM_NOTES		 10
#define NUM_ENTITIES	 10

uniform vec2 res;
uniform vec3 mouse_clicks[NUM_MOUSE_CLICKS];
uniform vec3 tile_states[NUM_TILE_STATES];
uniform vec4 notes[NUM_NOTES];
uniform vec4 entities[NUM_ENTITIES];
uniform float time;
uniform vec2 harmonics;


out vec4 frag_color;
in vec2 pos;


float man_dist(vec2 a, vec2 b)
{
	vec2 t = abs(a-b);
	return t.x + t.y;
}



vec2 mu_hex(vec2 p)
{
	float a = (2.*p.y - p.x) / 3.;
	float b = (2.*p.x - p.y) / 3.;
	return vec2(b, a);
}

vec2 xy_hex(vec2 p)
{
	vec2 mu = mu_hex(p);
	return vec2(ceil(mu.y / 2.) + mu.x, mu.y);
}



vec2 center(vec2 hex)
{
	hex = xy_hex(hex);
	float y = hex.y*1.5;

	float x_d = 0.5773503;
	float x = (hex.x-floor(mod(hex.y,2.))/2.)/x_d;
	return vec2(x,y);
}

void main()
{
	vec3 c;
	vec3 border_color = vec3(1.0);
	vec2 p = pos*res/1000;
	vec4 hex = hexagon(p);
	//c = vec3(rainbow((hex.x + 6.*hex.y) / 40.)); // shade each hex in a nice color

	vec2 mu = mu_hex(hex.xy)-15;
	float pitch = log(pow(harmonics.x,mu.y)*pow(harmonics.y,mu.x));
	c = vec3(rainbow(pitch)); // shade each hex according to pitch


	float width = 0.03;
	float fuzz = 0.02;
	for(int i = 0; i<5;i++)
	{
		vec3 m = mouse_clicks[i];
		float dt = time - m.z;
		if(m.xy == hex.xy)
		{
			float ss = 1.-smoothstep(0.0,1.0,pow(0.01,dt*0.2));
			c = pow(c,vec3(ss));
		}
	}
	for(int i = 0; i<NUM_NOTES;i++)
	{

	
		vec4 m = notes[i];
		float dt = time - m.z;
		vec2 pos = m.xy;
		float d = distance(pos,p);

		float intensity = smoothstep(dt,2.0,d+1.0)*smoothstep(2.0+dt,0.0+dt,d+1.0) * max(0.0,1.-dt);
		c = mix(c, c+(sin(distance(pos,p)*10.-dt*10.)),intensity) ;
	}

	for(int i = 0; i<NUM_TILE_STATES;i++)
	{
		vec3 m = tile_states[i];
		vec2 hc = center(hex.xy);
		if(m.xy == hex.xy)
		{
			float rad = 0.3;
			if(m.z == 1.)
				c = mix(c, vec3(1.), smoothstep(1.0-rad+fuzz,1.0-rad-fuzz,distance(hc,p)));
			else if(m.z == 2.)
				c = mix(c, vec3(1.), smoothstep(rad-fuzz,rad+fuzz,hex.z));
		}
	}
		
	

	for(int i = 0; i<NUM_ENTITIES;i++)
	{
		vec4 e = entities[i];
		c = mix(c, vec3(.2,.3,.6), smoothstep(.3,.2,distance(p,e.xy)));
	}



	float x_d = 0.5773503;
	float y_d = 1./1.5;
	float ym = floor(mod(hex.x,2.));


	
	
	
	c = mix(c, border_color, smoothstep(width+fuzz, width-fuzz, hex.z)); // mix in the borders
	float bright = 0.7;
	c = c *bright +(1.-bright);//brighten
	frag_color = vec4(c,1.0);
}


