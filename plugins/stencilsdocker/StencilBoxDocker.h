/* This file is part of the Calligra project
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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

#ifndef KOSTENCILBOXDOCKER_H
#define KOSTENCILBOXDOCKER_H

#include <QDockWidget>
#include <QListView>
#include <QMap>
#include <QIcon>
#include <QThread>

class CollectionItemModel;
class CollectionTreeWidget;

class KLineEdit;

class QToolButton;
class QMenu;
class QVBoxLayout;
class QHBoxLayout;
class QSortFilterProxyModel;

class StencilBoxDockerLoader;

class StencilBoxDocker : public QDockWidget
{
    Q_OBJECT
    public:
        explicit StencilBoxDocker(QWidget* parent = 0);
        ~StencilBoxDocker() override;

    protected:
        void removeCollection(const QString& family);

    protected Q_SLOTS:
        /// Called when the docker changes area
        void locationChanged(Qt::DockWidgetArea area);

    private:
        QMap<QString, CollectionItemModel*> m_modelMap;
        //QMap<QString, QSortFilterProxyModel*> m_proxyMap;

        CollectionTreeWidget *m_treeWidget;
        QMenu* m_menu;
        QToolButton* m_button;
        KLineEdit* m_filterLineEdit;
        QVBoxLayout* m_layout;
        QHBoxLayout* m_panelLayout;

        QThread loaderThread;
        StencilBoxDockerLoader *m_loader;

    private Q_SLOTS:
        void reapplyFilter();
#ifdef GHNS
        void getHotNewStuff();
#endif
        void manageStencilsFolder();
        //void regenerateProxyMap();
        void collectionsLoaded();
        void threadStarted();

    Q_SIGNALS:
        void startLoading();
};

#endif //KOSHAPECOLLECTIONDOCKER_H
