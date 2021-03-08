attribute vec4 a_position;
attribute vec4 a_color;

uniform mat4 u_MVP;

varying vec4 v_color;

void main(void)
{
	gl_Position = u_MVP * a_position;
	v_color = a_color;
}
