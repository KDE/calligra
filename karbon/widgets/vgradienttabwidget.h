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

#ifndef _VGRADIENTTABWIDGET_H_
#define _VGRADIENTTABWIDGET_H_

#include <qwidget.h>
#include <qtabwidget.h>

#include "vgradient.h"

class KComboBox;
class VGradientWidget;
class KListBox;
class QListBoxItem;
class QPushButton;
class QGroupBox;

class VGradientPreview : public QWidget
{
	public:
		VGradientPreview( VGradient*& gradient, QWidget* parent = 0L, const char* name = 0L );
		~VGradientPreview();

		virtual void paintEvent( QPaintEvent* );
		
	protected:
		VGradient**     m_lpgradient;
}; // VGradientPreview

class VGradientTabWidget : public QTabWidget
{
	Q_OBJECT

	public:
		enum VGradientTarget {
			STROKE,
			FILL
		};
	
		VGradientTabWidget( VGradient& gradient, QWidget* parent = 0L, const char* name = 0L );
		~VGradientTabWidget();

		const VGradient* gradient();
		void setGradient( VGradient& gradient );

		VGradientTarget target();
		void setTarget( VGradientTarget target );

	public slots:
		void combosChange( int );
		void addGradientToPredefs();
		void changeToPredef( QListBoxItem* );
		void deletePredef();
		
	protected:
		void setupUI();
		void initUI();
		void setupConnections();
		
	private:
		QTabWidget*       m_tabWidget;
		QGroupBox*        m_editGroup;
		VGradientWidget*  m_gradientWidget;
		KComboBox*        m_gradientTarget;
		KComboBox*        m_gradientRepeat;
		KComboBox*        m_gradientType;
		VGradientPreview* m_gradientPreview;
		KListBox*         m_predefGradientsView;
		QPushButton*      m_predefDelete;
		QPushButton*      m_addToPredefs;

		VGradient*        m_gradient;
			/** The predefined gradients list. */
		QPtrList<VGradient>     m_predefGradients;
}; // VGradientTabWidget

#endif /* _VGRADIENTTABWIDGET_H_ */
