// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

#include "/base/prgs/warp.inc"


uniform sampler2D colorBufferLeft;
uniform sampler2D colorBufferRight;


vec4 warpMerge(vec2 scale, float shift)
{
	vec4 color = vec4(0,0,0,0);

    // Screen coords:
	vec2 pix = gl_TexCoord[0].xy;

	// Transform screen coords into individual buffer coords.
	if (gl_TexCoord[0].x < 0.5) {
		pix = vec2((gl_TexCoord[0].x - 0.0) * 2.0 - shift, gl_TexCoord[0].y);
	} else {
		pix = vec2((gl_TexCoord[0].x - 0.5) * 2.0 + shift, gl_TexCoord[0].y);
	}

	// Make coords centric for scaling.
	pix = pix - vec2(0.5, 0.5);

	// Scale coords around center.

	pix = vec2(pix.x / scale.x, pix.y / scale.y);

	// Make coords non-centric again.
	pix = pix + vec2(0.5, 0.5);
	
	// Warp buffer coords for projection.
	pix = warpMono(pix);

	// Get color from the individual buffer.
	if (gl_TexCoord[0].x < 0.5) {
		color = textureLod(colorBufferLeft, pix.xy, 0.0);
	} else {
		color = textureLod(colorBufferRight, pix.xy, 0.0);
	}

	return color;
}


void main() {
	vec2 scale = vec2(1.05, 0.75);
	float shift = 0.0765;
	gl_FragColor = warpMerge(scale, shift);
	//gl_FragColor = textureLod(colorBufferLeft, pix.xy, 0.0);
}


