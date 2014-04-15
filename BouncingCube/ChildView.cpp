
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "BouncingCube.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_bTimer = false;
	m_nTimer = -1;
	m_fT = 0.f;
	m_cube = new CCube(0.5);
	m_wall = new CCube(10);
}

CChildView::~CChildView()
{
	delete m_cube;
	delete m_wall;
}


BEGIN_MESSAGE_MAP(CChildView, CShaderWnd)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CShaderWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}


const int  TextureSize  = 64;

// Texture objects and storage for texture image
GLuint textures[2];
GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];
//vec2   tex_coords[NumVertices];
typedef  vec4  point4;typedef  vec4  color4;

void CChildView::InitGL()
{
	// Create a checkerboard pattern
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			GLubyte c;
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			image[i][j][0]  = c;
			image[i][j][1]  = c;
			image[i][j][2]  = c;
		}
	}
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			GLubyte c;
			c = (((i & 0x16) == 0) | ((j & 0x16) == 0)) * 255;
			image2[i][j][0] = c;
			image2[i][j][1] = 0;
			image2[i][j][2] = c;
		}
	}

	m_program = LoadShaders( "ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl" );


	//GLuint vertexbuffer;
	glUseProgram(m_program);

	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize,TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize,TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textures[0]);
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, textures[1]);



	m_cube->InitGL(m_program);
	m_wall->InitGL(m_program);

	point4 light_position (-5.f, 5.f, -5.f, 0.f);
	color4 light_ambient (0.2f, 0.2f, 0.2f, 1.f);
	color4 light_diffuse (1.f, 1.f, 1.f, 1.f);
	color4 light_specular (1.f, 1.f, 1.f, 1.f);

	color4 material_ambient(.3f, .6f, .3f, 1.f);
	color4 material_diffuse (0.3f, .6f, 0.3f, 1.f);
	color4 material_specular (1.f, 1.f, 1.f, 1.f);
	float material_shininess = 100.0f;

	color4 ambient_product = light_ambient*material_ambient;
	color4 diffuse_product = light_diffuse*material_diffuse;
	color4 specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(m_program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(m_program, "SpecularProduct"), 1, value_ptr(specular_product));
	glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, value_ptr(light_position));
	glUniform1f(glGetUniformLocation(m_program, "Shininess"), material_shininess);

	//Set the value of the fragment shader texture sampler variable
	//	("texture") to the the appropriate texture unit. In this case,
	//	zero, for GL_TEXTURE0 which was previously set by calling
	//	glActiveTexture().
	glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 0);


	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	glClearColor(1.f,1.f,1.f,1.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CChildView::RenderGL()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mPVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(m_mVM));

	glUniform1f(glGetUniformLocation(m_program,"t"), m_fT);

	color4 light_ambient (0.2f, 0.2f, 0.2f, 1.f);
	color4 material_ambient(.3f, .6f, .3f, 1.f);
	color4 material_transpartent(.3f, .6f, .3f, 0.1f);

	color4 ambient_product = light_ambient*material_ambient;

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 0);
	m_cube->RenderGL(m_program);
	ambient_product = light_ambient*material_transpartent;
	glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 1);
	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	m_wall->RenderGL(m_program);
}

void CChildView::CleanGL()
{

}



void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case 'r':
	case 'R':
		ResetMatrix();
		break;
	case VK_SPACE:
		if (m_nTimer==-1) {
			m_nTimer = SetTimer(1, 40, NULL);
		} else {
			KillTimer(m_nTimer);
			m_nTimer = -1;
		}
	}
	CShaderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	m_cube->Update(0.04);
	Invalidate();

	CShaderWnd::OnTimer(nIDEvent);
}
