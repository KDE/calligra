/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCESELECTOR_H
#define KORESOURCESELECTOR_H

#include "kowidgets_export.h"
#include <QComboBox>

class QMouseEvent;
class KoAbstractResourceServerAdapter;
class KoResource;

/**
 * A custom combobox widget for selecting resource items like gradients or patterns.
 */
class KOWIDGETS_EXPORT KoResourceSelector : public QComboBox
{
    Q_OBJECT
public:
    enum DisplayMode {
        ImageMode, ///< Displays image of resources (default)
        TextMode ///< Displays name of resources
    };

    /**
     * Constructs a new resource selector.
     * @param parent the parent widget
     */
    explicit KoResourceSelector(QWidget *parent = nullptr);

    /**
     * Constructs a new resource selector showing the resources of the given resource adapter.
     * @param resourceAdapter the resource adapter providing the resources to display
     * @param parent the parent widget
     */
    explicit KoResourceSelector(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QWidget *parent = nullptr);

    /// Destroys the resource selector
    ~KoResourceSelector() override;

    /// Sets the resource adaptor to get resources from
    void setResourceAdapter(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter);

    /// Sets the display mode
    void setDisplayMode(DisplayMode mode);

    /// Sets number of columns to display in the popup view
    void setColumnCount(int columnCount);

    /// Sets the height of the popup view rows
    void setRowHeight(int rowHeight);

Q_SIGNALS:
    /// Emitted when a resource was selected
    void resourceSelected(KoResource *resource);

    /// Is emitted when the user has clicked on the current resource
    void resourceApplied(KoResource *resource);

protected:
    /// reimplemented
    void paintEvent(QPaintEvent *) override;
    /// reimplemented
    void mousePressEvent(QMouseEvent *) override;
    /// reimplemented
    void mouseMoveEvent(QMouseEvent *event) override;

private Q_SLOTS:
    void indexChanged(int index);
    void resourceAdded(KoResource *);
    void resourceRemoved(KoResource *);

private:
    class Private;
    Private *const d;
};

#endif // KORESOURCESELECTOR_H
