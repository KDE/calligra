/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTERVIEWBASEINTERFACE
#define KPRPRESENTERVIEWBASEINTERFACE

#include <QWidget>

class KoPAPageBase;

/**
 * KPrPresenterViewBaseInterface
 * This widget is the base interface for all widget used as user interface in the
 * presenter view feature. This class can be subclassed to provide different kind
 * of user interface.
 */
class KPrPresenterViewBaseInterface : public QWidget
{
    Q_OBJECT
public:
    explicit KPrPresenterViewBaseInterface(const QList<KoPAPageBase *> &pages, QWidget *parent = nullptr);

public Q_SLOTS:
    /// set the active page to @p page
    void setActivePage(KoPAPageBase *page);

    /// set the active page number to @p pageIndex
    virtual void setActivePage(int pageIndex);

Q_SIGNALS:
    void activeSlideChanged(KoPAPageBase *page);

protected:
    QList<KoPAPageBase *> m_pages;
    int m_activePage;
};

#endif
