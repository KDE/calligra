/* This file is part of the KDE project
   Copyright (C) 2006 - 2007 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef XMLLOADEROBJECT_H
#define XMLLOADEROBJECT_H

#include "kplatokernel_export.h"

#include "kptproject.h"
#include "kptdatetime.h"

#include <qdatetime.h>
#include <QString>
#include <qstringlist.h>

namespace KPlato 
{

class KPLATOKERNEL_EXPORT XMLLoaderObject {
public:
    enum Severity { None=0, Errors=1, Warnings=2, Diagnostics=3, Debug=4 };
    XMLLoaderObject()
    : m_project(0),
      m_errors(0),
      m_warnings(0),
      m_logLevel(Diagnostics),
      m_log(),
      m_baseCalendar( 0 )
    {}
    ~XMLLoaderObject() {}
    
    void setProject(Project *proj) { m_project = proj; }
    Project &project() const { return *m_project; }
    
    QString version() const { return m_version; }
    void setVersion( const QString ver ) { m_version = ver; }
    
    QString workVersion() const { return m_workversion; }
    void setWorkVersion( const QString ver ) { m_workversion = ver; }

    const KDateTime::Spec &projectSpec() const { return m_projectSpec; }
    void setProjectSpec( const KDateTime::Spec &spec ) { m_projectSpec = spec; }
    
    void startLoad() {
        m_timer.start();
        m_starttime = QDateTime::currentDateTime();
        m_errors = m_warnings = 0;
        m_log.clear();
        addMsg(QString("Loading started at %1").arg(m_starttime.toString()));
    }
    void stopLoad() { 
        m_elapsed = m_timer.elapsed();
        addMsg(QString("Loading finished at %1, took %2").arg(QDateTime::currentDateTime().toString()).arg(formatElapsed()));
    }
    QDateTime lastLoaded() const { return m_starttime; }
    int elapsed() const { return m_elapsed; }
    QString formatElapsed() { return QString("%1 seconds").arg((double)m_elapsed/1000); }
    
    void setLogLevel(Severity sev) { m_logLevel = sev; }
    const QStringList &log() const { return m_log; }
    void error( QString msg ) { addMsg( Errors, msg ); }
    void warning( QString msg ) { addMsg( Errors, msg ); }
    void diagnostic( QString msg ) { addMsg( Diagnostics, msg ); }
    void debug( QString msg ) { addMsg( Debug, msg ); }
    void message( QString msg ) { addMsg( None, msg ); }
    void addMsg(int sev, QString msg) {
        increment(sev);
        if (m_logLevel < sev) return;
        QString s;
        if (sev == Errors) s = "ERROR";
        else if (sev == Warnings) s = "WARNING";
        else if (sev == Diagnostics) s = "Diagnostic";
        else if (sev == Debug) s = "Debug";
        else s = "Message";
        m_log<<QString("%1: %2").arg(s, 13).arg(msg);
    }
    void addMsg(QString msg) { m_log<<msg; }
    void increment(int sev) {
        if (sev == Errors) { incErrors(); return; }
        if (sev == Warnings) { incWarnings(); return; }
    }
    void incErrors() { ++m_errors; }
    int errors() const { return m_errors; }
    bool error() const { return m_errors > 0; }
    void incWarnings() { ++m_warnings; }
    int warnings() const { return m_warnings; }
    bool warning() const { return m_warnings > 0; }

    // help to handle version < 0.6
    void setBaseCalendar( Calendar *cal ) { m_baseCalendar = cal; }
    Calendar *baseCalendar() const { return m_baseCalendar; }
    
protected:
    Project *m_project;
    int m_errors;
    int m_warnings;
    int m_logLevel;
    QStringList m_log;
    QDateTime m_starttime;
    QTime m_timer;
    int m_elapsed;
    QString m_version;
    QString m_workversion;
    KDateTime::Spec m_projectSpec;
    
    Calendar *m_baseCalendar; // help to handle version < 0.6
};

} //namespace KPlato

#endif
