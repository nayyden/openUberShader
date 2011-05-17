#define NUM_LIGHTS 2
#define MAX_LIGHTS 6
varying vec3 lightVec[MAX_LIGHTS];
varying vec3 eyeVec;
varying vec2 texCoord;
uniform sampler2D aoMap;
uniform sampler2D normalMap;
uniform float invRadius;
uniform float fRoughness;
uniform int multiLight;
void main (void)
{
	vec4 final_color = vec4(0.0, 0.0, 0.0, 0.0);
	float att = 0.0;
	vec3 norm =  texture2D(normalMap, texCoord).xyz ;
	vec3 n = normalize( norm* 2.0 - 1.0);
	const float PI = 3.14159;
	int i;
	float base = texture2D(aoMap, texCoord).r;
	
	if(multiLight == 0) {
		float distSqr = dot(lightVec[0], lightVec[0]);
		att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
		vec3 lVec = lightVec[0] * inversesqrt(distSqr);

		vec3 vVec = normalize(eyeVec);
		vec3 R = reflect(-vVec, n);
		vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	
		float diffuse = max( dot(lVec, n), 0.0 );	
		vec4 vDiffuse = gl_FrontMaterial.diffuse * diffuse  ;	
		final_color += vDiffuse;
	} else if(multiLight == 1) {
	
		for(i = 0; i < NUM_LIGHTS; i++) {
		
			float distSqr = dot(lightVec[i], lightVec[i]);
			att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
			vec3 lVec = lightVec[i] * inversesqrt(distSqr);

			vec3 vVec = normalize(eyeVec);
			vec3 R = reflect(-vVec, n);
			vec4 vAmbient = gl_LightSource[i].ambient * gl_FrontMaterial.ambient;
			
			float diffuse = max( dot(lVec, n), 0.0 );	
			vec4 vDiffuse = gl_FrontMaterial.diffuse * diffuse  ;	
			final_color += vDiffuse + vAmbient;
		}
	}
  /*  // Compute the other aliases
    float alpha    = max( acos( dot( vVec, n ) ), acos( dot( lVec, n ) ) );
	
    float beta     = min( acos( dot(lVec, n ) ), acos( dot( vVec, n ) ) );
    float gamma    = dot( vVec - n * dot( vVec, n ), lVec- n * dot( lVec, n ) );
    float rough_sq = fRoughness * fRoughness;
 
    float C1       = 1.0 - 0.5 * ( rough_sq / ( rough_sq + 0.33 ) );
 
    float C2       = 0.45 * ( rough_sq / ( rough_sq + 0.09 ) );
    if( gamma >= 0.0 )
    {
        C2 *= sin( alpha );
    }
    else
    {
        C2 *= ( sin( alpha ) - pow( (2.0 * beta) / PI, 3.0 ) );
    }
 
    float C3  = (1.0 / 8.0) ;
    C3       *= ( rough_sq / ( rough_sq + 0.09 ) );
    C3       *= pow( ( 4.0 * alpha * beta ) / (PI * PI), 2.0 );
 
    float A = gamma * C2 * tan( beta );
    float B = (1.0 - abs( gamma )) * C3 * tan( (alpha + beta) / 2.0 );
 
   vec3 final =  base *  
		gl_FrontMaterial.diffuse.xyz * max( 0.0, dot( n, lVec) ) * ( C1 + A + B );

	gl_FragColor = vec4(final, 1.0); */
    	
	gl_FragColor = (final_color ) * att ; // comment this for oren nayar
	
	
}