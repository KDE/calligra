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

#include <KoCanvasObserver.h>
#include <QtGui/QDockWidget>
#include <QtCore/QTime>

class KarbonStylePreview;
class KarbonStyleButtonBox;
class KoShapeBorderModel;
class KoShapeBorderCommand;
class KoShapeBackground;
class KoShapeBackgroundCommand;
class KoColorBackground;
class KoCanvasBase;
class KoResource;
class KoShape;
class KoColor;
class QToolButton;
class QStackedWidget;
class KoColorPopupAction;
class KoPathShape;

class KarbonStyleDocker : public QDockWidget, public KoCanvasObserver
{
    Q_OBJECT
public:
    explicit KarbonStyleDocker( QWidget * parent = 0L );
    virtual ~KarbonStyleDocker();

    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);

private slots:
    void fillSelected();
    void strokeSelected();
    void selectionChanged();
    void selectionContentChanged();
    void resourceChanged( int key, const QVariant& );
    void styleButtonPressed( int buttonId );
    void updateColor( const KoColor &c );
    void updateGradient( KoResource * item );
    void updatePattern( KoResource * item );
    void updateFillRule( Qt::FillRule fillRule );
private:
    void updateColor( const QColor &c, const QList<KoShape*> & selectedShapes );
    /// Sets the shape border and fill to display
    void updateStyle();
    void updateStyle( KoShapeBorderModel * stroke, KoShapeBackground * fill );
    
    /// Resets color related commands which are used to combine multiple color changes
    void resetColorCommands();
    
    /// Returns list of selected path shapes
    QList<KoPathShape*> selectedPathShapes();
    
    void updateStyleButtons( int activeStyle );
    
    KarbonStylePreview * m_preview;
    KarbonStyleButtonBox * m_buttons;
    QStackedWidget * m_stack;
    KoCanvasBase * m_canvas;
    QToolButton * m_colorSelector;
    KoColorPopupAction *m_actionColor;

    QTime m_lastColorChange;
    KoShapeBackgroundCommand * m_lastFillCommand;
    KoShapeBorderCommand * m_lastStrokeCommand;
    KoColorBackground * m_lastColorFill;
    QList<KoShapeBorderModel*> m_lastColorStrokes;
};

#endif // KARBONSTYLEDOCKER_H
