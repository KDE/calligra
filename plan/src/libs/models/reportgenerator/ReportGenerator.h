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

#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QModelIndexList>

#include <KoXmlReaderForward.h>

#include <qdom.h>
#include <QSortFilterProxyModel>

class QIODevice;
class QString;

class KoXmlWriter;
class KoStore;
class KoOdfWriteStore;
class KoOdfReadStore;

namespace KPlato
{

class Project;
class ScheduleManager;
class ItemModelBase;


class KPLATOMODELS_EXPORT ReportGenerator
{
public:
    explicit ReportGenerator();
    virtual ~ReportGenerator();

    void setReportType(const QString &type);

    void setTemplateFile(const QString &file);
    void setReportFile(const QString &file);
    void setProject(Project *project);
    void setScheduleManager(ScheduleManager *manager);

    virtual bool open();
    virtual void close();

    virtual bool createReport();

    QString lastError() const;

protected:
    QString m_lastError;
    QString m_reportType;
    QString m_templateFile;
    QString m_reportFile;
    Project *m_project;
    ScheduleManager *m_manager;

    ReportGenerator *m_reportGenerator;
};

} //namespace KPlato

#endif // REPORTGENERATOR_H
