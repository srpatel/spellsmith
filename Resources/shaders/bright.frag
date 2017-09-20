#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
void main() {
	vec4 vC = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	gl_FragColor = vec4(vC.r * 4.0, vC.g * 4.0, vC.b * 4.0, vC.a);
}
