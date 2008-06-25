/* This file is part of the KDE project
   Copyright (C) 2001-2002 Beno�t Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Rob Buis <buis@kde.org>
   Copyright (C) 2006-2008 Jan Hambrecht <jaham@gmx.net>

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

#ifndef KARBONGRADIENTTABWIDGET_H
#define KARBONGRADIENTTABWIDGET_H

#include <karbonui_export.h>

#include <QWidget>
#include <QTabWidget>
#include <QtGui/QTableWidgetItem>

#include <KoAbstractGradient.h>
#include <KoCheckerBoardPainter.h>

class KComboBox;
class KarbonGradientWidget;
class KListWidget;
class QPushButton;
class KarbonGradientChooser;
class KoSliderCombo;
class KoResourceItem;
class KColorButton;

/**
 * A tab widget for managing gradients.
 *
 * It has one tab to edit a selected gradients type, spread method and color stops.
 * Another tab contains a list with predefined gradients to choose from.
 */
class KARBONUI_EXPORT KarbonGradientTabWidget : public QTabWidget
{
Q_OBJECT

public:
    enum GradientTarget {
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
    explicit KarbonGradientTabWidget( QWidget* parent = 0L );

    /// Destroys the widget
    ~KarbonGradientTabWidget();

    /**
     * Sets a new gradient to edit.
     * @param gradient the gradient to edit
     */
    void setGradient( const QGradient & gradient );

    /// Returns the gradient target (fill/stroke)
    GradientTarget target();

    /// Sets a new gradient target
    void setTarget( GradientTarget target );

    /// Returns the gradient opacity
    double opacity() const;

    /// Sets the gradients opacity to @p opacity
    void setOpacity( double opacity );

    /// Sets the index of the stop to edit
    void setStopIndex( int index );

    /// Returns the gradient spread
    QGradient::Spread spread() const;

    /// Sets the gradient spread
    void setSpread( QGradient::Spread spread );

    /// Returns the gradient type
    QGradient::Type type() const;

    /// Sets the gradient type
    void setType( QGradient::Type type );

    /// Returns the gradient stops
    QGradientStops stops() const;

    /// Sets the gradient stops
    void setStops( const QGradientStops &stops );

Q_SIGNALS:
    /// Is emmited a soon as the gradient changes
    void changed();

protected Q_SLOTS:
    void combosChange( int );
    void addGradientToPredefs();
    void changeToPredef( QTableWidgetItem* );
    void opacityChanged( double value, bool final );
    void stopsChanged();
    void stopChanged();
protected:
    void setupUI();
    void updateUI();
    void updatePredefGradients();
    void setupConnections();
    void blockChildSignals( bool block );

private:
    QWidget          *m_editTab;
    KarbonGradientWidget * m_gradientWidget;
    KComboBox        *m_gradientTarget;
    KComboBox        *m_gradientRepeat;
    KComboBox        *m_gradientType;
    KarbonGradientChooser *m_predefGradientsView;
    QPushButton      *m_addToPredefs;
    KoSliderCombo * m_opacity;
    KColorButton * m_stopColor;
    KoSliderCombo * m_stopOpacity;
    double m_gradOpacity;    ///< the gradient opacity
    int m_stopIndex; ///< the index of the selected gradient stop
    KoCheckerBoardPainter m_checkerPainter;
    QGradient::Type m_type;
    QGradient::Spread m_spread;
    QGradientStops m_stops;
};

#endif // KARBONGRADIENTTABWIDGET_H
