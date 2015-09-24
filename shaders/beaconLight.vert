//Super-simple vertex shader, basically just says transform this
//vertex position according to the model view and projection matrices
//This is exactly what OpenGL would do if we weren't writing a shader
uniform float currentTime;

void main() {

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
