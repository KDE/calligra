/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <kplato@kde.org>

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

#ifndef KPTHTMLVIEW_H
#define KPTHTMLVIEW_H

#include "kplatoui_export.h"

#include "kptviewbase.h"

#include <KHTMLPart>
#include <KUrl>

class KoDocument;

class QPoint;


namespace KPlato
{


class KPLATOUI_EXPORT HtmlView : public ViewBase
{
    Q_OBJECT
public:
    HtmlView( KoDocument *part, QWidget *parent );
    
    bool openHtml( const KUrl &url );

    void setupGui();

    virtual void updateReadWrite( bool readwrite );

    KoPrintJob *createPrintJob();

    KHTMLPart &htmlPart() { return m_htmlPart; }
    const KHTMLPart &htmlPart() const { return m_htmlPart; }

signals:
    void openUrlRequest( HtmlView*, const KUrl& );

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void slotOpenUrlRequest(const KUrl &url, const KParts::OpenUrlArguments &arguments=KParts::OpenUrlArguments(), const KParts::BrowserArguments &browserArguments=KParts::BrowserArguments());

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotEnableActions( bool on );

private:
    KHTMLPart m_htmlPart;

};

}  //KPlato namespace

#endif
