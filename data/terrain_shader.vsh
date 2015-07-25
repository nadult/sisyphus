#version 100

uniform mat4 proj_view_matrix;
uniform vec4 mesh_color;
attribute vec3 in_pos;
attribute vec2 in_tex_coord;
varying vec2 tex_coord;
varying vec4 color;

void main() {
	gl_Position = proj_view_matrix * vec4(in_pos, 1.0);
	tex_coord = in_tex_coord;
	color = mesh_color;
}
