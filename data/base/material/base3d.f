varying vec3 normal, position, lightdir;
varying float direction;
uniform sampler3D tex;

void main()
{
	vec4 ambient = gl_LightSource[0].ambient;
	vec4 diffuse = gl_LightSource[0].diffuse * direction;
	vec4 basecolor = texture3D(tex,gl_TexCoord[0].xyz) * gl_Color;
	gl_FragColor = vec4(basecolor.rgb * diffuse + basecolor.rgb * ambient, basecolor.a);
}


