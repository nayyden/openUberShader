varying vec3 lightVec;
varying vec3 eyeVec;
varying vec2 texCoord;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform float invRadius;
uniform float fRoughness;
void main (void)
{
	const float PI = 3.14159;
	float distSqr = dot(lightVec, lightVec);
	float att = clamp( invRadius * sqrt(distSqr), 0.0, 1.0);
	vec3 lVec = lightVec *inversesqrt(distSqr);
	
	

	vec3 vVec = normalize(eyeVec);
	
	vec4 base = texture2D(colorMap, texCoord);
	
	vec3 n = normalize( texture2D(normalMap, texCoord).xyz * 2.0 - 1.0);
	//vec3 bump = normalize( texture2D(normalMap, texCoord).xyz );
	vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

	float diffuse = max(  0.0, dot(n, lVec) );
	
	vec4 vDiffuse =  gl_FrontMaterial.diffuse *
					diffuse;	

	
 
    // Compute the other aliases
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
 
   vec3 final =   gl_FrontMaterial.diffuse.xyz * max( 0.0, dot( n, lVec) ) * ( C1 + A + B );
 
 
	float specular = pow(clamp(dot(reflect(-lVec, n), vVec), 0.0, 1.0), 
	                 gl_FrontMaterial.shininess );
	
	vec4 vSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * 
					 specular;
	
	
	gl_FragColor = (vec4(final, 1.0) + (vSpecular  - 0.2 ))*att; 
    	
	gl_FragColor = (vDiffuse )*att;	// comment this for oren nayar
	
	
	
}