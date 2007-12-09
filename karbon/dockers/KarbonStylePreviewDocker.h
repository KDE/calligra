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

#ifndef _KARBONSTYLEPREVIEWDOCKER_H_
#define _KARBONSTYLEPREVIEWDOCKER_H_

#include <QDockWidget>
#include <KoDockFactory.h>
#include <KoCanvasObserver.h>

class KarbonStylePreview;
class KarbonStyleButtonBox;
class QBrush;
class KoShapeBorderModel;
class KoCanvasBase;

class KarbonStylePreviewDocker : public QDockWidget, public KoCanvasObserver
{
    Q_OBJECT
public:
    explicit KarbonStylePreviewDocker( QWidget * parent = 0L );
    ~KarbonStylePreviewDocker();

    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);

    bool strokeIsSelected() const;
    void updateStyle( const KoShapeBorderModel * stroke, const QBrush & fill );
private slots:
    void fillSelected();
    void strokeSelected();
    void selectionChanged();
private:
    KarbonStylePreview * m_preview;
    KarbonStyleButtonBox * m_buttons;
    KoCanvasBase * m_canvas;
};

class KarbonStylePreviewDockerFactory : public KoDockFactory
{
public:
    KarbonStylePreviewDockerFactory();

    virtual QString id() const;
    virtual KoDockFactory::DockPosition defaultDockPosition() const;
    virtual QDockWidget* createDockWidget();
    virtual bool isCollapsable() const { return false; }
};


#endif // _KARBONSTYLEPREVIEWDOCKER_H_
