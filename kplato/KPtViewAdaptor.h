/*  This file is part of the KDE project
    Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
              (C) 2004 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA
*/

#ifndef KPT_VIEW_ADAPTOR_H
#define KPT_VIEW_ADAPTOR_H

#include <KoViewAdaptor.h>

#include <QString>
#include <qrect.h>
#include <QColor>

namespace KPlato
{

class View;

class ViewAdaptor : public KoViewAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.projectmanaging.view")

public:
    ViewAdaptor( View* );
    virtual ~ViewAdaptor();

public Q_SLOTS:
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewGantt();
    void slotViewPert();
    void slotViewResources();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotConfigure();

private:
    View* m_view;
};

}  //KPlato namespace

#endif
