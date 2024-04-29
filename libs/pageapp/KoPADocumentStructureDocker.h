/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPADOCUMENTSTRUCTUREDOCKER_H
#define KOPADOCUMENTSTRUCTUREDOCKER_H

#include <KoCanvasObserverBase.h>
#include <KoDockFactoryBase.h>
#include <KoDocumentSectionView.h>
#include <KoPageApp.h>
#include <QDockWidget>
#include <QHash>

class KoShape;
class KoShapeLayer;
class KoPADocument;
class KoPADocumentModel;
class KoPAPageBase;
class KoViewItemContextBar;
class QAction;
class QButtonGroup;

class KoPADocumentStructureDockerFactory : public KoDockFactoryBase
{
public:
    explicit KoPADocumentStructureDockerFactory(KoDocumentSectionView::DisplayMode mode, KoPageApp::PageType pageType = KoPageApp::Page);

    QString id() const override;
    QDockWidget *createDockWidget() override;

    DockPosition defaultDockPosition() const override
    {
        return DockRight;
    }

private:
    KoDocumentSectionView::DisplayMode m_mode;
    KoPageApp::PageType m_pageType;
};

class KoPADocumentStructureDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    explicit KoPADocumentStructureDocker(KoDocumentSectionView::DisplayMode mode, KoPageApp::PageType pageType, QWidget *parent = nullptr);
    ~KoPADocumentStructureDocker() override;

    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;
    void setActivePage(KoPAPageBase *page);
    void setMasterMode(bool master);

protected:
    /// This is the context menu for the slide show in the KoPADocumentStructure docker
    void contextMenuEvent(QContextMenuEvent *event) override;

Q_SIGNALS:
    void pageChanged(KoPAPageBase *page);

    /// This signal will be emitted after the model for this docker has been reset
    void dockerReset();

public Q_SLOTS:
    void updateView();

    void selectPages(int start, int count);

private Q_SLOTS:
    void slotButtonClicked(int buttonId);
    void addLayer();
    void addPage();
    void deleteItem();
    void raiseItem();
    void lowerItem();
    void itemClicked(const QModelIndex &index);
    void minimalView();
    void detailedView();
    void thumbnailView();

    void itemSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void editCut();
    void editCopy();
    void editPaste();

private:
    void extractSelectedLayersAndShapes(QList<KoPAPageBase *> &pages, QList<KoShapeLayer *> &layers, QList<KoShape *> &shapes);
    void setViewMode(KoDocumentSectionView::DisplayMode mode);
    QModelIndex getRootIndex(const QModelIndex &index) const;

    KoDocumentSectionView::DisplayMode viewModeFromString(const QString &mode);
    QString viewModeToString(KoDocumentSectionView::DisplayMode mode);

    KoPADocument *m_doc;
    KoDocumentSectionView *m_sectionView;
    KoPADocumentModel *m_model;
    QHash<KoDocumentSectionView::DisplayMode, QAction *> m_viewModeActions;
    QList<KoShape *> m_selectedShapes;
    QButtonGroup *m_buttonGroup;
    QAction *m_addLayerAction;
    KoViewItemContextBar *m_itemsContextBar;
};

#endif // KOPADOCUMENTSTRUCTUREDOCKER_H
