//A brutally simple vertex shader that just mimics exactly what OpenGL's default
//fixed pipeline does
uniform float currentTime;

void main() {                
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[0].s += currentTime*0.003;
    gl_TexCoord[0].t += currentTime*0.003;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
