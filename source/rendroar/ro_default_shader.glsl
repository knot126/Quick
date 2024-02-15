varying vec2 fTextureCoords;
varying vec4 fColour;

#ifdef VERTEX
attribute vec3 inPosition;
attribute vec2 inTextureCoords;
attribute vec4 inColour;

void main() {
	gl_Position = vec4(inPosition, 1.0);
	fTextureCoords = inTextureCoords;
	fColour = inColour;
}
#endif

#ifdef FRAGMENT
void main() {
	gl_FragColor = fColour;
}
#endif
