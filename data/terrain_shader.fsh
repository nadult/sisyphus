#version 100

uniform sampler2D main_tex;
uniform sampler2D detail_tex0, detail_tex1;

varying lowp vec4 color;
varying mediump vec2 tex_coord;

mediump vec3 rgb2hsv(mediump vec3 c)
{
    mediump vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    mediump vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    mediump vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    mediump float d = q.x - min(q.w, q.y);
    mediump float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

mediump vec3 hsv2rgb(mediump vec3 c)
{
    mediump vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    mediump vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	gl_FragColor = texture2D(main_tex, tex_coord);
}

/*
void main() {
	lowp vec4 main_color = texture2D(main_tex, tex_coord);
	lowp vec4 detail0 = texture2D(detail_tex0, tex_coord * 10.0);
	lowp vec4 detail1 = texture2D(detail_tex1, tex_coord * 10.0);

	mediump vec3 X = rgb2hsv(main_color.xyz);
	mediump vec3 A = rgb2hsv(vec3(220, 211, 180) / 255.0);
	mediump vec3 B = rgb2hsv(vec3(123, 140, 67) / 255.0);

	mediump vec2 D = (X.xy - B.xy) / (A.xy - B.xy);
	mediump float d = (D.x + D.y) / 2.0;

//	gl_FragColor = vec4(d, d, d, 1.0);
	gl_FragColor = color * (detail1 * d + detail0 * (1.0 - d)) * X.z * 1.2;

	//	gl_FragColor = color * texture2D(main_tex, tex_coord) * texture2D(detail_tex[0], tex_coord *
	// 10.0);
}*/
