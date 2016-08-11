varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;

void main()
{
	vec4 tex = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	float gray = dot(tex.rgb, vec3(0.299, 0.587, 0.114));
	
	gl_FragColor = vec4(
		gray,
		gray,
		gray,
		tex.a);
}