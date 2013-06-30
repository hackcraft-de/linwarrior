// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

// --------------------
// Distortion functions


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
	float eps = +0.002;
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


float mixd(float a, float b, float alpha)
{
	return b;//((1.0-alpha)*a + alpha*b);
}


float ds[17] = float[](0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000);
float dc[17] = float[](1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000, -0.707, 0.000, 0.707, -1.000, 0.707, 0.000, -0.707, 1.000);


vec2 ofse(int i, float r, vec2 p)
{
	return vec2(1.0*ds[i]*r, (1.7*r) + 1.0*dc[i]*r);
	//return vec2(0.5*r-noiz3(p.x,p.y, i*0.23)*r, 0.5*r-noiz3(p.x,p.y, i*0.17)*r);
}



//

vec4 deriveNormal(sampler2D dep, vec2 pix, float pixofs) {

	float e = pixofs * 0.9;

	float d0  = linear(textureLod(dep,pix, 0.0).x);
	float d0x = linear(textureLod(dep,pix+vec2(e, 0.0), 0.0).x);
	float d0y = linear(textureLod(dep,pix+vec2(0.0, e), 0.0).x);

    float xd = (d0x - d0);
    float yd = (d0y - d0);

	vec3 nrm_ = normalize(vec3( -e * xd, -yd * e, e * e));
	//nrm_ = nrm_ * transpose(gl_NormalMatrix);
    vec3 nrm1 = vec3(0.5, 0.5, 0.5) + 0.5 * nrm_;
	//nrm1 = transpose(gl_NormalMatrix) * nrm1;
	vec4 nrm = vec4(nrm1, 1.0);

	return nrm;
}




