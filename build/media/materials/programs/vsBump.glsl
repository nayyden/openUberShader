
varying vec3 lightVec; 


varying vec3 eyeVec;

varying vec2 texCoord;



attribute vec3 tangent; 
					 

void main(void)
{

	gl_Position = ftransform();

	texCoord = gl_MultiTexCoord0.xy;
	
	
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * tangent);
	vec3 b = (cross(n, t));
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 lightPos = gl_LightSource[0].position.xyz;
	
	vec3 tmpVec =   lightPos - vVertex;
	
	lightVec.x = dot(tmpVec, t);
	lightVec.y = dot(tmpVec, b);
	lightVec.z = dot(tmpVec, n);
	

	tmpVec = -vVertex;
	eyeVec.x = dot(tmpVec, t);
	eyeVec.y = dot(tmpVec, b);
	eyeVec.z = dot(tmpVec, n);
}