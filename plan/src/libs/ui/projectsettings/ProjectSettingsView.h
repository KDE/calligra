/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef PROJECTSETTINGSVIEW_H
#define PROJECTSETTINGSVIEW_H

#include "kplatoui_export.h"
#include "ui_ProjectSettingsView.h"
#include "kptviewbase.h"


class KoDocument;

class QUrl;
class QPoint;


namespace KPlato
{


class KPLATOUI_EXPORT ProjectSettingsView : public ViewBase
{
    Q_OBJECT
public:
    ProjectSettingsView(KoPart *part, KoDocument *doc, QWidget *parent);

    bool openHtml( const QUrl &url );

    void setupGui();

    virtual void updateReadWrite( bool readwrite );

    KoPrintJob *createPrintJob();


public Q_SLOTS:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

Q_SIGNALS:
    void connectResources(const QString &file);

protected:
    void updateActionsEnabled(  bool on = true );

private Q_SLOTS:
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    
    void slotEnableActions( bool on );

    void slotOpenResourcesFile();
    void slotResourcesConnect();

private:
    Ui::ProjectSettingsView widget;
    
};

}  //KPlato namespace

#endif
