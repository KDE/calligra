/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SECTIONS_BOX_DOCKER_H_
#define _SECTIONS_BOX_DOCKER_H_

#include "ui_wdgsectionsbox.h"

class Document;
class DocumentModel;
class QSortFilterProxyModel;
class View;

#include <QDockWidget>
#include <KoDockFactory.h>

class SectionsBoxDock : public QDockWidget {
    Q_OBJECT
  public:
    SectionsBoxDock();
    virtual ~SectionsBoxDock();
    void setup(Document* document, View* m_view);
  private slots:
    void slotSectionActivated(const QModelIndex &);
    void slotMinimalView();
    void slotDetailedView();
    void slotThumbnailView();
    void searchBoxUpdated(QString );
  private:
    Ui::WdgSectionsBox m_wdgSectionsBox;
    DocumentModel* m_model;
    View* m_view;
    QSortFilterProxyModel* m_proxy;
};


class SectionsBoxDockFactory : public KoDockFactory
{

public:
    SectionsBoxDockFactory() { }

    virtual QString id() const {
        return QString("SectionsBox");
    }

    virtual QDockWidget* createDockWidget() {
        SectionsBoxDock * dockWidget = new SectionsBoxDock();

        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const {
        return DockRight;
    }
};

#endif
