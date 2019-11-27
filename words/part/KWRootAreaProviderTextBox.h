/* This file is part of the KDE project
 * Copyright (C) 2011-2015 C. Boemann <cbo@boemann.dk>
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

#ifndef KWROOTAREAPROVIDERTEXTBOX_H
#define KWROOTAREAPROVIDERTEXTBOX_H

#include <KWRootAreaProviderBase.h>

#include <QList>

class KWTextFrameSet;

class KWRootAreaProviderTextBox : public KWRootAreaProviderBase
{
public:
    explicit KWRootAreaProviderTextBox(KWTextFrameSet *textFrameSet);
    ~KWRootAreaProviderTextBox() override;

    void clearPages(int pageNumber) override;
    void setPageDirty(int pageNumber) override;

    /// reimplemented
    KoTextLayoutRootArea *provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints, int requestedPosition, bool *isNewArea) override;
    void releaseAllAfter(KoTextLayoutRootArea *afterThis) override;
    void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea) override;
    QRectF suggestRect(KoTextLayoutRootArea *rootArea) override;
private:
    QList<KoTextLayoutRootArea*> m_rootAreaCache;
};

#endif
