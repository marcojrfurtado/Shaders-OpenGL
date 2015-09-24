uniform sampler2D textureSample_0;

void main() {
    
   gl_FragColor = gl_Color * texture2D(textureSample_0,gl_TexCoord[0].xy);
    
}
