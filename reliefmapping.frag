/* -------------------------------------------------------

This shader implements a spotlight per pixel using  the 
diffuse, specular, and ambient terms acoording to "Mathematics of Lighthing" 
as found in the book "OpenGL Programming Guide" (aka the Red Book)

Antonio Ramires Fernandes

--------------------------------------------------------- */

uniform sampler2D color_texture;
uniform sampler2D norml_map;
uniform sampler2D relief_map;

varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector, eyeVec;
varying float dist;
varying vec2 texCoord;
varying vec3 vertex;
varying vec4 position;


//Auto magic tangent space http://www.geeks3d.com/20130122/normal-mapping-without-precomputed-tangent-space-vectors/
mat3 calcTBN(vec3 pos, vec2 texcoord, vec3 normal) {
	// compute derivations of the world position
	vec3 p_dx = dFdx(pos);
	vec3 p_dy = dFdy(pos);
	// compute derivations of the texture coordinate
	vec2 tc_dx = dFdx(texcoord);
	vec2 tc_dy = dFdy(texcoord);
	// compute initial tangent and bi-tangent
	vec3 t = normalize( tc_dy.y * p_dx - tc_dx.y * p_dy );
	vec3 b = normalize( tc_dy.x * p_dx - tc_dx.x * p_dy ); // sign inversion
	// get new tangent from a given mesh normal
	vec3 n = normalize(normal);
	vec3 x = cross(n, t);
	t = cross(x, n);
	t = normalize(t);
	// get updated bi-tangent
	x = cross(b, n);
	b = cross(n, x);
	b = normalize(b);
	return mat3(t, b, n);
}


void main()
{

	//This is my eye vector
	vec3 eye = eyeVec;
	vec2 tempCoord = texCoord;  //neccessary?

	//get depth of the relief map
	float depth = texture2D(relief_map, tempCoord).x * -0.025;

	//compute TBN matrix
	mat3 tangSpace = calcTBN(position.xyz, tempCoord, normal);

	//compute offset
	vec3 offset = tangSpace * eye * depth;

	float depth_at_1 = texture2D(relief_map, tempCoord + offset.xy).x;
	if (depth_at_1 < 0.96){
		offset *= (depth_at_1 + (texture2D(relief_map, tempCoord).x)) * 0.5;
	}

	tempCoord += offset.xy;


	//assign texture color
	vec4 texColor = texture2D(color_texture, tempCoord);

	//normal sample * tangent space equals new normal
	vec3 normSamp = ( texture2D(norml_map, tempCoord).xyz * 2.0) - 1.0;
	vec3 norml = normalize(tangSpace * normSamp);


	vec3 n,halfV;
	float NdotL,NdotHV;
	vec4 color = vec4(0); //ambientGlobal;
	float att,spotEffect;
	int get_light=0;

	/* a fragment shader can't write a verying variable, hence we need
	a new variable to store the normalized interpolated normal */
	n = normalize(norml);
	
	/* compute the dot product between normal and ldir */
	NdotL = max(dot(n,normalize(lightDir)),0.0);


	if (NdotL > 0.0) 
	{
		spotEffect = dot(normalize(gl_LightSource[0].spotDirection), normalize(-lightDir));
		
		float threshold = cos(gl_LightSource[0].spotCosCutoff); //spotCosCutoff is in radian

		if (spotEffect >threshold ) 
		{
			spotEffect = pow(spotEffect, gl_LightSource[0].spotExponent);
			att = spotEffect / (gl_LightSource[0].constantAttenuation +
			                    gl_LightSource[0].linearAttenuation * dist +
		                        gl_LightSource[0].quadraticAttenuation * dist * dist);


			color += att * (diffuse * NdotL + ambient);

			halfV = normalize(halfVector);
			NdotHV = max(dot(n,halfV),0.0);
			color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
			get_light=1;
		}
	}

	if(get_light==0)
	{		
		att = 0.1 / (gl_LightSource[0].constantAttenuation +
			         gl_LightSource[0].linearAttenuation * dist +
		             gl_LightSource[0].quadraticAttenuation * dist * dist);		
		color = att * ambient;
	}

	gl_FragColor = color * texColor + .05;
}
