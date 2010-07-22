/* This file is part of the KDE project
   Copyright 2010 - Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   Boston, MA 02110-1301, USA.
*/
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <qdebug.h>

#include <kcomponentdata.h>

#include <KoGlobal.h>
#include <KoZoomHandler.h>

#include <part/Doc.h>
#include <part/CanvasItem.h>
#include <part/HeaderItems.h>
#include <Sheet.h>

int main(int argc, char** argv)
{
    QApplication qapp(argc, argv);
    KComponentData cd("graphicsview-test");

    KSpread::Doc doc;
    bool ok = doc.openUrl(KUrl("/home/marijn/Documents/Addition.xls"));
    if (!ok) {
        qDebug() << "failed to load";
        return 0;
    }

    QFont font(KoGlobal::defaultFont());

    KSpread::CanvasItem* canvas = new KSpread::CanvasItem(&doc);

    QRect usedArea = canvas->activeSheet()->usedArea(true);
    QSizeF size(canvas->activeSheet()->columnPosition(usedArea.right()+3), canvas->activeSheet()->rowPosition(usedArea.bottom()+5));
    canvas->setDocumentSize(size);
    size = canvas->zoomHandler()->documentToView(size);
    canvas->resize(size);
    canvas->setPos(0, 0);

    KSpread::ColumnHeaderItem* columnHeader = new KSpread::ColumnHeaderItem(0, canvas);
    static_cast<QGraphicsWidget*>(columnHeader)->resize(size.width(), canvas->zoomHandler()->zoomItY(font.pointSizeF() + 3));
    columnHeader->setPos(0, -columnHeader->height());

    KSpread::RowHeaderItem* rowHeader = new KSpread::RowHeaderItem(0, canvas);
    static_cast<QGraphicsWidget*>(rowHeader)->resize(canvas->zoomHandler()->zoomItX(YBORDER_WIDTH), size.height());
    rowHeader->setPos(-rowHeader->width(), 0);

    columnHeader->toolChanged("PanTool");
    rowHeader->toolChanged("PanTool");
    
    QGraphicsScene scene;
    scene.addItem(canvas);
    scene.addItem(columnHeader);
    scene.addItem(rowHeader);

    QGraphicsView view(&scene);
    view.show();

    qapp.exec();
}
