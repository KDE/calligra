/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
class KIntNumInput;
class VColorSlider;

class VColorDlg : public QDockWindow
{
	Q_OBJECT

public:
 	 VColorDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
  
private:
	QTabWidget* mTabWidget;
	QWidget* mRGBWidget;
	QWidget* mCMYKWidget;
	KColorPatch* mRGBColorPreview;
	KColorPatch* mCMYKColorPreview;
	QHButtonGroup *mRGBButtonGroup;
	QHButtonGroup *mCMYKButtonGroup;
	VColorSlider *mRedSlider;
	VColorSlider *mGreenSlider;
	VColorSlider *mBlueSlider;
	VColorSlider *mCyanSlider;
	VColorSlider *mMagentaSlider;
	VColorSlider *mYellowSlider;
	VColorSlider *mBlackSlider;
	KIntNumInput* mRGBOpacity;
	KIntNumInput* mCMYKOpacity;
	enum ButtonChoice { Outline, Fill };
	KarbonPart *m_part;
	
private slots:
	void updateRGBColorPreview();
	void updateCMYKColorPreview();
	void buttonRGBClicked( int button_ID );
	void buttonCMYKClicked( int button_ID );
};

#endif

