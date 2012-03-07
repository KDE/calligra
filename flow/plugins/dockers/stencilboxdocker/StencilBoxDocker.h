/* This file is part of the Calligra project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2010-2012 Yue Liu <yue.liu@mail.com>
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
#ifndef STENCILBOXDOCKER_H
#define STENCILBOXDOCKER_H

#include <QDockWidget>
#include <QMap>
#include <QIcon>

#include <KoDockFactoryBase.h>

class StencilBoxTreeWidget;
class KoShape;

class KLineEdit;

class QActionGroup;
class QToolButton;
class QMenu;
class QVBoxLayout;
class QHBoxLayout;
//class QSortFilterProxyModel;

class StencilBoxDockerFactory : public KoDockFactoryBase
{
public:
    StencilBoxDockerFactory();

    QString id() const;
    QDockWidget* createDockWidget();
    DockPosition defaultDockPosition() const
    {
        return DockLeft;
    }
};

class StencilBoxDocker : public QDockWidget
{
    Q_OBJECT
public:
    explicit StencilBoxDocker(QWidget* parent = 0);

protected slots:
    /// Called when the docker changes area
    void locationChanged(Qt::DockWidgetArea area);

private:
    void updateStencilMap();
    void updateMenu();

    QMap<QString, QStringList> m_stencilMap; //Key: Stencil Name, Value: Stencil ID
    QActionGroup* m_stencilActionGroup;

    StencilBoxTreeWidget* m_treeWidget;
    QMenu* m_menu;
    QToolButton* m_button;
    KLineEdit* m_filterLineEdit;
    QVBoxLayout* m_layout;
    QHBoxLayout* m_subLayout;

private slots:
    void reapplyFilter();

    /// Get stencils from GHNS service
    void getHotNewStuff();

    /// Install stencil files
    void installStencil();

    /// Create new stencil
    void createStencil();

    /// Edit stencil name
    void editStencil();

    /// Remove stencil files
    void removeStencil();

    /// Create new shape
    void createShape();

    /// Edit shape
    void editShape();

    /// Remove a shape from a user-created stencil
    void removeShape(const QString &stencilId, const QString &shapeId);

    /// Open stencil in the docker
    void openStencil(QAction* action);
};

#endif //STENCILBOXDOCKER_H
