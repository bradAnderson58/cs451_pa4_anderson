#pragma once

#if ( (defined(__MACH__)) && (defined(__APPLE__)) )   
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
//#include <GL/glext.h>
#endif


#include <string>
using namespace std;


#include "SOIL.h"

class Texture
{
public:

	Texture(const string& name, GLenum unit)
	{
		texture_name = name;
		texture_unit=unit;
		texture_id=-1;
	}

	bool loadTexture(const char * filename)
	{
		unsigned int flag=SOIL_FLAG_POWER_OF_TWO
			| SOIL_FLAG_MIPMAPS
			//| SOIL_FLAG_COMPRESS_TO_DXT
			| SOIL_FLAG_TEXTURE_REPEATS
			//| SOIL_FLAG_INVERT_Y
			| SOIL_FLAG_DDS_LOAD_DIRECT;

		texture_id=SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flag);

		if (texture_id == 0) return false;
		return true;
	}

	bool loadTexture(const char * filename, int width, int height)
	{
		unsigned char * data;
		FILE * file;

		//The following code will read in our RAW file
		file = fopen(filename, "rb");

		if (file == NULL) return false;
		data = (unsigned char *)malloc(width * height * 3);
		fread(data, width * height * 3, 1, file);

		fclose(file);
		
		glGenTextures(1, &texture_id); //generate the texture with the loaded data
		glBindTexture(GL_TEXTURE_2D, texture_id); //bind the texture to it¡¦s array

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //set texture environment parameters

		//And if you go and use extensions, you can use Anisotropic filtering textures which are of an
		//even better quality, but this will do for now.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Here we are setting the parameter to repeat the texture instead of clamping the texture
		//to the edge of our shape.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		free(data); //free the texture

		return true;
	}

	void freeTexture(GLuint texture)
	{
		glDeleteTextures(1, &texture);
	}

	void bind(int program)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(texture_unit);
		int texture_location = glGetUniformLocation(program, texture_name.c_str());
		glUniform1i(texture_location, texture_unit - GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	void unbind()
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

	GLuint getTextureID() const { return texture_id; }

private:

	string texture_name;  //this name will be used to access GLSL sampler2d in the shader
	GLenum texture_unit; 
	GLuint texture_id;

};