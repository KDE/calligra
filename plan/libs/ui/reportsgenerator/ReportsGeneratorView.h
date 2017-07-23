/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef REPORTSGENERATORVIEW_H
#define REPORTSGENERATORVIEW_H

#include "kplatoui_export.h"

#include "kptglobal.h"
#include "kptviewbase.h"

class KoDocument;

class KActionMenu;

class QWidget;
class QTreeView;

namespace KPlato
{


class KPLATOUI_EXPORT ReportsGeneratorView : public ViewBase
{
    Q_OBJECT
public:
    ReportsGeneratorView(KoPart *part, KoDocument *doc, QWidget *parent);

    void setupGui();

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;


public Q_SLOTS:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void slotAddReport();
    void slotRemoveReport();
    void slotGenerateReport();

protected:
    void updateActionsEnabled( bool on );
    int selectedRowCount() const;
    QModelIndexList selectedRows() const;

    bool generateReport(const QString &templateFile, const QString &file);

protected Q_SLOTS:
    virtual void slotOptions();

private Q_SLOTS:
    void slotSelectionChanged();
    void slotCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested(const QPoint &pos);

    void slotEnableActions();

private:
    QTreeView *m_view;

    QAction *actionAddReport;
    QAction *actionRemoveReport;
    QAction *actionGenerateReport;
};


} //namespace KPlato


#endif
