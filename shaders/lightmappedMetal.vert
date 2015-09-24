//A brutally simple vertex shader that just mimics exactly what OpenGL's default
//fixed pipeline does


void main() {                
    
	vec4 diffuse, ambient;
        vec3 normal, lightDir;
	float NdotL;
	
	lightDir = normalize(vec3(gl_LightSource[0].position));
	normal = normalize(gl_NormalMatrix * gl_Normal );
	NdotL = max(dot(normal, lightDir), 0.0);

	ambient = gl_LightSource[0].ambient;
	diffuse = gl_LightSource[0].diffuse;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_FrontColor =  NdotL * diffuse + ambient;
	gl_Position = ftransform();
}
