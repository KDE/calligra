/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIWINDOWDATA_H
#define KEXIWINDOWDATA_H

#include <QObject>
#include <kexi_export.h>

//! Privides temporary data shared between KexiWindow's views (KexiView's)
/*! Designed for reimplementation, if needed. */
class KEXICORE_EXPORT KexiWindowData : public QObject
{
public:
    KexiWindowData(QObject* parent);

    ~KexiWindowData();

    /*! Initially false, KexiPart::Part implementation can set this to true
    on data loading (e.g. in loadSchemaData()), to indicate that TextView mode
    could be used instead of DataView or DesignView, because there are problems
    with opening object.

    For example, in KexiQueryPart::loadSchemaData() query statement can be invalid,
    and thus could not be displayed in DesignView mode or executed for DataView.
    So, this flag is set to true and user is asked for confirmation for switching
    to TextView (SQL Editor).

    After switching to TextView, this flag is cleared.
    */
    bool proposeOpeningInTextViewModeBecauseOfProblems;
};

#endif
