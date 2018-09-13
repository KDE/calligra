/* This file is part of the KDE project
 * Copyright (C) 2010-2011 Carlos Licea <carlos@kdab.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

protected:
    KoFilter::ConversionStatus read_tbl();
    KoFilter::ConversionStatus read_tblPr();
    KoFilter::ConversionStatus read_tblGrid();
    KoFilter::ConversionStatus read_tr();
    KoFilter::ConversionStatus read_tableStyleId();
    KoFilter::ConversionStatus read_gridCol();
    KoFilter::ConversionStatus read_tc();
    KoFilter::ConversionStatus read_tcPr();
    KoFilter::ConversionStatus read_lnT();
    KoFilter::ConversionStatus read_lnB();
    KoFilter::ConversionStatus read_lnR();
    KoFilter::ConversionStatus read_lnL();

    void defineStyles();

    uint m_currentTableNumber; //!< table counter, from 0
    uint m_currentTableRowNumber; //!< row counter, from 0, initialized in read_tbl()
    uint m_currentTableColumnNumber; //!< column counter, from 0, initialized in read_tr()

    KoTable::Ptr m_table;
    QString m_currentTableName;

    MSOOXML::DrawingTableStyle* m_tableStyle;
    MSOOXML::DrawingTableStyleConverterProperties::Roles m_activeRoles;

    MSOOXML::TableStyleProperties* m_currentLocalStyleProperties;
    MSOOXML::LocalTableStyles m_localTableStyles;

    #include "PresetDrawingMLTables.h"
