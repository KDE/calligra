/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 - Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <KoGlobal.h>
#include <KoZoomHandler.h>

#include <core/Sheet.h>
#include <part/CanvasItem.h>
#include <part/Doc.h>
#include <part/HeaderItems.h>
#include <part/Part.h>

int main(int argc, char **argv)
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

    Calligra::Sheets::CanvasItem *canvas = new Calligra::Sheets::CanvasItem(&doc);

    QRect usedArea = canvas->activeSheet()->usedArea(true);
    QFontMetricsF fm(font, nullptr);
    QSizeF size(canvas->activeSheet()->columnPosition(usedArea.right() + 3), canvas->activeSheet()->rowPosition(usedArea.bottom() + 5));
    canvas->setDocumentSize(size);
    size = canvas->zoomHandler()->documentToView(size);
    canvas->resize(size);
    canvas->setPos(0, 0);

    Calligra::Sheets::ColumnHeaderItem *columnHeader = static_cast<Calligra::Sheets::ColumnHeaderItem *>(canvas->columnHeader());
    static_cast<QGraphicsWidget *>(columnHeader)->resize(size.width(), fm.height() + 3);
    columnHeader->setPos(0, -columnHeader->height());

    Calligra::Sheets::RowHeaderItem *rowHeader = static_cast<Calligra::Sheets::RowHeaderItem *>(canvas->rowHeader());
    static_cast<QGraphicsWidget *>(rowHeader)->resize(fm.boundingRect(QString::fromLatin1("99999")).width() + 3, size.height());
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
