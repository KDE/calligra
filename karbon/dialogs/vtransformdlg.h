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

class VTransformDlg : public QDockWindow
{
	Q_OBJECT

public:
 	 VTransformDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
  
private:
	//QTabWidget* mTabWidget;
	//QWidget* mRGBWidget;
	enum ButtonChoice { tr_Duplicate, tr_Apply };
	KarbonPart *m_part;
};

#endif

