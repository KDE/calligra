/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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

#include <qdatetime.h>
#include <QString>
#include <qstringlist.h>

class QDomElement;

namespace KPlato
{

class Context {
public:
    Context();
    virtual ~Context();
    
    virtual bool load(QDomElement &element);
    virtual void save(QDomElement &element) const;
    

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

};

}  //KPlato namespace

#endif //CONTEXT_H
