/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
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
    explicit KPrPresenterViewBaseInterface( const QList<KoPAPageBase *> &pages, QWidget *parent = 0 );

public Q_SLOTS:
    /// set the active page to @p page
    void setActivePage( KoPAPageBase *page );

    /// set the active page number to @p pageIndex
    virtual void setActivePage( int pageIndex );

Q_SIGNALS:
    void activeSlideChanged( KoPAPageBase *page );

protected:
    QList<KoPAPageBase *> m_pages;
    int m_activePage;
};

#endif

