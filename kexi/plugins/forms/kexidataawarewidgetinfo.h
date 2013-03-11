/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDATAAWAREWIDGETINFO_H
#define KEXIDATAAWAREWIDGETINFO_H

#include <formeditor/widgetfactory.h>
#include <formeditor/WidgetInfo.h>

//! A widget info for data-aware widgets
/*! Used within factories just like KFormDesigner::WidgetInfo,
 but also predefines specific behaviour,
 e.g. sets autoSync flag to false for "dataSource" property.
*/
class KEXIFORMUTILS_EXPORT KexiDataAwareWidgetInfo : public KFormDesigner::WidgetInfo
{
public:
    KexiDataAwareWidgetInfo(KFormDesigner::WidgetFactory *f);

//2.0    KexiDataAwareWidgetInfo(KFormDesigner::WidgetFactory *f,
//2.0                            const char* parentFactoryName, const char* inheritedClassName = 0);

    virtual ~KexiDataAwareWidgetInfo();

    //! Sets flag controlling inline editing when data source is set (frue e.g. for line edit, text edit).
    //! True by default.
    void setInlineEditingEnabledWhenDataSourceSet(bool set);

    //! @return flag controlling inline editing when data source is set
    bool inlineEditingEnabledWhenDataSourceSet() const;

private:
    void init();
    class Private;
    Private * const d;
};

#endif
