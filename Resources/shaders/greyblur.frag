#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	float delta = (CC_Time.y - u_start_time) / 2.0;
	float ratio = delta;
	if (ratio > 1.0) {
		ratio = 1.0;
	}
	
	if (u_direction < 0) {
		ratio = 1.0 - ratio;
	}
	
	float blur = 1.0;
	float hstep = 1.0;
    float vstep = 0.0;
	
	vec2 tc = v_texCoord;
	vec4 tex = vec4(0.0);
	
	tex += texture2D(CC_Texture0, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
    tex += texture2D(CC_Texture0, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    tex += texture2D(CC_Texture0, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    tex += texture2D(CC_Texture0, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

    tex += texture2D(CC_Texture0, vec2(tc.x, tc.y)) * 0.2270270270;

    tex += texture2D(CC_Texture0, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    tex += texture2D(CC_Texture0, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    tex += texture2D(CC_Texture0, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    tex += texture2D(CC_Texture0, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

	
	//tex += v_fragmentColor * tex;
	
	vec4 orig = texture2D(CC_Texture0, vec2(tc.x, tc.y));
	
	float gray = dot(tex.rgb, vec3(0.299, 0.587, 0.114));
	
	//gl_FragColor = tex * (1.0 - ratio) + vec4(gray, gray, gray, tex.a) * ratio;
	gl_FragColor = v_fragmentColor * vec4(tex.rgb, orig.a);
}
