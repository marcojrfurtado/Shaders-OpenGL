//This pixel shader basically says: "I don't care about anything else,
//just paint this pixel (aka "fragment") white no matter what!"
uniform float currentTime;

void main() {   
    gl_FragColor = vec4(1.0 * abs(sin(currentTime)),0.0,0.0,1.0)  ;
}
