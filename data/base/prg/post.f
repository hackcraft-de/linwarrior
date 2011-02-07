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

float getAO(float d0, float dn)
{
	float eps = +0.0015;
	float diff = (dn - d0) + eps;
	return exp(-abs(diff) * 700.0);
}

float contrib(float d0, float dn)
{
	float diff = d0 - dn;
	float eps = 0.0001;
	if (abs(diff) < eps) return 1.0;
	return exp(-(abs(diff)-eps)*500.0);
}

float noiz3(float x, float y, float z)
{
	float u = 121.12421 * (1.0+x);
	float v = 731.23897 * (1.0+x+y);
	float w = 531.38413 * (1.0+x+y+z);
	return 0.5 + 0.5 * 0.3333 * ( sin(w + u*sin(u + v*sin(v))) + sin(v + w*sin(w + u*sin(u))) +  sin(u + v*sin(v + w*sin(w))) );
}

float ds[17] = float[](0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000);
float dc[17] = float[](1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000);

void main()
{
	vec2 pix = gl_TexCoord[0].xy;
	//float nx = 3.0 * ofs * (noiz3(gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0) - 0.5);
	//float ny = 3.0 * ofs * (noiz3(gl_TexCoord[0].y, gl_TexCoord[0].x, 0.0) - 0.5);
	//pix = vec2(ofs*0.0 + 0.5 - 0.5 * cos(pix.x*3.14), 0.5 - 0.5 * cos(pix.y*3.14));

	//float str = 60.0 * linear(texture2DLod(dep,gl_TexCoord[0].xy, 0.0).x);
	float str = 10.0;
	//pix = vec2(pix.x + str * ofs*(0.5 - 0.5 * cos(pix.x*31.4)), pix.y + str * ofs*(0.5 - 0.5 * cos(pix.y*31.4)));

	float rd = ofs * 0.35;

	// Color blur
	float r = rd;
	float calpha = 0.3;
	vec4 c0 = texture2DLod(tex,pix, 0.0);
	vec4 c1 = mix(c0, texture2DLod(tex,pix+vec2(ds[1]*r,dc[1]*r), 0.0), calpha); r += rd;
	vec4 c2 = mix(c1, texture2DLod(tex,pix+vec2(ds[2]*r,dc[2]*r), 0.0), calpha); r += rd;
	vec4 c3 = mix(c2, texture2DLod(tex,pix+vec2(ds[3]*r,dc[3]*r), 0.0), calpha); r += rd;
	vec4 c4 = mix(c3, texture2DLod(tex,pix+vec2(ds[4]*r,dc[4]*r), 0.0), calpha); r += rd;
	vec4 c5 = mix(c4, texture2DLod(tex,pix+vec2(ds[5]*r,dc[5]*r), 0.0), calpha); r += rd;
	vec4 color0 = c0;
	vec4 colorn = c5;
	vec4 blur = colorn - color0;

	float nois = noiz3(gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0);
	float sn = 1.0;//sin(nois * 3.14);
	float cn = 1.0;//sin(nois * 3.14);

	// Depth blur
	float dalpha = 0.3;
	float d0nonlin = texture2DLod(dep,gl_TexCoord[0].xy, 0.0).x;
	float d0 = linear(d0nonlin);
	rd = ofs * 0.51;
	r = rd;
	float d1_ = linear(texture2DLod(dep,pix+vec2(sn*ds[1]*r,cn*dc[1]*r), 0.0).x);
	float d1 = mix(d0, d1_, dalpha); r += rd;
	float d2_ = linear(texture2DLod(dep,pix+vec2(sn*ds[2]*r,cn*dc[2]*r), 0.0).x);
	float d2 = mix(d1, d2_, dalpha); r += rd;
	float d3_ = linear(texture2DLod(dep,pix+vec2(sn*ds[3]*r,cn*dc[3]*r), 0.0).x);
	float d3 = mix(d2, d3_, dalpha); r += rd;
	float d4_ = linear(texture2DLod(dep,pix+vec2(sn*ds[4]*r,cn*dc[4]*r), 0.0).x);
	float d4 = mix(d3, d4_, dalpha); r += rd;
	float d5_ = linear(texture2DLod(dep,pix+vec2(sn*ds[5]*r,cn*dc[5]*r), 0.0).x);
	float d5 = mix(d4, d5_, dalpha); r += rd;
	float depth0 = d0;
	float depthn = d5;

	// SeDepth blur
	float depthu = 0.0;
	float depthv = 0.0;
	float depthw = 0.0;
	float depthx = 0.0;
	float depthy = 0.0;
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

	// Derive Normal
	float e = ofs * 0.9;
	float d0x = linear(texture2DLod(dep,pix+vec2(e, 0.0), 0.0).x);
	float d0y = linear(texture2DLod(dep,pix+vec2(0.0, e), 0.0).x);
        float xd = (d0x - d0);
        float yd = (d0y - d0);
	vec3 nrm_ = normalize(vec3( -e * xd, -yd * e, e * e));
//	nrm_ = nrm_ * transpose(gl_NormalMatrix);
        vec3 nrm1 = vec3(0.5, 0.5, 0.5) + 0.5 * nrm_;
//	nrm1 = transpose(gl_NormalMatrix) * nrm1;
	vec4 nrm = vec4(nrm1, 1.0);

	// Unproject Pos
	mat4 MVPinv = gl_ModelViewProjectionMatrixInverse;
	vec4 voxel = MVPinv * (vec4(pix.x, pix.y, d0nonlin, 1.0 ) * 2.0 - 1.0);
	voxel.xyz /= voxel.w;
	voxel.xyz *= 0.01;
	voxel.w = 1.0;

	vec4 poscode = vec4(0.5*sin(voxel.xyz*100.0) + 0.5, 1.0);

	// Openness
	float outside = min(1.0, 5.0 * max(0.0, -(depthn-depth0)));
	float inside = min(1.0, 10.0 * max(0.0, +(depthn-depth0)));
	float l = 0.0035;
	float openness = 0.5 - 0.5 * min(l, max(-l, depth0 - depthn)) / l;
	if (depthn < depth0 - 4.0*l) openness = 1.0;

	// AO
	float ao = getAO(depth0, depthu);
	ao = max(ao, getAO(depth0, depthv));
	ao = max(ao, getAO(depth0, depthw));
	ao = max(ao, getAO(depth0, depthx));
	ao = max(ao, getAO(depth0, depthy));
	float ao_ = min(1.0, 1.0 - ao);

	float bluryness = min(1.0, 1.2*(depth0-0.1));
	float lum0 = (color0.r + color0.g + color0.b);
	float lumn = (colorn.r + colorn.g + colorn.b);
	float bloom = 1.0*(sig((max(0,lumn - lum0) - 0.0005) * 9) - 0.1);
	vec4 composite = color0 + blur * bluryness + (0.80 * bloom) * color0;

#if 0
	vec4 result = (1.0+0.0*nois) * composite;// + 10.0*d0 * max(0.0,-voxel.y) * vec4(1.0,1.0,0.0,1.0) + 3.0*d0 * max(0.0,voxel.y) * vec4(1.0,1.0,1.0,1.0);
#elif 0
	vec4 result = vec4(100.0*abs(vec3(depthn - depth0)), 1.0);
#elif 0
	vec4 result = vec4((100.0*abs(vec3(depthy - depth0))), 1.0);
#elif 1
	vec4 result = vec4(1.0 * color0.rgb - pow(ao, 3.5)*0.7, 1.0);
	result = mix(result, vec4(vec3(pow(ao_, 3.5)), 3.0), 0.09);
#elif 1
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
	gl_FragColor = result;
}


