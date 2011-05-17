#define NUM_LIGHTS 2
#define MAX_LIGHTS 6



varying vec3 lightVec[MAX_LIGHTS];

varying vec3 eyeVec;
varying vec2 texCoord;
attribute vec3 tangent; 
uniform int multiLight;

void main(void)
{
	gl_Position = ftransform();
	texCoord = gl_MultiTexCoord0.xy;
	
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * tangent);
	vec3 b = normalize(cross(n, t));

	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 tmpVec;

	if(multiLight == 0) {
		tmpVec = normalize(gl_LightSource[0].position.xyz - vVertex);
		lightVec[0].x = dot(tmpVec, t);
		lightVec[0].y = dot(tmpVec, b);
		lightVec[0].z = dot(tmpVec, n);
	} else if (multiLight == 1) {
		int i;
		for(i = 0; i < NUM_LIGHTS; i++) {
			 tmpVec = normalize(gl_LightSource[i].position.xyz - vVertex);		
			lightVec[i].x = dot(tmpVec, t);
			lightVec[i].y = dot(tmpVec, b);
			lightVec[i].z = dot(tmpVec, n);
		}
	}
		
	

	
	tmpVec = -vVertex;
	eyeVec.x = dot(tmpVec, t);
	eyeVec.y = dot(tmpVec, b);
	eyeVec.z = dot(tmpVec, n);
}