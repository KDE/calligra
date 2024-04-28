/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Words.h"
#include "frames/KWTextFrameSet.h"

#include <KLocalizedString>

namespace Words
{

bool isAutoGenerated(KWFrameSet *fs)
{
    Q_ASSERT(fs);
    if (fs->type() == BackgroundFrameSet)
        return true;
    if (fs->type() == TextFrameSet) {
        KWTextFrameSet *tfs = static_cast<KWTextFrameSet *>(fs);
        return tfs
            && (tfs->textFrameSetType() == Words::OddPagesHeaderTextFrameSet || tfs->textFrameSetType() == Words::EvenPagesHeaderTextFrameSet
                || tfs->textFrameSetType() == Words::OddPagesFooterTextFrameSet || tfs->textFrameSetType() == Words::EvenPagesFooterTextFrameSet
                || tfs->textFrameSetType() == Words::MainTextFrameSet);
    }
    return false;
}

bool isHeaderFooter(KWTextFrameSet *tfs)
{
    return tfs
        && (tfs->textFrameSetType() == Words::OddPagesHeaderTextFrameSet || tfs->textFrameSetType() == Words::EvenPagesHeaderTextFrameSet
            || tfs->textFrameSetType() == Words::OddPagesFooterTextFrameSet || tfs->textFrameSetType() == Words::EvenPagesFooterTextFrameSet);
}

QString frameSetTypeName(TextFrameSetType type)
{
    switch (type) {
    case Words::OddPagesHeaderTextFrameSet:
        return i18n("Odd Pages Header");
    case Words::EvenPagesHeaderTextFrameSet:
        return i18n("Even Pages Header");
    case Words::OddPagesFooterTextFrameSet:
        return i18n("Odd Pages Footer");
    case Words::EvenPagesFooterTextFrameSet:
        return i18n("Even Pages Footer");
    case Words::MainTextFrameSet:
        return i18n("Main text");
    case Words::OtherTextFrameSet:
        return i18n("Other text");
    }
    return QString();
}

QString frameSetTypeName(KWFrameSet *frameset)
{
    if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet *>(frameset))
        return frameSetTypeName(tfs->textFrameSetType());
    return QString();
}

}
