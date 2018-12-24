
varying vec2 f_texcoord;
uniform sampler2D mytexture;

void main(void) {
	vec2 flipped_texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);	//reverses the texture so it isn't upside down consider removing this and just write the texcoords the other way
	gl_FragColor = texture2D(mytexture, flipped_texcoord);
}