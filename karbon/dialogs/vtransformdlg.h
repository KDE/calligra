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

#ifndef __VTRANSFORMDLG_H__
#define __VTRANSFORMDLG_H__

#include <qdockwindow.h>

class QCheckBox;
class QTabWidget;
class QVButtonGroup;

class KoMainWindow;
class KoView;

class KarbonPart;
class KoUnitDoubleSpinBox;
class VReference;

enum ButtonChoice { tr_Duplicate, tr_Apply };

enum TabChoice { Translate, Rotate, Shear, Scale }; //<--- For quickjumping to a certain tab - useful for quick shortcuts

// Translate widget for this dialog...
class VTranslateWidget : public QWidget
{
	Q_OBJECT
public:
	VTranslateWidget( QWidget* parent = 0L);
private:
	QCheckBox* mRelative;
	QVButtonGroup* mButtonGroup;
	KoUnitDoubleSpinBox* mHSpinBox;
	KoUnitDoubleSpinBox* mVSpinBox;
	VReference* mReference;
};

// Rotation widget for this dialog...
class VRotateWidget : public QWidget
{
	Q_OBJECT
public:
	VRotateWidget( QWidget* parent = 0L );
private:
	QCheckBox* mRelative;
	QVButtonGroup* mButtonGroup;
	KoUnitDoubleSpinBox* mAngle;
	KoUnitDoubleSpinBox* mHSpinBox;
	KoUnitDoubleSpinBox* mVSpinBox;
	VReference* mReference;
};

// Shear widget for this dialog...
class VShearWidget : public QWidget
{
	Q_OBJECT
public:
	VShearWidget( QWidget* parent = 0L );
private:
	QVButtonGroup* mButtonGroup;
	KoUnitDoubleSpinBox* mHSpinBox;
	KoUnitDoubleSpinBox* mVSpinBox;
	VReference* mReference;
};

// Scale widget for this dialog...
class VScaleWidget : public QWidget
{
	Q_OBJECT
public:
	VScaleWidget( QWidget* parent = 0L );
private:
	QCheckBox* mRelative;
	QVButtonGroup* mButtonGroup;
	KoUnitDoubleSpinBox* mHSpinBox;
	KoUnitDoubleSpinBox* mVSpinBox;
	VReference* mReference;
};

// The dialog ...
class VTransformDlg : public QDockWindow
{
	Q_OBJECT

public:
	VTransformDlg( KarbonPart* part, KoView* parent = 0L, const char* name = 0L );
	void setTab( TabChoice m_tabChoice );
  
private:
	QTabWidget* mTabWidget;
	VTranslateWidget* mTranslateWidget;
	VRotateWidget* mRotateWidget;
	VShearWidget* mShearWidget;
	VScaleWidget* mScaleWidget;
	KarbonPart *m_part;
};

#endif

