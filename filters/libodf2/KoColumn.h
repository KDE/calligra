/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOCOLUMN_H
#define KOCOLUMN_H

#include "KoCellStyle.h"
#include "KoColumnStyle.h"

#include "koodf2_export.h"

class KoXmlWriter;
class KoGenStyles;

/**
 * \class KoColumn
 * \brief represents a column inside a table.
 * its properties aren't shared unlike the KoStyle's it contains.
 */

class KOODF2_EXPORT KoColumn
{
    friend class KoTable;
    KoColumn();

public:
    ~KoColumn();

    KoColumnStyle::Ptr style();
    void setStyle(KoColumnStyle::Ptr style);

    KoCellStyle::Ptr defualtCellStyle() const;
    void setDefaultCellStyle(KoCellStyle::Ptr defaultStyle);

    enum Visibility { Collapse, Filter, Visible };
    Visibility visibility();
    void setVisibility(Visibility visibility);

private:
    void saveOdf(KoXmlWriter &writer, KoGenStyles &styles);

    KoColumn(const KoColumn &) = delete;
    KoColumn &operator=(const KoColumn &) = delete;

    KoCellStyle::Ptr m_defaultCellStyle;
    KoColumnStyle::Ptr m_style;
    Visibility m_visibility;
};

#endif
