/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCOLORDLG_H__
#define __VCOLORDLG_H__

#include <qdockwindow.h>

class QTabWidget;
class QHButtonGroup;
class KColorPatch;
class KoMainWindow;
class KoView;
class KarbonPart;
class VColorSlider;

class VColorDlg : public QDockWindow
{
	Q_OBJECT

public:
 	 VColorDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
  
private:
	QTabWidget* mTabWidget;
	QWidget* mRGBWidget;
	KColorPatch* mColorPreview;
	QHButtonGroup *mButtonGroup;
	VColorSlider *mRedSlider;
	VColorSlider *mGreenSlider;
	VColorSlider *mBlueSlider;
	enum ButtonChoice { Outline, Fill };
	KarbonPart *m_part;
	
private slots:
	void updateRGBColorPreview();
	void buttonClicked( int button_ID );
};

#endif

