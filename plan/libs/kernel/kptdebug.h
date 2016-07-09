/* This file is part of the KDE project
  Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTDEBUG_H
#define KPTDEBUG_H

#include "kplatokernel_export.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QString>

extern const KPLATOKERNEL_EXPORT QLoggingCategory &PLAN_LOG();

#define debugPlan qCDebug(PLAN_LOG)<<QString("%2:%3::%1:").arg(__func__).arg(QString(__FILE__).right(QString(__FILE__).length()-QString(__FILE__).lastIndexOf('/'))).arg(__LINE__)
#define warnPlan qCWarning(PLAN_LOG)
#define errorPlan qCCritical(PLAN_LOG)

extern const KPLATOKERNEL_EXPORT QLoggingCategory &PLANDEPEDITOR_LOG();

#define debugPlanDepEditor qCDebug(PLANDEPEDITOR_LOG)
#define warnPlanDepEditor qCWarning(PLANDEPEDITOR_LOG)
#define errorPlanDepEditor qCCritical(PLANDEPEDITOR_LOG)

extern const KPLATOKERNEL_EXPORT QLoggingCategory &PLANXML_LOG();

#define debugPlanXml qCDebug(PLANXML_LOG)
#define warnPlanXml qCWarning(PLANXML_LOG)
#define errorPlanXml qCCritical(PLANXML_LOG)

#endif
