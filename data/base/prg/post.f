// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

#version 120

varying vec3 normal, position, lightdir;
varying float direction;

uniform sampler2D tex;
uniform sampler2D dep;

float ofs = 0.0011;

float linear(float z)
{
	float n = 0.07;// 0.0*gl_DepthRange.near;
	float f = 300.0;// + 0.0*gl_DepthRange.far;
	return (2.0 * n) / (f + n - z * (f - n));
}

float sig(float x)
{
	return 1.0 / (1.0 + exp(-x));
}

float ds[17] = float[](0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000);
float dc[17] = float[](1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000);

void main()
{
	float rd = ofs * 0.33;

	float r = rd;
	float calpha = 0.3;
	vec4 c0 = texture2DLod(tex,gl_TexCoord[0].xy, 0.0);
	vec4 c1 = mix(c0, texture2DLod(tex,gl_TexCoord[0].xy+vec2(ds[1]*r,dc[1]*r), 1.0), calpha); r += rd;
	vec4 c2 = mix(c1, texture2DLod(tex,gl_TexCoord[0].xy+vec2(ds[2]*r,dc[2]*r), 1.0), calpha); r += rd;
	vec4 c3 = mix(c2, texture2DLod(tex,gl_TexCoord[0].xy+vec2(ds[3]*r,dc[3]*r), 1.0), calpha); r += rd;
	vec4 c4 = mix(c3, texture2DLod(tex,gl_TexCoord[0].xy+vec2(ds[4]*r,dc[4]*r), 1.0), calpha); r += rd;
	vec4 c5 = mix(c4, texture2DLod(tex,gl_TexCoord[0].xy+vec2(ds[5]*r,dc[5]*r), 1.0), calpha); r += rd;
	vec4 color0 = c0;
	vec4 colorn = c5;
	vec3 blur = colorn.rgb - color0.rgb;

	r = rd;
	float dalpha = 0.3;
	float d0 = linear(texture2DLod(dep,gl_TexCoord[0].xy, 0.0).x);
	float d1 = mix(d0, linear(texture2DLod(dep,gl_TexCoord[0].xy+vec2(ds[1]*r,dc[1]*r), 1.0).x), dalpha); r += rd;
	float d2 = mix(d1, linear(texture2DLod(dep,gl_TexCoord[0].xy+vec2(ds[2]*r,dc[2]*r), 1.0).x), dalpha); r += rd;
	float d3 = mix(d2, linear(texture2DLod(dep,gl_TexCoord[0].xy+vec2(ds[3]*r,dc[3]*r), 1.0).x), dalpha); r += rd;
	float d4 = mix(d3, linear(texture2DLod(dep,gl_TexCoord[0].xy+vec2(ds[4]*r,dc[4]*r), 1.0).x), dalpha); r += rd;
	float d5 = mix(d4, linear(texture2DLod(dep,gl_TexCoord[0].xy+vec2(ds[5]*r,dc[5]*r), 1.0).x), dalpha); r += rd;
	float depth0 = d0;
	float depthn = d5;

	float outside = min(1.0, 5.0 * max(0.0, -(depthn-depth0)));
	float inside = min(1.0, 10.0 * max(0.0, +(depthn-depth0)));
	float l = 0.0035;
	float openness = 0.5 - 0.5 * min(l, max(-l, depth0 - depthn)) / l;
	if (depthn < depth0 - 4.0*l) openness = 1.0;

	vec4 result = vec4(0,0,0,0);
	if (0.75*gl_TexCoord[0].x + 0.25*gl_TexCoord[0].y <= 2.80) {
#if 1
		float bluryness = min(1.0, 0.9*(depth0-0.1));
		float lum0 = (color0.r + color0.g + color0.b);
		float lumn = (colorn.r + colorn.g + colorn.b);
		float bloom = 1.0*(sig((max(0,lumn - lum0) - 0.0005) * 9) - 0.1);
		result = vec4(color0.rgb + bluryness*blur + (0.10 * bloom) * (color0.rgb), color0.a);
#else
		result = vec4(color0.rgb+vec3(1,1,1)*openness,1);
#endif
	} else {
		result = vec4(vec3(inside,openness,outside),1.0);
	}
	gl_FragColor = result;
}


