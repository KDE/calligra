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

#include "kptcontext.h"
#include "kptview.h"

#include <KoXmlReader.h>

#include <qdom.h>

#include <kdebug.h>

namespace KPlato
{

Context::Context()
    : currentEstimateType(0),
      currentSchedule(0),
      m_contextLoaded( false )
{
    ganttview.ganttviewsize = -1;
    ganttview.taskviewsize = -1;
    
    accountsview.accountsviewsize = -1;
    accountsview.periodviewsize = -1;


}

Context::~Context() {
}

const KoXmlElement &Context::context() const
{
    return m_context;
}

bool Context::load( const KoXmlDocument &document ) {
    m_document = document; // create a copy, document is deleted under our feet
    
    // Check if this is the right app
    KoXmlElement elm = m_document.documentElement();
    QString value = elm.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!";
//        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato" ) {
        kError() << "Unknown mime type " << value;
//        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value ) );
        return false;
    }
    QString m_syntaxVersion = elm.attribute( "version", "0.0" );
    if ( m_syntaxVersion > "0.0" ) {
/*        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlato (syntax version: %1)\n"
                               "Opening it in this version of KPlato will lose some information.", m_syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }*/
    }

#ifdef KOXML_USE_QDOM
    int numNodes = elm.childNodes().count();
#else
    int numNodes = elm.childNodesCount();
#endif
    
    KoXmlNode n = elm.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement element = n.toElement();
        if ( element.tagName() == "context" ) {
            m_context = element;
            m_contextLoaded = true;
/*            currentView = element.attribute("current-view");
            currentEstimateType = element.attribute("estimate-type").toInt();
            currentSchedule = element.attribute("current-schedule").toLong();
            actionViewExpected = element.attribute("view-expected").toInt();
            actionViewOptimistic = element.attribute("view-optimistic").toInt();
            actionViewPessimistic = element.attribute("view-pessimistic").toInt();
        
            QDomNodeList list = element.childNodes();
            for (int i=0; i<list.count(); ++i) {
                if (list.item(i).isElement()) {
                    QDomElement e = list.item(i).toElement();
                    if (e.tagName() == "gantt-view") {
                        ganttview.ganttviewsize = e.attribute("ganttview-size").toInt();
                        ganttview.taskviewsize = e.attribute("taskview-size").toInt();
                        ganttview.currentNode = e.attribute("current-node");
                        ganttview.showResources = e.attribute("show-resources").toInt();
                        ganttview.showTaskName = e.attribute("show-taskname").toInt();
                        ganttview.showTaskLinks = e.attribute("show-tasklinks").toInt();
                        ganttview.showProgress = e.attribute("show-progress").toInt();
                        ganttview.showPositiveFloat = e.attribute("show-positivefloat").toInt();
                        ganttview.showCriticalTasks = attribute("show-criticaltasks").toInt();
                        ganttview.showCriticalPath = e.attribute("show-criticalpath").toInt();
                        ganttview.showNoInformation = e.attribute("show-noinformation").toInt();
        
                        QDomNodeList list = e.childNodes();
                        for (int i=0; i<list.count(); ++i) {
                            if (list.item(i).isElement()) {
                              QDomElement g = list.item(i).toElement();
                                if (g.tagName() == "closed-nodes") {
                                    QDomNodeList list = g.childNodes();
                                    for (int i=0; i<list.count(); ++i) {
                                        if (list.item(i).isElement()) {
                                            QDomElement ei = list.item(i).toElement();
                                            if (ei.tagName() == "node") {
                                                ganttview.closedNodes.append(ei.attribute("id"));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else if (e.tagName() == "accounts-view") {
                        accountsview.accountsviewsize = e.attribute("accountsview-size").toInt();
                        accountsview.periodviewsize = e.attribute("periodview-size").toInt();
                        accountsview.date = QDate::fromString(e.attribute("date"), Qt::ISODate);
                        accountsview.period = e.attribute("period").toInt();
                        accountsview.cumulative = e.attribute("cumulative").toInt();
        
                        QDomNodeList list = e.childNodes();
                        for (int i=0; i<list.count(); ++i) {
                            if (list.item(i).isElement()) {
                                QDomElement g = list.item(i).toElement();
                                if (g.tagName() == "closed-items") {
                                    QDomNodeList list = g.childNodes();
                                    for (int i=0; i<list.count(); ++i) {
                                        if (list.item(i).isElement()) {
                                            QDomElement ei = list.item(i).toElement();
                                            if (ei.tagName() == "account") {
                                                 accountsview.closedItems.append(ei.attribute("name"));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        kError()<<"Unknown tag: "<<e.tagName();
                    }
                }
            }*/
        }
    }
    return true;
}

