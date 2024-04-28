/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSHAPECOLLECTIONDOCKER_H
#define KOSHAPECOLLECTIONDOCKER_H

#include <QDockWidget>
#include <QIcon>
#include <QMap>
#include <QModelIndex>

#include <KoCanvasObserverBase.h>
#include <KoDockFactoryBase.h>

class ShapeCollectionDockerFactory : public KoDockFactoryBase
{
public:
    ShapeCollectionDockerFactory();

    QString id() const override;
    QDockWidget *createDockWidget() override;
    DockPosition defaultDockPosition() const override
    {
        return DockRight;
    }
};

class CollectionItemModel;
class KoShape;
class QListView;
class QListWidget;
class QListWidgetItem;
class QToolButton;
class QMenu;
class QSpacerItem;
class QGridLayout;

class ShapeCollectionDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit ShapeCollectionDocker(QWidget *parent = 0);

    /// reimplemented
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

protected Q_SLOTS:
    /**
     * Activates the shape creation tool when a shape is selected.
     */
    void activateShapeCreationTool(const QModelIndex &index);
    void activateShapeCreationToolFromQuick(const QModelIndex &index);

    /**
     * Changes the current shape collection
     */
    void activateShapeCollection(QListWidgetItem *item);

    /**
     * Called when a collection is added from the add collection menu
     */
    void loadCollection();

    /// Called when an error occurred while loading a collection
    void onLoadingFailed(const QString &reason);

    /// Called when loading of a collection is finished
    void onLoadingFinished();

    /// Called when the close collection button is clicked
    void removeCurrentCollection();

    /// Called when the docker changes area
    void locationChanged(Qt::DockWidgetArea area);

protected:
    /**
     * Load the default calligra shapes
     */
    void loadDefaultShapes();

    /**
     * Add a collection to the docker
     */
    bool addCollection(const QString &id, const QString &title, CollectionItemModel *model);
    void removeCollection(const QString &id);

    /**
     * Builds the menu for the Add Collection Button
     */
    void buildAddCollectionMenu();

    /// Generate an icon from @p shape
    QIcon generateShapeIcon(KoShape *shape);

private:
    void scanCollectionDir(const QString &dirName, QMenu *menu);

private:
    QListView *m_quickView;
    QToolButton *m_moreShapes;
    QMenu *m_moreShapesContainer;
    QListWidget *m_collectionChooser;
    QListView *m_collectionView;
    QToolButton *m_closeCollectionButton;
    QToolButton *m_addCollectionButton;
    QSpacerItem *m_spacer;
    QGridLayout *m_layout;

    QMap<QString, CollectionItemModel *> m_modelMap;
};

#endif // KOSHAPECOLLECTIONDOCKER_H
