varying vec3 v_V;
varying vec3 v_N;

uniform float time;

uniform vec2 freq;
uniform vec2 scale;

void main () {
	float wiggleX = sin(gl_Vertex.x * freq.x + time) + scale.x;
	float wiggleY = cos(gl_Vertex.y * freq.y + time) * scale.y;

	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x + wiggleY, gl_Vertex.y + wiggleX, gl_Vertex.z, gl_Vertex.w);
	v_V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	v_N = gl_NormalMatrix * gl_Normal;
}