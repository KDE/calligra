/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <QString>

#define PanTool_ID "PanTool"
#define TextTool_ID "TextToolFactory_ID"

/*!
* Size of full screen button
*/
#define FS_BUTTON_SIZE 64

/*!
* Path for full screen icon
*/
const QString FS_BUTTON_PATH("/usr/share/icons/hicolor/48x48/hildon/general_fullsize.png");

/*!
* Path for presentation back icon in full screen mode
*/
const QString FS_PPT_BACK_BUTTON_PATH("/usr/share/icons/hicolor/48x48/hildon/general_back.png");

/*!
* Path for presentation forward icon in full screen mode
*/
const QString FS_PPT_FORWARD_BUTTON_PATH("/usr/share/icons/hicolor/48x48/hildon/general_forward.png");

/*!
* Path for splash screen KOffice logo
*/
const QString SPLASH_LOGO_KOFFICE_PATH(":/images/178x200/freoffice.png");
/*!
 * * KOffice pixmap in about dialog
 */
const QString ABOUT_DIALOG_KOFFICE_PIXMAP("/usr/share/icons/hicolor/64x64/apps/freoffice.png");
/*!
 * * Nokia pixmap in about dialog
 * */
const QString ABOUT_DIALOG_NOKIA_PIXMAP("/usr/share/icons/hicolor/scalable/hildon/about_nokia_logo.png");

/*!
* extensions
*/
const QString EXT_PPT("ppt");
const QString EXT_PPTX("pptx");
const QString EXT_ODP("odp");
const QString EXT_DOC("doc");
const QString EXT_DOCX("docx");
const QString EXT_ODT("odt");
const QString EXT_TXT("txt");
const QString EXT_RTF("rtf");
const QString EXT_ODS("ods");
const QString EXT_XLS("xls");
const QString EXT_XLSX("xlsx");

/*!
* Style for full screen button
*/
const QString FS_BUTTON_STYLE_SHEET("background-color: black; \
                                     border: none; \
                                     border-radius: 5px;");
/*!
 * DBus FreOffice Path
 */
const QString FREOFFICE_PATH("/com/nokia/FreOffice");

/*!
 * DBus FreOffice ServiceName
 */
const QString FREOFFICE_SERVICE_NAME("com.nokia.FreOffice");

const QString FILE_CHOOSER_FILTER("Document(*.doc *.docx *.odt *.txt);;Presentation(*.ppt *.pptx *.odp);;Spreadsheet(*.xls *.xlsx *.ods)");

const QString FREOFFICE_APPLICATION_PATH("/usr/bin/FreOffice");
#endif // COMMON_H
