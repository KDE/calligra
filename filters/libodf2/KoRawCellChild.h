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

#ifndef KORAWCELLCHILD_H
#define KORAWCELLCHILD_H

#include <QByteArray>

#include "KoCellChild.h"
#include "koodf2_export.h"


/**
 * \class KoRawCellChild
 * \brief This class is a Cell child that can take any given QBuffer 
 * and will insert its contents blindly.
 * 
 * Its purpose is to allow the user to insert custom elements or elements for
 * which the appropriate class has not been provided.
 *
 * \note The class will insert its content "blindly," so, invalid XML or ODF *can* be created.
 * It's the user's responsibility to ensure that it's not the case.
 * \note KoRawCellData takes ownership of the given buffer.
 */
class KOODF2_EXPORT KoRawCellChild : public KoCellChild
{
public:
    explicit KoRawCellChild(const QByteArray &content);
    ~KoRawCellChild() override;

protected:
    void saveOdf(KoXmlWriter& writer, KoGenStyles& styles) const override;

private:
    QByteArray m_content;
};

#endif
