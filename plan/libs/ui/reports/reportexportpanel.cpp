/* This file is part of the KDE project
  Copyright (C) 2010, 2011 Dag Andersen <danders@get2net.dk>

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

#include "reportexportpanel.h"
#include "report.h"

#include <KoIcon.h>

#include <kdebug.h>

namespace KPlato
{

ReportExportPanel::ReportExportPanel( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    ui_format->insertItem (0, koIcon("application-vnd.oasis.opendocument.text"), i18n("Open document text (table)"), (int)Reports::EF_OdtTable);
    ui_format->insertItem (1, koIcon("application-vnd.oasis.opendocument.text"), i18n("Open document text (frames)"), (int)Reports::EF_OdtFrames);
    ui_format->insertItem (2, koIcon("application-vnd.oasis.opendocument.spreadsheet"), i18n("Open document spreadsheet"), (int)Reports::EF_Ods);
    ui_format->insertItem (3, koIcon("text-html"), i18n("HTML"), (int)Reports::EF_Html);
    ui_format->insertItem (4, koIcon("application-xhtml+xml"), i18n("XHTML"), (int)Reports::EF_XHtml);
}

int ReportExportPanel::selectedFormat() const
{
    int result = ui_format->itemData( ui_format->currentIndex() ).toInt();
    return result;
}

} //namespace KPlato

#include "reportexportpanel.moc"
