/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <TableModel.h>

namespace KDChart {
    class Chart;
    class BarDiagram;
    class DatasetProxyModel;
}

class QAbstractItemView;
class QStandardItemModel;
class QTableView;
class QItemSelectionModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected slots:
    void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

private slots:
    void initializeData();
    void openFile(const QString &path = QString());
    void saveFile();

private:
    void setupModel();
    void setupViews();

    KDChart::Chart* m_chart;
    KDChart::DatasetProxyModel* m_datasetProxy;

    QStandardItemModel  *m_model;
    KDChart::BarDiagram *m_diagramView;
    QTableView          *m_tableView;
    QItemSelectionModel *m_selectionModel;
};

#endif
