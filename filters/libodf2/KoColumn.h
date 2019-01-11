/*
 *  Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

    enum Visibility {
        Collapse,
        Filter,
        Visible
    };
    Visibility visibility();
    void setVisibility(Visibility visibility);

private:
    void saveOdf(KoXmlWriter& writer, KoGenStyles& styles);

    KoColumn(const KoColumn&);
    KoColumn& operator=(const KoColumn&);

    KoCellStyle::Ptr m_defaultCellStyle;
    KoColumnStyle::Ptr m_style;
    Visibility m_visibility;
};

#endif
