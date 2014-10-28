//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#include "main.h"

//
//
//
//
//   The MAIN function
//
//
//
//

int main(int argc, char ** argv)
{
    //
    if(!parseArg(argc,argv)){
        printUsage(argv[0]);
        return 1;
    }

    //read obj vile
    readfromfiles();
    //

    /////////////////////////////////////////////////////////////////
    //setup glut/gli
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH );
    glutInitWindowSize( 680, 680);
    glutInitWindowPosition( 50, 50 );
	string title = "CS451 PA4 Relief Mapping: ";
	switch (cs451_pa4_render_type)
	{
		case PA4_SPOTLIGHT_RENDERING: title += "Spotlight Rendering"; break;
		case PA4_TEXTURE_RENDERING: title += "Texture Rendering"; break;
		case PA4_NORAMALMAP_RENDERING: title += "Normal Mapping"; break;
		case PA4_RELIEFMAP_RENDERING: title += "Relif Mapping"; break;
	}
	glutCreateWindow(title.c_str());

    InitGL();
    gli::gliInit();
    gli::gliDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    //set camera position
    gli::setCameraPosZ(R*2.1f);

	//load textures
	if (!color_texture_filename.empty())
	{
		if (color_texture.loadTexture(color_texture_filename.c_str()) == false)
		{
			cerr << "! Error: Failed to load texture:" << color_texture_filename << endl;
			return 1;
		}
	}

	if (!normalmap_texture_filename.empty())
	{
		if (normalmap_texture.loadTexture(normalmap_texture_filename.c_str()) == false)
		{
			cerr << "! Error: Failed to load texture:" << normalmap_texture_filename << endl;
			return 1;
		}
	}

	if (!reliefmap_texture_filename.empty())
	{
		if (reliefmap_texture.loadTexture(reliefmap_texture_filename.c_str()) == false)
		{
			cerr << "! Error: Failed to load texture:" << reliefmap_texture_filename << endl;
			return 1;
		}
	}

    /////////////////////////////////////////////////////////////
    gli::gliMainLoop();

    return 0;
}

