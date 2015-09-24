//A brutally simple vertex shader that just mimics exactly what OpenGL's default
//fixed pipeline does

varying vec2 rgbTexCoord;

void main() {                
    
    // Scale used for the island terrain
    float scale = 6.0;

    rgbTexCoord = gl_MultiTexCoord0.st;

    rgbTexCoord.st = rgbTexCoord * scale  ;
    
    gl_TexCoord[0] = gl_MultiTexCoord0  ;

    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex ;
}
