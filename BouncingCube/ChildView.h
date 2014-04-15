
// ChildView.h : interface of the CChildView class
//


#pragma once
#include "ShaderWnd/ShaderWnd.h"
#include "Cube.h"
// CChildView window

class CChildView : public CShaderWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	CCube *m_cube, *m_wall;
	bool m_bTimer;
	int m_nTimer;
	float m_fT;

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();
	virtual void InitGL();
	virtual void RenderGL();
	virtual void CleanGL();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

