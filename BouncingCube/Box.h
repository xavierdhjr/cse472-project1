#pragma once
#include "ShaderWnd/ShaderWnd.h"
#include "graphics/GrTexture.h"
#include "Cube.h"


class CBox : public CCube
{

public:
	CBox(double size=1.);
	~CBox(void);
	
};

const vec3 operator* (double dt, vec3 v); 
const quat operator+ (quat, quat);
const quat operator* (vec3 v, quat q);
const quat operator* (float s, quat q);