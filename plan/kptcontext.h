/* This file is part of the KDE project
   Copyright (C) 2005, 2007 Dag Andersen <danders@get2net.dk>

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


#ifndef KPTCONTEXT_H
#define KPTCONTEXT_H

#include <QDateTime>
#include <QString>
#include <QStringList>

#include <KoXmlReader.h>

namespace KPlato
{

class View;

class Context {
public:
    Context();
    virtual ~Context();
    
    virtual bool load( const KoXmlDocument &doc );
    virtual QDomDocument save( const View *view ) const;
    const KoXmlElement &context() const;
    bool isLoaded() const { return m_contextLoaded; }

    bool setContent( const QString &str );

    // View
    QString currentView;
    int currentEstimateType;
    long currentSchedule;
    bool actionViewExpected;
    bool actionViewOptimistic;
    bool actionViewPessimistic;

    struct Ganttview {
        int ganttviewsize;
        int taskviewsize;
        QString currentNode;
        bool showResources;
        bool showTaskName;
        bool showTaskLinks;
        bool showProgress;
        bool showPositiveFloat;
        bool showCriticalTasks;
        bool showCriticalPath;
        bool showNoInformation;
        QStringList closedNodes;
    } ganttview;    
    
    struct Pertview {
    } pertview;
    
    struct Resourceview {
    } resourceview;
    
    struct Accountsview {
        int accountsviewsize;
        int periodviewsize;
        QDate date;
        int period;
        bool cumulative;
        QStringList closedItems;
    } accountsview;
    
    struct Reportview {
    } reportview;

private:
    bool m_contextLoaded;
    KoXmlElement m_context;
    KoXmlDocument m_document;
};

}  //KPlato namespace

#endif //CONTEXT_H
