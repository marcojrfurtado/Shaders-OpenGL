uniform sampler2D textureSample_0;
uniform sampler2D textureSample_1;

varying vec2 rgbTexCoord;

void main() {


	vec4 texel0, texel1_ori, texel1_scaled, texel1_res, result;

	texel0 = texture2D(textureSample_0,rgbTexCoord);

	texel1_ori = texture2D(textureSample_1,gl_TexCoord[0].st);
	texel1_scaled = texture2D(textureSample_1,rgbTexCoord);
	texel1_res = vec4(texel1_scaled.x,texel1_scaled.y,texel1_scaled.z,texel1_ori.a);


	result = mix(texel0,texel1_res,texel1_res.a);

	gl_FragColor = result ;
    
}
