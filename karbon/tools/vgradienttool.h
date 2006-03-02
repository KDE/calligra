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

#ifndef __VGRADIENTTOOL_H__
#define __VGRADIENTTOOL_H__

#include <kdialogbase.h>

#include "vtool.h"
#include "vgradient.h"
#include "KoRect.h"

class VGradientTabWidget;

class VGradientTool : public VTool
{
	Q_OBJECT

public:
	VGradientTool( KarbonView *view );
	virtual ~VGradientTool();

	virtual void activate();
	virtual void deactivate();

	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Gradient Tool" ); }
	virtual QString statusText();

	virtual QString contextHelp();
	virtual bool showDialog() const;

	virtual void draw( VPainter* painter );

protected:

	virtual void draw();
	virtual void mouseButtonRelease();
	virtual void mouseButtonPress();
	virtual void mouseDragRelease();
	virtual void mouseDrag();
	virtual void cancel();
	virtual void setCursor() const;
	virtual bool keyReleased( Qt::Key key );

	/**
	 * Determines the actual gradient to be edited.
	 *
	 * @param gradient the found gradient
	 * @return true if gradient was found, else false
	 */
	bool getGradient( VGradient &gradient );

	bool getOpacity( double &opacity );

protected slots:
	void targetChanged();

private:
	enum { normal, moveOrigin, moveVector, moveCenter, createNew } m_state;

	class VGradientOptionsWidget : public KDialogBase
	{
	public:
		VGradientOptionsWidget( VGradient& gradient );
		VGradientTabWidget *gradientWidget() { return m_gradientWidget; }
	private:
		VGradientTabWidget *m_gradientWidget;
	};
	VGradient            m_gradient; /**< the actual gradient */
	KoPoint              m_current;  /**< the current position when dragging */
	KoPoint              m_fixed;    /**< the fixed point when only dragging one point of the gradient vector */
	VGradientOptionsWidget*  m_optionsWidget; /**< the options dialog, for editing gradients */
	KoRect               m_origin;   /**< the handle of the gradient origin */
	KoRect               m_vector;   /**< the handle of the gradient vector */
	KoRect               m_center;   /**< the handle of the radial gradient focal point */
	int                  m_handleSize; /**< the handle's size */
	bool                 m_active;   /**< holds active state of the tool, which is used for drawing of the gradient vector */
};

#endif

