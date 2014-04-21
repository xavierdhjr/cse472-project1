#include "StdAfx.h"
#include "Box.h"

CBox::CBox(double size)
{
	m_texture = NULL;
	// Vertices of a unit Box centered at origin, sides aligned with axes
	vertex_positions[0] = point4( -size * 2.0, -size,  size, 1.0 );
	vertex_positions[1] = point4( -size * 2.0,  size,  size, 1.0 );
	vertex_positions[2] = point4(  size * 2.0,  size,  size, 1.0 );
	vertex_positions[3] = point4(  size * 2.0, -size,  size, 1.0 );
	vertex_positions[4] = point4( -size * 2.0, -size, -size, 1.0 );
	vertex_positions[5] = point4( -size * 2.0,  size, -size, 1.0 );
	vertex_positions[6] = point4(  size * 2.0,  size, -size, 1.0 );
	vertex_positions[7] = point4(  size * 2.0, -size, -size, 1.0 );


	int Index = 0;

	quad( 1, 0, 3, 2 , vertex_positions, Index);
	quad( 2, 3, 7, 6 ,  vertex_positions, Index);
	quad( 3, 0, 4, 7 ,  vertex_positions, Index);
	quad( 6, 5, 1, 2 ,  vertex_positions, Index);
	quad( 4, 5, 6, 7 ,  vertex_positions, Index);
	quad( 5, 4, 0, 1 ,  vertex_positions, Index);

	//Initialize the motion
	v = vec3(15.0, 0., 0.);
	w = vec3(1., 1., 1.);
}

CBox::~CBox(void)
{
}

