/* This file is part of the KDE project
   Copyright (C) 2001-2002 Benoît Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Rob Buis <buis@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

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
#include <QTabWidget>
#include <Q3ListBox>
#include <QPixmap>

#include <karbon_export.h>

class KComboBox;
class VGradientWidget;
class KListBox;
class KIntNumInput;
class QPushButton;
class Q3GroupBox;
class KarbonResourceServer;

/**
 * A list item for showing gradients in a list box.
 */
class VGradientListItem : public Q3ListBoxItem
{
public:
    /**
     * Constructs a new gradient list item with the given gradient and file name.
     *
     * The gradient list item takes ownership of the gradient.
     *
     * @param gradient the gradient the item represents
     * @param filename the filename of the gradient
     */
    VGradientListItem( const QGradient * gradient, const QString & filename );

    /// Copy constructor
    explicit VGradientListItem( const VGradientListItem& );

    /// Destroys the gradient list item
    ~VGradientListItem();

    QPixmap& pixmap() { return m_pixmap; }
    /// Returns the list items gradient
    const QGradient* gradient() const { return m_gradient; }
    /// Returns the filename of the gradient
    QString filename() { return m_filename; }
    /// Returns if the gradient can be removed from disk
    bool canDelete() { return m_delete; }

    virtual int height( const Q3ListBox* ) const { return 16; }
    virtual int width( const Q3ListBox* lb ) const;

protected:
    virtual void paint( QPainter* p );

private:
    const QGradient * m_gradient; ///< the gradient of this list item
    QPixmap m_pixmap; ///< cached pixmap for drawing
    QString m_filename; ///< the filename the gradient was loaded from
    bool m_delete; ///< shows if the gradient can be removed from disk
};

/// A widget to preview a gradient
class VGradientPreview : public QWidget
{
public:
    /// Constructs a gradient preview with the givne parent
    explicit VGradientPreview( QWidget* parent = 0L );

    /// Destroys the gradient preview
    ~VGradientPreview();

    /**
     * Sets the gradient to preview.
     * @param gradient the gradient to preview
     */
    void setGradient( const QGradient * gradient );

protected:
    virtual void paintEvent( QPaintEvent* );
private:
    QGradient * m_gradient; ///< the gradient to preview
};

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
     * Creates a new gradient tab widget with the given parent.
     *
     * The predefined gradients are retrived from the given resource server.
     *
     * @param server the resource server to retrieve predefined gradients from
     * @param parent the widgets parent
     * @param name the widgets name
     */
    explicit VGradientTabWidget( QWidget* parent = 0L );

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

    /**
     * Sets the resource server to get the predefined gradients from.
     * @param server the new resource server to use
     */
    void setResourceServer( KarbonResourceServer* server );

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
    void updateUI();
    void updatePredefGradients();
    void setupConnections();

private:
    QWidget          *m_editTab;
    VGradientWidget  *m_gradientWidget;
    KComboBox        *m_gradientTarget;
    KComboBox        *m_gradientRepeat;
    KComboBox        *m_gradientType;
    KListBox         *m_predefGradientsView;
    QPushButton      *m_predefDelete;
    QPushButton      *m_predefImport;
    QPushButton      *m_addToPredefs;
    KIntNumInput     *m_opacity;

    QGradient * m_gradient; /// the actual edited gradient
    KarbonResourceServer* m_resourceServer; ///< the predefined gradients list.
    double m_gradOpacity;    ///< the gradient opacity
};

/// helper function to clone a gradient
QGradient * cloneGradient( const QGradient * gradient );

#endif /* _VGRADIENTTABWIDGET_H_ */
