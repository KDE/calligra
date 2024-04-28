/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011-2015 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebastian.sauer@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    QList<KoTextLayoutRootArea *> m_rootAreaCache;
};

#endif
