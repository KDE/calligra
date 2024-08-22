/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOROW_H
#define KOROW_H

class KoTable;
#include "KoCellStyle.h"
#include "KoRowStyle.h"

#include "koodf2_export.h"

class KoGenStyles;
class KoXmlWriter;

/**
 * \class KoRow
 * \brief This class represents a row inside a table.
 * its properties aren't shared unlike the KoStyle's it contains.
 */

class KOODF2_EXPORT KoRow
{
    friend class KoTable;
    KoRow();

public:
    ~KoRow();

    KoRowStyle::Ptr style();
    void setStyle(KoRowStyle::Ptr style);

    KoCellStyle::Ptr defualtCellStyle() const;
    void setDefaultCellStyle(KoCellStyle::Ptr defaultStyle);

    enum Visibility { Collapse, Filter, Visible };
    Visibility visibility();
    void setVisibility(Visibility visibility);

private:
    void saveOdf(KoXmlWriter &writer, KoGenStyles &styles);
    void finishSaveOdf(KoXmlWriter &writer, KoGenStyles &styles);

    KoRow(const KoRow &) = delete;
    KoRow &operator=(const KoRow &) = delete;

    KoCellStyle::Ptr m_defaultCellStyle;
    KoRowStyle::Ptr m_style;
    Visibility m_visibility;
};

#endif
