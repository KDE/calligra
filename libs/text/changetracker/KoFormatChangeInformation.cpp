/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormatChangeInformation.h"

KoFormatChangeInformation::KoFormatChangeInformation(KoFormatChangeInformation::FormatChangeType formatChangeType)
{
    this->formatChangeType = formatChangeType;
}

KoFormatChangeInformation::FormatChangeType KoFormatChangeInformation::formatType()
{
    return formatChangeType;
}

KoTextStyleChangeInformation::KoTextStyleChangeInformation(KoFormatChangeInformation::FormatChangeType formatChangeType)
    : KoFormatChangeInformation(formatChangeType)
{
}

void KoTextStyleChangeInformation::setPreviousCharFormat(QTextCharFormat &previousFormat)
{
    this->previousTextCharFormat = previousFormat;
}

QTextCharFormat &KoTextStyleChangeInformation::previousCharFormat()
{
    return this->previousTextCharFormat;
}

KoParagraphStyleChangeInformation::KoParagraphStyleChangeInformation()
    : KoTextStyleChangeInformation(KoFormatChangeInformation::eParagraphStyleChange)
{
}

void KoParagraphStyleChangeInformation::setPreviousBlockFormat(QTextBlockFormat &previousFormat)
{
    this->previousTextBlockFormat = previousFormat;
}

QTextBlockFormat &KoParagraphStyleChangeInformation::previousBlockFormat()
{
    return this->previousTextBlockFormat;
}

KoListItemNumChangeInformation::KoListItemNumChangeInformation(KoListItemNumChangeInformation::ListItemNumChangeType type)
    : KoFormatChangeInformation(KoFormatChangeInformation::eListItemNumberingChange)
    , eSubType(type)
{
}

void KoListItemNumChangeInformation::setPreviousStartNumber(int oldStartNumber)
{
    this->oldStartNumber = oldStartNumber;
}

KoListItemNumChangeInformation::ListItemNumChangeType KoListItemNumChangeInformation::listItemNumChangeType()
{
    return eSubType;
}

int KoListItemNumChangeInformation::previousStartNumber()
{
    return oldStartNumber;
}
