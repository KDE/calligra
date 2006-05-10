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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _VGRADIENTTABWIDGET_H_
#define _VGRADIENTTABWIDGET_H_

#include <QWidget>
#include <qtabwidget.h>
#include <qlistbox.h>
#include <koffice_export.h>
#include "vgradient.h"

class KComboBox;
class VGradientWidget;
class KListBox;
class KIntNumInput;
class QPushButton;
class QGroupBox;
class KarbonResourceServer;


class VGradientListItem : public QListBoxItem
{
public:
	VGradientListItem( const VGradient& gradient, QString filename );
	VGradientListItem( const VGradientListItem& );
	~VGradientListItem();

	QPixmap& pixmap() { return m_pixmap; }
	const VGradient* gradient() const { return m_gradient; }
	QString filename() { return m_filename; }
	bool canDelete() { return m_delete; }

	virtual int height( const QListBox* ) const { return 16; }
	virtual int width( const QListBox* lb ) const;

protected:
	virtual void paint( QPainter* p );

private:
	VGradient	*m_gradient;
	QPixmap		m_pixmap;
	QString		m_filename;
	bool		m_delete;
}; // VGradientListItem

class VGradientPreview : public QWidget
{
	public:
		VGradientPreview( VGradient& gradient, double& opacity, QWidget* parent = 0L, const char* name = 0L );
		~VGradientPreview();

		virtual void paintEvent( QPaintEvent* );
		
	protected:
		VGradient*     m_gradient;
		double*         m_opacity;
}; // VGradientPreview

class KARBONBASE_EXPORT VGradientTabWidget : public QTabWidget
{
	Q_OBJECT

	public:
		enum VGradientTarget {
			STROKE,
			FILL
		};
	
		VGradientTabWidget( VGradient& gradient, KarbonResourceServer* server, QWidget* parent = 0L, const char* name = 0L );
		~VGradientTabWidget();

		const VGradient& gradient();
		void setGradient( VGradient& gradient );

		VGradientTarget target();
		void setTarget( VGradientTarget target );

		double opacity() const;
		void setOpacity( double opacity );

	public slots:
		void combosChange( int );
		void addGradientToPredefs();
		void changeToPredef( QListBoxItem* );
		void predefSelected( QListBoxItem* );
		void deletePredef();
		void opacityChanged( int );

	protected:
		void setupUI();
		void initUI();
		void setupConnections();
		
	private:
		QGroupBox				*m_editGroup;
		VGradientWidget			*m_gradientWidget;
		KComboBox				*m_gradientTarget;
		KComboBox				*m_gradientRepeat;
		KComboBox				*m_gradientType;
		VGradientPreview		*m_gradientPreview;
		KListBox				*m_predefGradientsView;
		QPushButton				*m_predefDelete;
		QPushButton				*m_predefImport;
		QPushButton				*m_addToPredefs;
		KIntNumInput			*m_opacity;

		VGradient             m_gradient;
			/** The predefined gradients list. */
		KarbonResourceServer* m_resourceServer;
		double                m_gradOpacity;
}; // VGradientTabWidget

#endif /* _VGRADIENTTABWIDGET_H_ */
