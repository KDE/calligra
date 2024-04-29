/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTERVIEWSLIDESINTERFACE
#define KPRPRESENTERVIEWSLIDESINTERFACE

#include "KPrPresenterViewBaseInterface.h"

class KoPAPageBase;
class KoPAPageThumbnailModel;

class QListView;
class QModelIndex;

/**
 * KPrPresenterViewSlidesInterface
 * This widget shows all slides in the presentation
 */
class KPrPresenterViewSlidesInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    explicit KPrPresenterViewSlidesInterface(const QList<KoPAPageBase *> &pages, QWidget *parent = nullptr);

Q_SIGNALS:
    void selectedPageChanged(int index, bool doubleClicked);

private Q_SLOTS:
    void itemClicked(const QModelIndex &index);
    void itemDoubleClicked(const QModelIndex &index);

private:
    KoPAPageThumbnailModel *m_thumbnailModel;
    QListView *m_listView;
};

#endif
