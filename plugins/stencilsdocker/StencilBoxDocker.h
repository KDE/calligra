/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSTENCILBOXDOCKER_H
#define KOSTENCILBOXDOCKER_H

#include <QDockWidget>
#include <QIcon>
#include <QListView>
#include <QMap>
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
    explicit StencilBoxDocker(QWidget *parent = nullptr);
    ~StencilBoxDocker() override;

protected:
    void removeCollection(const QString &family);

protected Q_SLOTS:
    /// Called when the docker changes area
    void locationChanged(Qt::DockWidgetArea area);

private:
    QMap<QString, CollectionItemModel *> m_modelMap;
    // QMap<QString, QSortFilterProxyModel*> m_proxyMap;

    CollectionTreeWidget *m_treeWidget;
    QMenu *m_menu;
    QToolButton *m_button;
    KLineEdit *m_filterLineEdit;
    QVBoxLayout *m_layout;
    QHBoxLayout *m_panelLayout;

    QThread loaderThread;
    StencilBoxDockerLoader *m_loader;

private Q_SLOTS:
    void reapplyFilter();
    void manageStencilsFolder();
    // void regenerateProxyMap();
    void collectionsLoaded();
    void threadStarted();

Q_SIGNALS:
    void startLoading();
};

#endif // KOSHAPECOLLECTIONDOCKER_H
