/* This file is part of the wvWare 2 project
   SPDX-FileCopyrightText: 2002-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef PARAGRAPHPROPERTIES_H
#define PARAGRAPHPROPERTIES_H

#include "word97_generated.h"
#include "wv2_export.h"

class QString;

namespace wvWare
{

class ListInfo;
class ListInfoProvider;

/**
 * A tiny helper class to wrap the PAP and any additional information
 * we want to pass to the consumer. Right now we have a ListInfo object
 * if the paragraph belongs to some list.
 */
class WV2_EXPORT ParagraphProperties : public Shared
{
public:
    ParagraphProperties();
    explicit ParagraphProperties(const Word97::PAP &pap);
    ParagraphProperties(const ParagraphProperties &rhs);
    ~ParagraphProperties() override;

    Word97::PAP &pap();
    const Word97::PAP &pap() const;
    /**
     * If this paragraph belongs to a list, the ListInfo object will be
     * valid and contain useful information about the formatting of the
     * list counter. @return 0 if the paragraph is not inside of a list
     */
    const ListInfo *listInfo() const;

    /**
     * Set the name of the bullet picture into ListInfo.
     */
    void setBulletPictureName(const QString &name);

    /**
     * @internal
     * @param list info provider
     * @param pragraph mark's CHPs
     */
    void createListInfo(ListInfoProvider &listInfoProvider, Word97::CHP &chp);

private:
    ParagraphProperties &operator=(const ParagraphProperties &rhs) = delete;

    Word97::PAP m_pap;
    ListInfo *m_listInfo;
};

} // namespace wvWare

#endif // PARAGRAPHPROPERTIES_H
