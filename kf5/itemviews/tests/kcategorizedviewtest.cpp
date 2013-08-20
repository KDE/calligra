/**
  * This file is part of the KDE project
  * Copyright (C) 2007, 2009 Rafael Fernández López <ereslibre@kde.org>
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

#include <QApplication>
#include <QMainWindow>
#include <QStringListModel>

#include <kiconloader.h>

#include <kcategorizedview.h>
#include <kcategorydrawer.h>
#include <kcategorizedsortfilterproxymodel.h>

QStringList icons;

class MyModel
    : public QStringListModel
{
public:
    virtual QVariant data(const QModelIndex &index, int role) const
    {
        switch (role) {
            case KCategorizedSortFilterProxyModel::CategoryDisplayRole: {
                    return QString::number(index.row() / 10);
                }
            case KCategorizedSortFilterProxyModel::CategorySortRole: {
                    return index.row() / 10;
                }
            case Qt::DecorationRole:
                return DesktopIcon(icons[index.row() % 4], KIconLoader::Desktop);
            default:
                break;
        }
        return QStringListModel::data(index, role);
    }
};

int main(int argc, char **argv)
{
    icons << "konqueror";
    icons << "okular";
    icons << "plasma";
    icons << "system-file-manager";

    QApplication app(argc, argv);

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    KCategorizedView *listView = new KCategorizedView();
    listView->setCategoryDrawer(new KCategoryDrawer());
    listView->setViewMode(QListView::IconMode);
    MyModel *model = new MyModel();

    model->insertRows(0, 100);
    for (int i = 0; i < 100; ++i)
    {
        model->setData(model->index(i, 0), QString::number(i), Qt::DisplayRole);
    }

    KCategorizedSortFilterProxyModel *proxyModel = new KCategorizedSortFilterProxyModel();
    proxyModel->setCategorizedModel(true);
    proxyModel->setSourceModel(model);

    listView->setModel(proxyModel);

    mainWindow->setCentralWidget(listView);

    mainWindow->show();

    return app.exec();
}

