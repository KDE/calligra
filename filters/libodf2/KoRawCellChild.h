/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
    void saveOdf(KoXmlWriter &writer, KoGenStyles &styles) const override;

private:
    QByteArray m_content;
};

#endif
