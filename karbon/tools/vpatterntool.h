/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#ifndef __VPATTERNTOOL_H__
#define __VPATTERNTOOL_H__

#include <qframe.h>

#include "vtool.h"
#include "vpattern.h"

class QHButtonGroup;
class QToolButton;
class KarbonView;
class KoIconChooser;

class VPatternWidget : public QFrame
{
Q_OBJECT

public:
	VPatternWidget( QPtrList<KoIconItem>* patterns, VTool* tool, QWidget* parent = 0L );
	~VPatternWidget();

	VPattern* selectedPattern();

public slots:
	void importPattern();
	void deletePattern();

	void slotButtonClicked( int id );
	void patternSelected( KoIconItem* );

private:
	KoIconChooser*        m_patternChooser;
	QHButtonGroup*        m_buttonGroup;
	QToolButton*          m_importPatternButton;
	QToolButton*          m_deletePatternButton;
	VTool*                m_tool;
	VPattern*             m_pattern;
}; // VPatternWidget


class VPatternTool : public VTool
{
public:
	VPatternTool( KarbonView* view );
	virtual ~VPatternTool();

	virtual void activate();

	virtual QString name() { return i18n( "Pattern tool" ); }
	virtual QString contextHelp();
	virtual QWidget* optionsWidget() { return m_optionsWidget; }

protected:
	virtual void draw();

	virtual void mouseButtonRelease();
	virtual void mouseButtonPress();
	virtual void mouseDragRelease();
	virtual void mouseDrag();
	/*virtual void mouseDragShiftPressed(); // To use to scale the pattern.
	virtual void mouseDragShiftReleased(); */
	virtual void cancel();

private:
	KoPoint			m_current;
	VPatternWidget*	m_optionsWidget;
}; // VPatternTool

#endif /* __VPATTERNTOOL_H__ */

