/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VPATTERNTOOL_H__
#define __VPATTERNTOOL_H__

#include <kdialogbase.h>

#include "vtool.h"
#include "vpattern.h"

class QHButtonGroup;
class QToolButton;
class KoIconChooser;

class VPatternWidget : public KDialogBase
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
	VPatternTool( KarbonPart *part );
	virtual ~VPatternTool();

	virtual void activate();

	virtual QString name() { return i18n( "Pattern Tool" ); }
	virtual QString icon() { return "14_pattern"; }
	virtual QString contextHelp();
	virtual bool showDialog() const;

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

