/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
   SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
   SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_RESOURCE_ITEM_CHOOSER
#define KO_RESOURCE_ITEM_CHOOSER

#include <QWidget>

#include "kowidgets_export.h"

class QModelIndex;
class QAbstractProxyModel;
class QAbstractItemDelegate;
class QAbstractButton;
class QToolButton;
class KoAbstractResourceServerAdapter;
class KoResourceItemView;
class KoResource;

/**
 * A widget that contains a KoResourceChooser as well
 * as an import/export button
 */
class KOWIDGETS_EXPORT KoResourceItemChooser : public QWidget
{
    Q_OBJECT
public:
    enum Buttons { Button_Import, Button_Remove };

    /// \p usePreview shows the aside preview with the resource's image
    explicit KoResourceItemChooser(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QWidget *parent = nullptr, bool usePreview = false);
    ~KoResourceItemChooser() override;

    /// Sets number of columns in the view and causes the number of rows to be calculated accordingly
    void setColumnCount(int columnCount);

    /// Sets number of rows in the view and causes the number of columns to be calculated accordingly
    void setRowCount(int rowCount);

    /// Sets the height of the view rows
    void setRowHeight(int rowHeight);

    /// Sets the width of the view columns
    void setColumnWidth(int columnWidth);

    /// Sets a custom delegate for the view
    void setItemDelegate(QAbstractItemDelegate *delegate);

    /// Gets the currently selected resource
    /// @returns the selected resource, 0 is no resource is selected
    KoResource *currentResource() const;

    /// Sets the item representing the resource as selected
    void setCurrentResource(KoResource *resource);

    /**
     * Sets the sected resource, does nothing if there is no valid item
     * @param row row of the item
     * @param column column of the item
     */
    void setCurrentItem(int row, int column);

    void showButtons(bool show);

    void addCustomButton(QAbstractButton *button, int cell);

    /// determines whether the preview right or below the splitter
    void setPreviewOrientation(Qt::Orientation orientation);
    /// determines whether the preview should tile the resource's image or not
    void setPreviewTiled(bool tiled);
    /// shows the preview converted to grayscale
    void setGrayscalePreview(bool grayscale);

    /// sets the visibility of tagging KlineEdits.
    void showTaggingBar(bool show);

    /// Set a proxy model with will be used to filter the resources
    void setProxyModel(QAbstractProxyModel *proxyModel);

    QSize viewSize() const;

    KoResourceItemView *itemView() const;

    void setViewModeButtonVisible(bool visible);
    QToolButton *viewModeButton() const;

    void setSynced(bool sync);

    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    /// Emitted when a resource was selected
    void resourceSelected(KoResource *resource);
    void splitterMoved();
public Q_SLOTS:
    void slotButtonClicked(int button);

private Q_SLOTS:
    void activated(const QModelIndex &index);
    void contextMenuRequested(const QPoint &pos);
    void baseLengthChanged(int length);

    void slotBeforeResourcesLayoutReset(KoResource *activateAfterReset);
    void slotAfterResourcesLayoutReset();

    void updateView();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void updateButtonState();
    void updatePreview(KoResource *resource);

    void resizeEvent(QResizeEvent *event) override;

    /// Resource for a given model index
    /// @returns the resource pointer, 0 is index not valid
    KoResource *resourceFromModelIndex(const QModelIndex &index) const;

    class Private;
    Private *const d;
};

#endif // KO_RESOURCE_ITEM_CHOOSER
