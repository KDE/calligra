/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.t-com.hr)
   Copyright (C) 2005, The Karbon Developers
   Copyright (c) 2008 Jan Hambrecht <jaham@gmx.net>

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


#ifndef KARBONSMALLSTYLEPREVIEW_H
#define KARBONSMALLSTYLEPREVIEW_H

#include <KoCheckerBoardPainter.h>
#include <QtGui/QWidget>

class QPaintEvent;
class KarbonFillStyleWidget;
class KarbonStrokeStyleWidget;
class KoCanvasController;

/// This is a small widget used on the statusbar, to display fill/stroke colors etc.
class KarbonSmallStylePreview : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonSmallStylePreview( QWidget* parent = 0L );
    virtual ~KarbonSmallStylePreview();

private slots:
    void selectionChanged();
    void toolCodesSelected(const KoCanvasController *canvas, QList<QString> types);

private:
    KarbonFillStyleWidget * m_fillFrame;
    KarbonStrokeStyleWidget * m_strokeFrame;
};

#endif // KARBONSMALLSTYLEPREVIEW_H
