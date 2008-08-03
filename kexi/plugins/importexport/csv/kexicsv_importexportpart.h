/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_MIGRATION_PART_H
#define KEXI_MIGRATION_PART_H

#include <core/kexiinternalpart.h>
#include "kexicsvexportwizard.h"

/*! Internal part for CSV data import/export dialogs. */
class KexiCSVImportExportPart : public KexiInternalPart
{
public:
    KexiCSVImportExportPart(QObject *parent, const QStringList &args);
    virtual ~KexiCSVImportExportPart();

    /*! Reimplemented to return wizard object. */
    virtual QWidget *createWidget(const char* widgetClass,
                                  QWidget *parent, const char *objName = 0, QMap<QString, QString>* args = 0);

    /*! Reimplemented to execute a command \a commandName (nonvisual). The result are put into the \a args. */
    virtual bool executeCommand(const char* commandName, QMap<QString, QString>* args = 0);

protected:
};

#endif
