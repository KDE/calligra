/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBONSTYLEDOCKER_H
#define KARBONSTYLEDOCKER_H

#include <QDockWidget>
#include <KoCanvasObserver.h>

class KarbonStylePreview;
class KarbonStyleButtonBox;
class KoShapeBorderModel;
class KoShapeBackground;
class KoCanvasBase;
class KoColorComboBox;
class KoResource;
class KoShape;
class QStackedWidget;

class KarbonStyleDocker : public QDockWidget, public KoCanvasObserver
{
    Q_OBJECT
public:
    explicit KarbonStyleDocker( QWidget * parent = 0L );
    virtual ~KarbonStyleDocker();

    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);

    /// Returns if the stroke mode is active
    bool strokeIsSelected() const;
    
    /// Sets the shape border and fill to display
    void updateStyle( KoShapeBorderModel * stroke, KoShapeBackground * fill );

private slots:
    void fillSelected();
    void strokeSelected();
    void selectionChanged();
    void resourceChanged( int key, const QVariant& );
    void styleButtonPressed( int buttonId );
    void updateColor( const QColor &c );
    void updateGradient( KoResource * item );
    void updatePattern( KoResource * item );
    void updateFillRule( Qt::FillRule fillRule );
private:
    void updateColor( const QColor &c, const QList<KoShape*> & selectedShapes );

    KarbonStylePreview * m_preview;
    KarbonStyleButtonBox * m_buttons;
    QStackedWidget * m_stack;
    KoCanvasBase * m_canvas;
    KoColorComboBox * m_colorSelector;
};

#endif // KARBONSTYLEDOCKER_H
