/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAPAGETHUMBNAILMODEL_H
#define KOPAPAGETHUMBNAILMODEL_H

#include <QAbstractListModel>
#include <QSize>

#include "kopageapp_export.h"

class KoPAView;
class KoPAPageBase;

/**
 * Model class for the page thumbnails widget. This class is intended as a simple model to
 * create a list view of available pages. Example usage is widget for choosing master page
 * and slide/page sorter widget.
 *
 * XXX: Isn't that duplicating the model for the document section box? (boud)
 */
class KOPAGEAPP_EXPORT KoPAPageThumbnailModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit KoPAPageThumbnailModel(const QList<KoPAPageBase *> &pages, QObject *parent = nullptr);
    ~KoPAPageThumbnailModel() override;

    // from QAbstractItemModel
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setIconSize(const QSize &size);

private:
    KoPAView *m_view;
    QList<KoPAPageBase *> m_pages;

    int m_iconWidth;
    QSize m_iconSize;
};

#endif
