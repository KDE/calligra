/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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
#include <q3listbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <karbon_export.h>
#include "vgradient.h"

class KComboBox;
class VGradientWidget;
class KListBox;
class KIntNumInput;
class QPushButton;
class Q3GroupBox;
class KarbonResourceServer;


class VGradientListItem : public Q3ListBoxItem
{
public:
    VGradientListItem( const QGradient * gradient, QString filename );
    VGradientListItem( const VGradientListItem& );
    ~VGradientListItem();

    QPixmap& pixmap() { return m_pixmap; }
    const QGradient* gradient() const { return m_gradient; }
    QString filename() { return m_filename; }
    bool canDelete() { return m_delete; }

    virtual int height( const Q3ListBox* ) const { return 16; }
    virtual int width( const Q3ListBox* lb ) const;

protected:
    virtual void paint( QPainter* p );

private:
    const QGradient * m_gradient;
    QPixmap m_pixmap;
    QString m_filename;
    bool m_delete;
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

/**
 * A tab widget for managing gradients.
 *
 * It has one tab to edit a selected gradients type, spread method and color stops.
 * Another tab contains a list with predefined gradients to choose from.
 */
class KARBONBASE_EXPORT VGradientTabWidget : public QTabWidget
{
Q_OBJECT

public:
    enum VGradientTarget {
        StrokeGradient,
        FillGradient
    };

    /**
     * Creates a new gradient tab widget with the given selected gradient.
     *
     * The predefined gradients are retrived from the fiven resource server.
     *
     * @param gradient the gradient to select
     * @param server the resource server to retrieve predefined gradients from
     * @param parent the widgets parent
     * @param name the widgets name
     */
    VGradientTabWidget( const QGradient* gradient, KarbonResourceServer* server, QWidget* parent = 0L, const char* name = 0L );

    /// Destroys the widget
    ~VGradientTabWidget();

    /**
     * Returns the actual selected gradient.
     * @return the actual gradient
     */
    const QGradient* gradient();

    /**
     * Sets a new gradient to edit.
     * @param gradient the gradient to edit
     */
    void setGradient( const QGradient* gradient );

    /// Returns the gradient target (fill/stroke)
    VGradientTarget target();

    /// Sets a new gradient target
    void setTarget( VGradientTarget target );

    /// Returns the gradient opacity
    double opacity() const;

    /// Sets the gradients opacity to @p opacity
    void setOpacity( double opacity );

Q_SIGNALS:
    /// Is emmited a soon as the gradient changes
    void changed();

protected Q_SLOTS:
    void combosChange( int );
    void addGradientToPredefs();
    void changeToPredef( Q3ListBoxItem* );
    void predefSelected( Q3ListBoxItem* );
    void deletePredef();
    void opacityChanged( int );
    void stopsChanged();

protected:
    void setupUI();
    void initUI();
    void setupConnections();

private:
    QWidget          *m_editTab;
    VGradientWidget  *m_gradientWidget;
    KComboBox        *m_gradientTarget;
    KComboBox        *m_gradientRepeat;
    KComboBox        *m_gradientType;
    VGradientPreview *m_gradientPreview;
    KListBox         *m_predefGradientsView;
    QPushButton      *m_predefDelete;
    QPushButton      *m_predefImport;
    QPushButton      *m_addToPredefs;
    KIntNumInput     *m_opacity;

    QGradient * m_gradient; /// the actual edited gradient
    VGradient m_oldGradient;
    KarbonResourceServer* m_resourceServer; ///< the predefined gradients list.
    double                m_gradOpacity;    ///< the gradient opacity
};

/// helper function to clone a gradient
QGradient * cloneGradient( const QGradient * gradient );

#endif /* _VGRADIENTTABWIDGET_H_ */
