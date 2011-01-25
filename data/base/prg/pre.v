varying vec3 normal, position, lightdir;
varying float direction;

void main()
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = ftransform();
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightdir = normalize(vec3(gl_LightSource[0].position.xyz - position));
	direction = max(dot(lightdir,normalize(normal)),0.0);
}


