/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTVIEWLISTDIALOG_H
#define KPTVIEWLISTDIALOG_H

#include "ui_kptviewlistaddview.h"
#include "ui_kptviewlisteditview.h"

#include <kdialog.h>

#include <QWidget>

class KoView;

namespace KPlato
{

class View;
class ViewListWidget;
class ViewListItem;
class AddViewPanel;
class EditViewPanel;

class ViewListDialog : public KDialog
{
    Q_OBJECT
public:
    ViewListDialog( View *view, ViewListWidget &viewlist, QWidget *parent=0 );

protected slots:
    void slotOk();
    
private:
    AddViewPanel *m_panel;
};

class AddViewPanel : public QWidget
{
    Q_OBJECT
    public:
        AddViewPanel( View *view, ViewListWidget &viewlist, QWidget *parent );

        bool ok();
    
        Ui::AddViewPanel widget;
    
    signals:
        void enableButtonOk( bool );
    
    protected slots:
        void changed();

    private:
        View *m_view;
        ViewListWidget &m_viewlist;
        QMap<QString, QString> m_categories;
};

class ViewListEditDialog : public KDialog
{
    Q_OBJECT
public:
    ViewListEditDialog( ViewListWidget &viewlist, ViewListItem *item, QWidget *parent=0 );

protected slots:
    void slotOk();

private:
    EditViewPanel *m_panel;
};

class EditViewPanel : public QWidget
{
    Q_OBJECT
public:
    EditViewPanel( ViewListWidget &viewlist, ViewListItem *item, QWidget *parent );

    bool ok();

    Ui::EditViewPanel widget;

signals:
    void enableButtonOk( bool );

protected slots:
    void changed();
    void categoryChanged( int index );
    void fillBefore( ViewListItem *cat );

private:
    ViewListItem *m_item;
    ViewListWidget &m_viewlist;
};

} //KPlato namespace

#endif // CONFIGDIALOG_H
