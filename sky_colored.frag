#ifdef OPENGL_ES
precision highp float;
#endif

// Varyings
varying vec3 v_color;						// Input Vertex color ( r g b )

void main()
{
	gl_FragColor.rgb = v_color;
}
