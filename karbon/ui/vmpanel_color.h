/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef VMPANEL_COLOR_H
#define VMPANEL_COLOR_H

#include <qdockwindow.h>

class QSpinBox;
class QTabWidget;
class KColorPatch;
class KoMainWindow;
class KoView;

class VColorPanel : public QDockWindow
{
  Q_OBJECT
public:
 	 VColorPanel( KoView* parent = 0L, const char* name = 0L );
  
private:
	QTabWidget* mTabWidget;
	QWidget* mRGBWidget;
	QSpinBox* mRed;
	QSpinBox* mGreen;
	QSpinBox* mBlue;
	KColorPatch* mColorPreview;
};

#endif
