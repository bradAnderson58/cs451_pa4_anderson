/* -------------------------------------------------------

This shader implements a spotlight per pixel using  the 
diffuse, specular, and ambient terms acoording to "Mathematics of Lighthing" 
as found in the book "OpenGL Programming Guide" (aka the Red Book)

Antonio Ramires Fernandes

--------------------------------------------------------- */

varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector, eyeVec;
varying float dist;
varying vec2 texCoord;
varying vec4 position;

void main()
{	
	vec4 ecPos;
	vec3 aux;
	
	//set the texCoord?
	texCoord = gl_MultiTexCoord0.xy;

	position = (gl_ModelViewMatrix * gl_Vertex);

	//vec3 cameraInWS = gl_Position.xyz - gl_ModelViewMatrixInverse[ 3 ].xyz;
	//eyeVec = cameraInWS - ( gl_ModelViewMatrixInverse * gl_Vertex ).xyz;
	eyeVec = vec3(0.0, 0.0, 1.0);
	/* first transform the normal into eye space and normalize the result */
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	/* now normalize the light's direction. Note that according to the
	OpenGL specification, the light is stored in eye space.*/
	ecPos = gl_ModelViewMatrix * gl_Vertex;
	aux = vec3(gl_LightSource[0].position-ecPos);
	lightDir = normalize(aux);
	
	/* compute the distance to the light source to a varying variable*/
	dist = length(aux);

	/* Normalize the halfVector to pass it to the fragment shader */
	{
		// compute position in camera space
		//vec3 pos = vec3(ecPos);
			
		// compute eye vector and normalize it 
		vec3 eye = normalize(-vec3(ecPos));

		// compute the half vector
		halfVector = normalize(lightDir + eye);
	}

	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	gl_Position = ftransform();
	
} 
