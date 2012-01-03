// Simplex Noise
//
// Shader implementation by
//
// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de


uniform sampler1D Perm;
uniform sampler1D Gradient;

float noise(float x, float y, float z, float seed)
{
	bool simplexonly = !true;
    const float skew = 1.0 / 3.0;
    const float unsk = 1.0 / 6.0;
	
    // Transform orthogonal input to skewed simplex space.
    // Take the floor to get the simplex grid point.
	float offset = (x + y + z) * skew;
	float x_f = x + offset;
	float y_f = y + offset;
	float z_f = z + offset;
	int x_ = int(x_f);
	int y_ = int(y_f);
	int z_ = int(z_f);
	if (x_f <= 0.0) x_ = int(x_f - 1.0);
	if (y_f <= 0.0) y_ = int(y_f - 1.0);
	if (z_f <= 0.0) z_ = int(z_f - 1.0);
	
    // And transform grid point back to orthogonal space.
    offset = float(x_ + y_ + z_) * unsk;
    float x_o = float(x_) - offset;
    float y_o = float(y_) - offset;
    float z_o = float(z_) - offset;
    
    // Determine relative distance to grid point in orthogonal space.
    float xofs0 = x - x_o;
    float yofs0 = y - y_o;
    float zofs0 = z - z_o;
    
    const float scaling = (1.0 / 256.0);
    
    // Determine tetrahedron edge directions.
    vec3 left;
    vec3 right;
    if (xofs0 >= yofs0) {
    	if (xofs0 >= zofs0) {
    		if (yofs0 >= zofs0) {
    			left  = vec3(1.,0.,0.);
    			right = vec3(1.,1.,0.);
    		} else {
    			left  = vec3(1.,0.,0.);
    			right = vec3(1.,0.,1.);
    		}
    	} else {
			left  = vec3(0.,0.,1.);
			right = vec3(1.,0.,1.);
		}
    } else {
    	if (xofs0 >= zofs0) {
			left  = vec3(0.,1.,0.);
			right = vec3(1.,1.,0.);
    	} else {
    		if (yofs0 >= zofs0) {
				left  = vec3(0.,1.,0.);
				right = vec3(0.,1.,1.);
	    	} else {
				left  = vec3(0.,0.,1.);
				right = vec3(0.,1.,1.);
    		}
    	}
    }

    // Simplex corner 2 offset ("left")
    float xofs1 = xofs0 - left[0] + 1.0 * unsk;
    float yofs1 = yofs0 - left[1] + 1.0 * unsk;
    float zofs1 = zofs0 - left[2] + 1.0 * unsk;
    // Simplex corner 3 offset ("right")
    float xofs2 = xofs0 - right[0] + 2.0 * unsk;
    float yofs2 = yofs0 - right[1] + 2.0 * unsk;
    float zofs2 = zofs0 - right[2] + 2.0 * unsk;
    // Simplex corner 4 offset ("far")
    float xofs3 = xofs0 - 1.0 + 3.0 * unsk;
    float yofs3 = yofs0 - 1.0 + 3.0 * unsk;
    float zofs3 = zofs0 - 1.0 + 3.0 * unsk;

    // Have the base grid point wrapped inside [0,255] -> [0,1].
    float u = float(x_) * scaling + seed;
    float v = float(y_) * scaling + seed;
    float w = float(z_) * scaling + seed;
    left *= scaling;
    right *= scaling;
    vec3 far = vec3(scaling,scaling,scaling);
        
    // Contribution "attenuation" factors for distances to grid corner points.
    float c0 = 0.6 - (xofs0*xofs0 + yofs0*yofs0 + zofs0*zofs0);
    float c1 = 0.6 - (xofs1*xofs1 + yofs1*yofs1 + zofs1*zofs1);
    float c2 = 0.6 - (xofs2*xofs2 + yofs2*yofs2 + zofs2*zofs2);
    float c3 = 0.6 - (xofs3*xofs3 + yofs3*yofs3 + zofs3*zofs3);
    
    if (simplexonly) return c0+c1+c2+c3;

    // Contribution sum of the four corners.
    float sum = 0.0;

    // Get gradient index by permutation (Perm).
    // "Base" corner.
    float g0 = texture1D(Perm, u + texture1D(Perm, v + texture1D(Perm, w)[0])[0])[0];
    // "Left" corner.
    float g1 = texture1D(Perm, u + left[0] + texture1D(Perm, v + left[1] + texture1D(Perm, w + left[2])[0])[0])[0];
    // "Right" corner.
    float g2 = texture1D(Perm, u + right[0] + texture1D(Perm, v + right[1] + texture1D(Perm, w + right[2])[0])[0])[0];
    // "Far" corner.
    float g3 = texture1D(Perm, u + far[0] + texture1D(Perm, v + far[1] + texture1D(Perm, w + far[2])[0])[0])[0];

    // base
    if (c0 > 0.0) {
        vec3 g_ = vec3(texture1D(Gradient, g0)); // Gradient
        g_ = g_ - 0.5;
        float sqr = c0 * c0;
        sum += sqr * sqr * (g_[0]*xofs0 + g_[1]*yofs0 + g_[2]*zofs0);
    }

    // left
    if (c1 > 0.0) {
        vec3 g_ = vec3(texture1D(Gradient, g1)); // Gradient
        g_ = g_ - 0.5;
        float sqr = c1 * c1;
        sum += sqr * sqr * (g_[0]*xofs1 + g_[1]*yofs1 + g_[2]*zofs1);
    }

    // right
    if (c2 > 0.0) {
        vec3 g_ = vec3(texture1D(Gradient, g2)); // Gradient
        g_ = g_ - 0.5;
        float sqr = c2 * c2;
        sum += sqr * sqr * (g_[0]*xofs2 + g_[1]*yofs2 + g_[2]*zofs2);
    }

    // far
    if (c3 > 0.0) {
        vec3 g_ = vec3(texture1D(Gradient, g3)); // Gradient
        g_ = g_ - 0.5;
        float sqr = c3 * c3;
        sum += sqr * sqr * (g_[0]*xofs3 + g_[1]*yofs3 + g_[2]*zofs3);
    }

    // [-1,+1]
    return 2.0 * 32.0 * sum;
}

varying float LightIntensity; 
varying vec3  position;

uniform int  time;
uniform float speed;

vec3 offset = vec3(0,0,0);

void main (void)
{  
  float ofs = float(time) * speed;
  offset = vec3(-ofs,ofs,ofs);
  vec3 voxel = position + offset;
  float u = noise(voxel[0]*1.0, voxel[1]*1.0, voxel[2]*1.0, 0.0327);
  float v = noise(voxel[0]*2.0, voxel[1]*2.0, voxel[2]*2.0, 0.123);
  float w = noise(voxel[0]*4.0, voxel[1]*4.0, voxel[2]*4.0, 0.231);
  float k = noise(voxel[0]*8.0, voxel[1]*8.0, voxel[2]*8.0, 0.531);
  float composite = u + 0.5 * v + 0.25 * w + 0.125 * k + 0.0;
  vec4  noisevec  = vec4(composite, composite, composite, 1.0);
  gl_FragColor = clamp(noisevec + 0.5*LightIntensity, 0.0, 1.0); 
}

