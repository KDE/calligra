/* This file is part of the KDE project
 * Copyright (C) 2011 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2011 Sebastian Sauer <sebastian.sauer@kdab.com>
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

#ifndef KWROOTAREAPROVIDERBASE_H
#define KWROOTAREAPROVIDERBASE_H

#include "KWPage.h"
#include <KoTextLayoutRootAreaProvider.h>

#include <QMap>
#include <QPair>

class KWTextFrameSet;
class KWPage;

class KWRootAreaProviderBase : public KoTextLayoutRootAreaProvider
{
public:
    explicit KWRootAreaProviderBase(KWTextFrameSet *textFrameSet);

    virtual void clearPages(int startingPageNumber) = 0;
    virtual void setPageDirty(int pageNumber) = 0;

    /// reimplemented
    void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea) override;
    void updateAll() override;
    QRectF suggestRect(KoTextLayoutRootArea *rootArea) override;
    QList<KoTextLayoutObstruction *> relevantObstructions(KoTextLayoutRootArea *rootArea) override;

    KWTextFrameSet *frameSet() {return m_textFrameSet;}

private:
    KWTextFrameSet *m_textFrameSet;
};

#endif
