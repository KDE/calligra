/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTRANSFORMDLG_H__
#define __VTRANSFORMDLG_H__

#include <qdockwindow.h>

class QTabWidget;
class QHButtonGroup;
class KoMainWindow;
class KoView;
class KarbonPart;

// Translate widget for this dialog...
class VTranslateWidget : public QWidget
{
	Q_OBJECT
public:
	VTranslateWidget( QWidget* parent );
};

// Rotation widget for this dialog...
class VRotateWidget : public QWidget
{
	Q_OBJECT
public:
	VRotateWidget( QWidget* parent );
};

// Shear widget for this dialog...
class VShearWidget : public QWidget
{
	Q_OBJECT
public:
	VShearWidget( QWidget* parent );
};

// Scale widget for this dialog...
class VScaleWidget : public QWidget
{
	Q_OBJECT
public:
	VScaleWidget( QWidget* parent );
};

// The dialog ...
class VTransformDlg : public QDockWindow
{
	Q_OBJECT

public:
	enum TabChoice {
		Translate,
		Rotate,
		Shear,
		Scale
	}; //<--- For quickjumping to a certain tab - useful for quick shortcuts
	
	VTransformDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
  
private:
	QTabWidget* mTabWidget;
	VTranslateWidget* mTranslateWidget;
	enum ButtonChoice { tr_Duplicate, tr_Apply };
	KarbonPart *m_part;
};

#endif

