/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
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

#ifndef KPTVIEWLISTDOCKER_H
#define KPTVIEWLISTDOCKER_H

#include <QDockWidget>
#include <KoDockFactoryBase.h>

namespace KPlato
{

class View;
class ViewListWidget;

class ViewListDocker : public QDockWidget
{
    Q_OBJECT

public:
    explicit ViewListDocker(View *view);
    ~ViewListDocker();
    View *view();
    void setView(View *view);
    ViewListWidget *viewList() const { return m_viewlist; }
private:
    View *m_view;
    ViewListWidget *m_viewlist;
};

class ViewListDockerFactory : public KoDockFactoryBase
{
public:
    ViewListDockerFactory(View *view);

    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
    /// @return the dock widget area the widget should appear in by default
    virtual KoDockFactoryBase::DockPosition defaultDockPosition() const { return DockLeft; }

private:
    View *m_view;
};

} //namespace KPlato

#endif
