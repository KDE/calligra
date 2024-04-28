/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010-2011 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

    MSOOXML::DrawingTableStyle *m_tableStyle;
    MSOOXML::DrawingTableStyleConverterProperties::Roles m_activeRoles;

    MSOOXML::LocalTableStyles m_localTableStyles;

#include "PresetDrawingMLTables.h"
