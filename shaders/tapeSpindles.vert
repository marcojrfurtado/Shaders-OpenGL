//A brutally simple vertex shader that just mimics exactly what OpenGL's default
//fixed pipeline does
uniform float currentTime;

void main() {                

    vec4 diffuse, ambient;
    vec3 normal, lightDir;
    float NdotL;
	
    lightDir = normalize(vec3(gl_LightSource[0].position));
    normal = normalize(gl_NormalMatrix * gl_Normal );
    NdotL = max(dot(normal, lightDir), 0.0);

    vec4 aTex = gl_MultiTexCoord0;
    aTex.x = gl_MultiTexCoord0.x*cos(currentTime) - gl_MultiTexCoord0.y*sin(currentTime);
    aTex.y = gl_MultiTexCoord0.y*cos(currentTime) + gl_MultiTexCoord0.x*sin(currentTime);
    
    gl_TexCoord[0] = aTex ;
   
    ambient = gl_LightSource[0].ambient;
    diffuse = gl_LightSource[0].diffuse;
    
    
    gl_FrontColor =  NdotL * diffuse + ambient;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
