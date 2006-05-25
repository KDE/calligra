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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VTRANSLATE_H__
#define __VTRANSLATE_H__

#include <QWidget>
//Added by qt3to4:
#include <QLabel>

class QCheckBox;
class QLabel;
class QPushButton;
class QString;
class KDoubleNumInput;

class VTranslate : public QWidget
{
	Q_OBJECT

public:
	VTranslate( QWidget* parent = 0L, const char* name = 0L );
	~VTranslate();

public slots:
	//sets the unit labels do display correct text (mm, cm, pixels etc):
	void setUnits( const QString& units );

private:
	QLabel* m_labelX;
	KDoubleNumInput* m_inputX; //X coordinate
	QLabel* labely;
	KDoubleNumInput* m_inputY; //Y coordinate
	QLabel* m_labelUnit1;
	QLabel* m_labelUnit2;
	QCheckBox* m_checkBoxPosition; //If checked, displays coordinates of selected object
	QPushButton* m_buttonDuplicate; //duplicate (makes a copy of selected object(s) with a new position)
	QPushButton* m_buttonApply; //apply new position
};

#endif

