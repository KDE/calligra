/* This file is part of the KDE project
   Copyright 2010 - Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
#include <QDebug>

#include <KoGlobal.h>
#include <KoZoomHandler.h>

#include <part/Doc.h>
#include <part/Part.h>
#include <part/CanvasItem.h>
#include <part/HeaderItems.h>
#include <Sheet.h>

int main(int argc, char** argv)
{
    QApplication qapp(argc, argv);

    Calligra::Sheets::Part part;
    Calligra::Sheets::Doc doc(&part);
    part.setDocument(&doc);


    bool ok = doc.openUrl(QUrl("/home/marijn/kde/src/calligra/docs/oos_AMSAT-IARU_Link_Model.ods"));
    if (!ok) {
        qDebug() << "failed to load";
        return 0;
    }

    QFont font(KoGlobal::defaultFont());

    Calligra::Sheets::CanvasItem* canvas = new Calligra::Sheets::CanvasItem(&doc);

    QRect usedArea = canvas->activeSheet()->usedArea(true);
    QFontMetricsF fm(font, 0);
    QSizeF size(canvas->activeSheet()->columnPosition(usedArea.right()+3), canvas->activeSheet()->rowPosition(usedArea.bottom()+5));
    canvas->setDocumentSize(size);
    size = canvas->zoomHandler()->documentToView(size);
    canvas->resize(size);
    canvas->setPos(0, 0);

    Calligra::Sheets::ColumnHeaderItem* columnHeader = static_cast<Calligra::Sheets::ColumnHeaderItem*>(canvas->columnHeader());
    static_cast<QGraphicsWidget*>(columnHeader)->resize(size.width(), fm.height() + 3);
    columnHeader->setPos(0, -columnHeader->height());

    Calligra::Sheets::RowHeaderItem* rowHeader = static_cast<Calligra::Sheets::RowHeaderItem*>(canvas->rowHeader());
    static_cast<QGraphicsWidget*>(rowHeader)->resize(fm.width(QString::fromLatin1("99999")) + 3, size.height());
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
