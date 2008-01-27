/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#ifndef SIMPLETEXTTOOL_H
#define SIMPLETEXTTOOL_H

#include "SimpleTextShape.h"

#include <KoTool.h>

class QAction;

/// This is the tool for the simple text shape.
class SimpleTextTool : public KoTool 
{
    Q_OBJECT
public:
    explicit SimpleTextTool(KoCanvasBase *canvas);
    ~SimpleTextTool();

    /// reimplemented
    virtual void paint( QPainter &painter, const KoViewConverter &converter );

    /// reimplemented
    virtual void mousePressEvent( KoPointerEvent *event ) ;
    /// reimplemented
    virtual void mouseMoveEvent( KoPointerEvent *event );
    /// reimplemented
    virtual void mouseReleaseEvent( KoPointerEvent *event );
    /// reimplemented
    virtual void activate (bool temporary=false);
    /// reimplemented
    virtual void deactivate();
    /// reimplemented
    virtual QWidget *createOptionWidget();
    /// reimplemented
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void attachPath();
    void detachPath();
    void convertText();

private:
    void updateActions();

    SimpleTextShape * m_currentShape;
    KoPathShape * m_path;
    KoPathShape * m_tmpPath;

    QAction * m_attachPath;
    QAction * m_detachPath;
    QAction * m_convertText;
};

#endif // SIMPLETEXTTOOL_H
