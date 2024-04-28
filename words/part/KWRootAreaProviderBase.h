/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebastian.sauer@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

    KWTextFrameSet *frameSet()
    {
        return m_textFrameSet;
    }

private:
    KWTextFrameSet *m_textFrameSet;
};

#endif
