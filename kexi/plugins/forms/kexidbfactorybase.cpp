/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include <q3scrollview.h>
#include <QCursor>
#include <QPainter>
#include <QStyle>

#include <KLocale>
#include <KDebug>
#include <KIconLoader>
#include <KActionCollection>
#include <KStandardAction>
#include <KPluginFactory>

#include <formeditor/container.h>
#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/objecttree.h>
#include <formeditor/utils.h>
#include <formeditor/widgetlibrary.h>
#include <core/kexi.h>
#include <core/kexipart.h>
#include <core/KexiMainWindowIface.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>
#include <kexiutils/utils.h>
#include <widget/properties/KexiCustomPropertyFactory.h>
#include <widget/utils/kexicontextmenuutils.h>
#include <kexi_global.h>

#include "kexiformview.h"
#include "kexidataawarewidgetinfo.h"

#include "kexidbfactorybase.h"
#include <widget/dataviewcommon/kexiformdataiteminterface.h>


//////////////////////////////////////////

KexiDBFactoryBase::KexiDBFactoryBase(QObject *parent, const char *name)
        : KFormDesigner::WidgetFactory(parent, name)
{
    m_propDesc["dataSource"] = i18n("Data Source");

    //used in labels, frames...
    m_propDesc["frameColor"] = i18n("Frame Color");

    m_propDesc["readOnly"] = i18n("Read Only");
}

KexiDBFactoryBase::~KexiDBFactoryBase()
{
}

bool KexiDBFactoryBase::isPropertyVisibleInternal(
    const QByteArray& classname, QWidget *w,
    const QByteArray& property, bool isTopLevel)
{
    //general
    if (property == "dataSource" || property == "dataSourcePartClass") {
        return false; //force
    }
    return WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

#include "kexidbfactorybase.moc"
