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

#include "kexicsv_importexportpart.h"
#include "kexicsvimportdialog.h"
#include "kexicsvexportwizard.h"
#include <core/KexiMainWindowIface.h>
#include <core/kexiproject.h>
#include <kexiutils/utils.h>

#include <kgenericfactory.h>

KexiCSVImportExportPart::KexiCSVImportExportPart(QObject *parent, const QStringList &args)
 : KexiInternalPart(parent, args)
{
}

KexiCSVImportExportPart::~KexiCSVImportExportPart()
{
}

QWidget *KexiCSVImportExportPart::createWidget(const char* widgetClass,
  QWidget *parent, const char *objName, QMap<QString,QString>* args )
{
  if (0==qstrcmp(widgetClass, "KexiCSVImportDialog")) {
    KexiCSVImportDialog::Mode mode = (args && (*args)["sourceType"]=="file")
      ? KexiCSVImportDialog::File : KexiCSVImportDialog::Clipboard;
    KexiCSVImportDialog *dlg = new KexiCSVImportDialog( mode, parent );
    dlg->setObjectName(objName);
    m_cancelled = dlg->cancelled();
    if (m_cancelled) {
      delete dlg;
      return 0;
    }
    return dlg;
  }
  else if (0==qstrcmp(widgetClass, "KexiCSVExportWizard")) {
    if (!args)
      return 0;
    KexiCSVExport::Options options;
    if (!options.assign( *args ))
      return 0;
    KexiCSVExportWizard *dlg = new KexiCSVExportWizard( options, parent );
    dlg->setObjectName(objName);
    m_cancelled = dlg->cancelled();
    if (m_cancelled) {
      delete dlg;
      return 0;
    }
    return dlg;
  }
  return 0;
}

bool KexiCSVImportExportPart::executeCommand(const char* commandName, 
  QMap<QString,QString>* args)
{
  if (0==qstrcmp(commandName, "KexiCSVExport")) {
    KexiCSVExport::Options options;
    if (!options.assign( *args ))
      return false;
    KexiDB::TableOrQuerySchema tableOrQuery(
      KexiMainWindowIface::global()->project()->dbConnection(), options.itemId);
    QTextStream *stream = 0;
    if (args->contains("textStream"))
      stream = KexiUtils::stringToPtr<QTextStream>( (*args)["textStream"] );
    return KexiCSVExport::exportData(tableOrQuery, options, -1, stream);
  }
  return false;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_csv_importexport, 
  KGenericFactory<KexiCSVImportExportPart>("kexihandler_csv_importexport") )