QDomDocument Context::save( const View *view ) const {
    QDomDocument document( "kplato.context" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "context" );
    doc.setAttribute( "editor", "KPlato" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato" );
    doc.setAttribute( "version", 0.0 );
    document.appendChild( doc );
    
    QDomElement e = doc.ownerDocument().createElement("context");
    doc.appendChild( e );
    view->saveContext( e );
    
/*    me.setAttribute("current-view", currentView);
    me.setAttribute("estimate-type", currentEstimateType);
    me.setAttribute("current-schedule", (qlonglong)currentSchedule);
    me.setAttribute("view-expected", actionViewExpected);
    me.setAttribute("view-optimistic", actionViewOptimistic);
    me.setAttribute("view-pessimistic", actionViewPessimistic);
    // Ganttview
    QDomElement g = me.ownerDocument().createElement("gantt-view");
    me.appendChild(g);
    g.setAttribute("ganttview-size", ganttview.ganttviewsize);
    g.setAttribute("taskview-size", ganttview.taskviewsize);
    g.setAttribute("current-node", ganttview.currentNode);
    g.setAttribute("show-resources", ganttview.showResources);
    g.setAttribute("show-taskname", ganttview.showTaskName);
    g.setAttribute("show-tasklinks", ganttview.showTaskLinks);
    g.setAttribute("show-progress", ganttview.showProgress);
    g.setAttribute("show-positivefloat", ganttview.showPositiveFloat);
    g.setAttribute("show-criticaltasks", ganttview.showCriticalTasks);
    g.setAttribute("show-criticalpath", ganttview.showCriticalPath);
    g.setAttribute("show-noinformation", ganttview.showNoInformation);
    if (!ganttview.closedNodes.isEmpty()) {
        QDomElement e = g.ownerDocument().createElement("closed-nodes");
        g.appendChild(e);
        for (QStringList::ConstIterator it = ganttview.closedNodes.begin(); it != ganttview.closedNodes.end(); ++it) {
            QDomElement c = e.ownerDocument().createElement("node");
            e.appendChild(c);
            c.setAttribute("id", (*it));
        }
    }
    // Accountsview
    QDomElement a = me.ownerDocument().createElement("accounts-view");
    me.appendChild(a);
    a.setAttribute("accountsview-size", accountsview.accountsviewsize);
    a.setAttribute("periodview-size", accountsview.periodviewsize);
    a.setAttribute("date", accountsview.date.toString(Qt::ISODate));
    a.setAttribute("period", accountsview.period);
    a.setAttribute("cumulative", accountsview.cumulative);
    if (!accountsview.closedItems.isEmpty()) {
        QDomElement e = a.ownerDocument().createElement("closed-items");
        a.appendChild(e);
        for (QStringList::ConstIterator it = accountsview.closedItems.begin(); it != accountsview.closedItems.end(); ++it) {
            QDomElement c = e.ownerDocument().createElement("account");
            e.appendChild(c);
            c.setAttribute("name", (*it));
        }
    }*/
    return document;
}

}  //KPlato namespace
