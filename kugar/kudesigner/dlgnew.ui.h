/***************************************************************************
                          dlgnew.ui.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void dlgNew::init()
{
    pSize = QSize(210, 297);
//    fcReportName->setMode(FileChooser::AnyFile);
}

void dlgNew::cbPageSize_highlighted( const QString &s )
{
    if (s == "A0") pSize = QSize(841, 1189);
    if (s == "A1") pSize = QSize(594, 841);
    if (s == "A2") pSize = QSize(420, 594);
    if (s == "A3") pSize = QSize(297, 420);
    if (s == "A4") pSize = QSize(210, 297);
    if (s == "A5") pSize = QSize(148, 210);
    if (s == "A6") pSize = QSize(105, 148);
    if (s == "A7") pSize = QSize(74, 105);
    if (s == "A8") pSize = QSize(52, 74);
    if (s == "A9") pSize = QSize(37, 52);
    if (s == "B0") pSize = QSize(1030, 1456);
    if (s == "B1") pSize = QSize(728, 1030);
    if (s == "B10") pSize = QSize(32, 45);
    if (s == "B2") pSize = QSize(515, 728);
    if (s == "B3") pSize = QSize(364, 515);
    if (s == "B4") pSize = QSize(257, 346);
    if (s == "B5") pSize = QSize(182, 257);
    if (s == "B6") pSize = QSize(128, 182);
    if (s == "B7") pSize = QSize(91, 128);
    if (s == "B8") pSize = QSize(64, 91);
    if (s == "B9") pSize = QSize(45, 64);
    if (s == "C5E") pSize = QSize(163, 229);
    if (s == "Comm10E") pSize = QSize(105, 241);
    if (s == "DLE") pSize = QSize(110, 220);
    if (s == "Executive") pSize = QSize(191, 254);
    if (s == "Folio") pSize = QSize(210, 330);
    if (s == "Ledger") pSize = QSize(432, 279);
    if (s == "Legal") pSize = QSize(216, 356);
    if (s == "Letter") pSize = QSize(216, 279);
    if (s == "Tabloid") pSize = QSize(279, 432);
}
