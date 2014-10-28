//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#pragma once

#include "objReader.h"
#include "model.h"
#include "texture.h"

#include <list>
#include <float.h>
using namespace std;

//-----------------------------------------------------------------------------
// INPUTS
list<string> input_filenames; //model name

//texture filenames
string color_texture_filename;
string normalmap_texture_filename;
string reliefmap_texture_filename;


//-----------------------------------------------------------------------------
// Intermediate data
list<model> models;
float R=0;       //radius
Point3d COM;     //center of mass

//
// New in CS451 PA4
//
Texture color_texture("color_texture", GL_TEXTURE0);
Texture normalmap_texture("norml_map", GL_TEXTURE1);
Texture reliefmap_texture("relief_map", GL_TEXTURE2);

//this is used to determine which shader is used.
enum CS451_PA4_RENDERING_TYPE { PA4_SPOTLIGHT_RENDERING, PA4_TEXTURE_RENDERING, PA4_NORAMALMAP_RENDERING, PA4_RELIEFMAP_RENDERING };
//this value is determined by texture filenames above. See parseArg function for detail
CS451_PA4_RENDERING_TYPE cs451_pa4_render_type = PA4_SPOTLIGHT_RENDERING;

//-----------------------------------------------------------------------------
//read M+ from file
bool readfromfile();
void computeCOM_R();

//-----------------------------------------------------------------------------
bool parseArg(int argc, char ** argv)
{
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-')
        {
			string tmp(argv[i]);
			if (tmp == "-texture") color_texture_filename = argv[++i];
			else if (tmp == "-normalmap") normalmap_texture_filename = argv[++i];
			else if (tmp == "-reliefmap") reliefmap_texture_filename = argv[++i];
            else return false; //unknown
        }
        else{
            input_filenames.push_back(argv[i]);
        }
    }

	//determine the type of rendering based on what is given
	if (!color_texture_filename.empty())
	{
		cs451_pa4_render_type = PA4_TEXTURE_RENDERING;
	}

	if (!normalmap_texture_filename.empty())
	{
		if (color_texture_filename.empty())
		{
			cerr << "! Errr: Normal mapping require both normal map and texture map" << endl;
			return false;
		}

		cs451_pa4_render_type = PA4_NORAMALMAP_RENDERING;
	}

	if (!reliefmap_texture_filename.empty())
	{
		if (color_texture_filename.empty() || normalmap_texture_filename.empty())
		{
			cerr << "! Errr: Relief mapping require displacement (height) map, normal map and texture map" << endl;
			return false;
		}

		cs451_pa4_render_type = PA4_RELIEFMAP_RENDERING;
	}

    return true;
}

void printUsage(char * name)
{
    //int offset=20;
    cerr<<"Usage: "<<name<<" [options] *.obj \n"
        <<"options: -texture filename -normalmap filename -reliefmap filename \n\n";
    cerr<<"\n-- Report bugs to: Jyh-Ming Lien jmlien@gmu.edu"<<endl;
}

//-----------------------------------------------------------------------------

bool readfromfiles()
{
    long vsize=0;
    long fsize=0;

    uint id=0;
    for(list<string>::iterator i=input_filenames.begin();i!=input_filenames.end();i++,id++){
        cout<<"- ["<<id<<"/"<<input_filenames.size()<<"] Start reading "<<*i<<endl;
        model m;
        if(!m.build(*i)) continue;
        cout<<"- ["<<id<<"/"<<input_filenames.size()<<"] Done reading "<<m.v_size<<" vertices and "<<m.t_size<<" facets"<<endl;
        vsize+=m.v_size;
        fsize+=m.t_size;
        models.push_back(m);
    }
    cout<<"- Total: "<<vsize<<" vertices, "<<fsize<<" triangles, and "<<input_filenames.size()<<" models"<<endl;
    computeCOM_R();

    return true;
}

void computeCOM_R()
{
    //compute a bbox
    double box[6]={FLT_MAX,-FLT_MAX,FLT_MAX,-FLT_MAX,FLT_MAX,-FLT_MAX};
    //-------------------------------------------------------------------------
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        for(unsigned int j=0;j<i->v_size;j++){
            Point3d& p=i->vertices[j].p;
            if(p[0]<box[0]) box[0]=p[0];
            if(p[0]>box[1]) box[1]=p[0];
            if(p[1]<box[2]) box[2]=p[1];
            if(p[1]>box[3]) box[3]=p[1];
            if(p[2]<box[4]) box[4]=p[2];
            if(p[2]>box[5]) box[5]=p[2];
        }//j
    }//i

    //-------------------------------------------------------------------------
    // compute center of mass and R...
    COM.set( (box[1]+box[0])/2,(box[3]+box[2])/2,(box[5]+box[4])/2);

    //-------------------------------------------------------------------------
	R=0;
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        for(unsigned int j=0;j<i->v_size;j++){
            Point3d& p=i->vertices[j].p;
            float d=(float)(p-COM).normsqr();
            if(d>R) R=d;
        }//j
    }//i

    R=sqrt(R);
}

//-----------------------------------------------------------------------------
//
//
//
//  Open GL stuff below
//
//
//-----------------------------------------------------------------------------

#include <draw.h>



