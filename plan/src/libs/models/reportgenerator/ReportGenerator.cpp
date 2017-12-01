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

#include "kplatomodels_export.h"
#include "ReportGenerator.h"

#include "ReportGeneratorOdt.h"


#include <KLocalizedString>

namespace KPlato
{

ReportGenerator::ReportGenerator()
    : m_project(0)
    , m_manager(0)
    , m_reportGenerator(0)
{
}


ReportGenerator::~ReportGenerator()
{
    close();
}

void ReportGenerator::setReportType(const QString &type)
{
    m_reportType = type;
}

void ReportGenerator::setTemplateFile(const QString &file)
{
    m_templateFile = file;
}

void ReportGenerator::setReportFile(const QString &file)
{
    m_reportFile = file;
}

void ReportGenerator::setProject(Project *project)
{
    m_project = project;
}

void ReportGenerator::setScheduleManager(ScheduleManager *manager)
{
    m_manager = manager;
}

bool ReportGenerator::open()
{
    m_lastError.clear();
    if (m_reportType == "odt") {
        m_reportGenerator = new ReportGeneratorOdt();
        m_reportGenerator->setTemplateFile(m_templateFile);
        m_reportGenerator->setReportFile(m_reportFile);
        m_reportGenerator->setProject(m_project);
        m_reportGenerator->setScheduleManager(m_manager);
        return m_reportGenerator->open();
    }
    m_lastError = i18n("Unknown report type: %1", m_reportType);
    return false;
}

void ReportGenerator::close()
{
    delete m_reportGenerator;
    m_reportGenerator = 0;
}

bool ReportGenerator::createReport()
{
    if (!m_reportGenerator) {
        m_lastError = i18n("The report generator has not been opened successfully.");
        return false;
    }
    return m_reportGenerator->createReport();
}

QString ReportGenerator::lastError() const
{
    if (m_reportGenerator) {
        return m_reportGenerator->lastError();
    }
    return m_lastError;
}

} //namespace KPlato
