#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform sampler2D u_texture0;
varying vec4 v_texCoord0;

void main(void)
{
    gl_FragColor = texture2D(u_texture0, v_texCoord0.st);
}
