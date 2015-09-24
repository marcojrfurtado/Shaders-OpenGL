//This pixel shader basically says: "I don't care about anything else,
//just paint this pixel (aka "fragment") white no matter what!"

void main() {   
      gl_FragColor = gl_LightSource[0].diffuse;
}
