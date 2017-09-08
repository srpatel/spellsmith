#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_greyness;
uniform int u_direction;

void main()
{
	float ratio = u_greyness;
	if (ratio > 1.0) {
		ratio = 1.0;
	}
	
	if (u_direction < 0) {
		ratio = 1.0 - ratio;
	}
	
	vec4 tex = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	float gray = dot(tex.rgb, vec3(0.299, 0.587, 0.114));
	
	gl_FragColor = tex * (1.0 - ratio) + vec4(gray, gray, gray, tex.a) * ratio;
}
