// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

#include "/base/prgs/utils.inc"
#include "/base/prgs/warp.inc"
#include "/base/prgs/color.inc"


varying vec3 normal, position, lightdir;
varying float direction;

uniform sampler2D tex;
uniform sampler2D dep;

float ofs = 0.0011;


vec4 test()
{
        // For Screen:
	vec2 pix = warpNone(gl_TexCoord[0].xy);

        // For HMD:
	//vec2 pix = warpDualShift(gl_TexCoord[0].xy, 0.079);

	//float nx = 3.0 * ofs * (noiz3(gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0) - 0.5);
	//float ny = 3.0 * ofs * (noiz3(gl_TexCoord[0].y, gl_TexCoord[0].x, 0.0) - 0.5);
	//pix = vec2(ofs*0.0 + 0.5 - 0.5 * cos(pix.x*3.14), 0.5 - 0.5 * cos(pix.y*3.14));

	//float str = 60.0 * linear(textureLod(dep,gl_TexCoord[0].xy, 0.0).x);
	float str = 20.0;
	//pix = vec2(pix.x + str * ofs*(0.5 - 0.5 * cos(pix.x*31.4)), pix.y + str * ofs*(0.5 - 0.5 * cos(pix.y*31.4)));

	float rd = ofs * 0.35;


	// Color blur
	float r = rd;
	float calpha = 0.3;
	vec4 c0 = textureLod(tex,pix, 0.0);
	vec4 c1 = mix(c0, textureLod(tex,pix+vec2(ds[1]*r,dc[1]*r), 0.0), calpha); r += rd;
	vec4 c2 = mix(c1, textureLod(tex,pix+vec2(ds[2]*r,dc[2]*r), 0.0), calpha); r += rd;
	vec4 c3 = mix(c2, textureLod(tex,pix+vec2(ds[3]*r,dc[3]*r), 0.0), calpha); r += rd;
	vec4 c4 = mix(c3, textureLod(tex,pix+vec2(ds[4]*r,dc[4]*r), 0.0), calpha); r += rd;
	vec4 c5 = mix(c4, textureLod(tex,pix+vec2(ds[5]*r,dc[5]*r), 0.0), calpha); r += rd;
	vec4 color0 = c0;
	vec4 colorn = c5;
	vec4 blur = colorn - color0;


	float nois = noiz3(gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0);
	float sn = 1.0;//sin(nois * 3.14);
	float cn = 1.0;//sin(nois * 3.14);

	// Depth blur
	float dalpha = 0.7;
	float d0nonlin = textureLod(dep,pix, 0.0).x;
	float d0 = linear(d0nonlin);
	rd = ofs * 0.33 * 1.5;
	r = ofs * 1.0;
	float d1_ = linear(textureLod(dep,pix+ofse(1,r, pix), 0.0).x);
	float d1 = mix(d0, d1_, dalpha); r += rd;
	float d2_ = linear(textureLod(dep,pix+ofse(2,r, pix), 0.0).x);
	float d2 = mix(d1, d2_, dalpha); r += rd;
	float d3_ = linear(textureLod(dep,pix+ofse(3,r, pix), 0.0).x);
	float d3 = mix(d2, d3_, dalpha); r += rd;
	float d4_ = linear(textureLod(dep,pix+ofse(4,r, pix), 0.0).x);
	float d4 = mix(d3, d4_, dalpha); r += rd;
	float d5_ = linear(textureLod(dep,pix+ofse(5,r, pix), 0.0).x);
	float d5 = mix(d4, d5_, dalpha); r += rd;
	float depth0 = d0;
	float depthn = d5;

	// Derive Normal
	vec4 nrm = deriveNormal(dep, pix, ofs);

	float d6_ = linear(textureLod(dep,pix+1.2*ofs*(vec2(nrm.x, nrm.y)-vec2(0.5)), 0.0).x);
	float d6 = mix(d5, d6_, dalpha);

	// Unproject Pos
	mat4 MVPinv = gl_ModelViewProjectionMatrixInverse;
	vec4 voxel = MVPinv * (vec4(pix.x, pix.y, d0nonlin, 1.0 ) * 2.0 - 1.0);
	voxel.xyz /= voxel.w;
	voxel.xyz *= 0.01;
	voxel.w = 1.0;

	vec4 poscode = vec4(0.5*sin(voxel.xyz*100.0) + 0.5, 1.0);

	// SeDepth blur
	float depthu = 0.0;
	float depthv = 0.0;
	float depthw = 0.0;
	float depthx = 0.0;
	float depthy = 0.0;
	float depthz = 0.0;
	{
		float depths = 0.0;
		float sum = 0.0;

		float c = 1.0;
		sum += c;
		depths += c * d0;

		c = contrib(depth0, d1_);
		sum += c;
		depths += c * d1_;
		depthu = depths / sum;

		c = contrib(depth0, d2_);
		sum += c;
		depths += c * d2_;
		depthv = depths / sum;

		c = contrib(depth0, d3_);
		sum += c;
		depths += c * d3_;
		depthw = depths / sum;

		c = contrib(depth0, d4_);
		sum += c;
		depths += c * d4_;
		depthx = depths / sum;

		c = contrib(depth0, d5_);
		sum += c;
		depths += c * d5_;
		depthy = depths / sum;
	}

	// Openness
	float outside = min(1.0, 5.0 * max(0.0, -(depthn-depth0)));
	float inside = min(1.0, 10.0 * max(0.0, +(depthn-depth0)));
	float l = 0.0035;
	float openness = 0.5 - 0.5 * min(l, max(-l, depth0 - depthn)) / l;
	if (depthn < depth0 - 4.0*l) openness = 1.0;

	// AO
	float ao = 0.0;
	ao = max(ao, getAO(depth0, depthu));
	ao = max(ao, getAO(depth0, depthv));
	ao = max(ao, getAO(depth0, depthw));
	ao = max(ao, getAO(depth0, depthx));
	ao = max(ao, getAO(depth0, depthy));
	ao = max(ao, getAO(depth0, mix(depth0,d6_, contrib(depth0,d6_))));
	float ao_ = min(1.0, 1.0 - ao);


	float bluryness = min(1.0, 1.1*(depth0-0.1));
	float lum0 = (color0.r + color0.g + color0.b);
	float lumn = (colorn.r + colorn.g + colorn.b);
	float bloom = 1.0*(sig((max(0,lumn - lum0) - 0.00045) * 9) - 0.1);
	vec4 composite = color0 * (0.70 + 1.69 * bloom) + 0.1 * (blur * bluryness);

#if 1

	vec3 yuv = RGB2YUV(color0.rgb);
	vec3 yuv_blur = RGB2YUV(colorn.rgb) - yuv;

	vec3 yuv_composite = yuv + 0.1 * (yuv_blur * bluryness);
	yuv_composite.r = yuv_composite.r * (0.70 + 1.69 * bloom);

	ao_ = min(1.0, 1.7 * ao_);
	composite.r = (composite.r * ao_) + (0.5 * yuv_blur.r * (1.0-ao_));

	vec4 result = vec4(YUV2RGB(yuv_composite), color0.a);

#elif 0
	//composite = vec4(ao_,ao_,ao_,1.0);
	//composite = nois * composite + (1-nois) * vec4(ao_,ao_,ao_,1.0) * composite;
	//composite *= vec4(ao_,ao_,ao_,1.0);
	ao_ = min(1.0, 1.7 * ao_);
	composite = composite * vec4(ao_,ao_,ao_,1.0) + 0.5 * (blur * vec4(1.0-ao_,1.0-ao_,1.0-ao_,1.0));
	vec4 result = (1.0+0.0*nois) * composite;// + d0 * pow(max(0.0,-voxel.y)*10.0,2.0) * vec4(0.0,1.0,0.0,1.0) + pow(1.0*d0, 0.9) * max(0.0,voxel.y) * vec4(1.0,0.9,0.5,1.0);
#elif 0
	float io = 0.0;
	if (d0 < 0.86) {
		io = 1.0;
	}
	//vec4 result = (1.0+0.0*nois) * composite + io * vec4(1.0,0.9,0.5,1.0);
	vec4 result = (1.0+0.0*nois) * composite + io * pow(1.0*d0, 0.9) * max(0.0,voxel.y) * vec4(1.0,0.9,0.5,1.0);
#elif 0
	vec4 result = vec4(100.0*abs(vec3(depthn - depth0)), 1.0);
#elif 0
	vec4 result = vec4((100.0*abs(vec3(depthy - depth0))), 1.0);
#elif 0
	vec4 result;
	if (gl_TexCoord[0].x < 1.5) {
		result = vec4(vec3(pow(1.4*ao_, 3.5)), 1.0);
	} else {
		result = vec4(color0.rgb, 1.0);
	}
#elif 0
	float mixalpha = 0.5;

	vec3 yuv = RGB2YUV(color0.rgb);

//	yuv.r = (yuv.r * pow(1.3*ao_, 1.0)) + 0.1*bloom;
	yuv.r = (yuv.r * pow(1.4*ao_, 3.5)) + 0.1*bloom;

	color0.rgb = YUV2RGB(yuv);

	vec4 result = vec4(color0.rgb, 1.0);
	if (!true && gl_TexCoord[0].x < 0.5) {
//		result = vec4(vec3(pow(1.3*ao_, 1.0)), 1.0);
		result = vec4(vec3(pow(1.4*ao_, 3.5)), 1.0);
	}
#elif 0
	float mixalpha = 0.5;
	vec4 result = vec4(1.0 * color0.rgb - pow(ao, 3.5)*0.7, 1.0);
	result = mix(result, vec4(vec3(pow(ao_, 3.5)), 3.0), mixalpha);
#elif 0
	vec4 result = vec4(vec3(nois), 1.0);
#else
	vec4 result = vec4(0,0,0,0);
	float slice = 0.9 * gl_TexCoord[0].x - 0.15 * gl_TexCoord[0].y + 0.15;
	float segmentw = 1.0 / 8.0;
	if (slice <= 1.0*segmentw) {
		result = nrm;
	} else if (slice <= 2.0*segmentw) {
		result = composite;
	} else if (slice <= 3.0*segmentw) {
		result = color0;
	} else if (slice <= 4.0*segmentw) {
		result = vec4(vec3(depthy), 1.0);
	} else if (slice <= 5.0*segmentw) {
		result = vec4(depthn*0.25, depthn*0.5, depthn, 1.0);
	} else if (slice <= 6.0*segmentw) {
		result = voxel;
	} else if (slice <= 7.0*segmentw) {
		result = poscode;
	} else {
		result = vec4(vec3(inside,openness,outside), 1.0);
	}
#endif

#if 0
	float ex = 1.0;
	result = vec4(1,1,1,1) - exp(-ex * result);
#endif

#if 0
	float gc = 1.0 / 2.2;
	//float gc = 1.0 / 1.4;
	result = pow(result, vec4(gc,gc,gc,1.0));
#endif
	return result;
}


void main() {
	gl_FragColor = test();
}


