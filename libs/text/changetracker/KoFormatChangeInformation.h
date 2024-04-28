/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __FORMAT_CHANGE_INFORMATION_H__
#define __FORMAT_CHANGE_INFORMATION_H__

#include <QTextBlockFormat>
#include <QTextCharFormat>

class KoFormatChangeInformation
{
public:
    typedef enum { eTextStyleChange = 0, eParagraphStyleChange, eListItemNumberingChange } FormatChangeType;

    KoFormatChangeInformation::FormatChangeType formatType();

protected:
    explicit KoFormatChangeInformation(KoFormatChangeInformation::FormatChangeType formatChangeType);

private:
    KoFormatChangeInformation::FormatChangeType formatChangeType;
};

class KoTextStyleChangeInformation : public KoFormatChangeInformation
{
public:
    explicit KoTextStyleChangeInformation(KoFormatChangeInformation::FormatChangeType formatChangeType = KoFormatChangeInformation::eTextStyleChange);
    void setPreviousCharFormat(QTextCharFormat &oldFormat);
    QTextCharFormat &previousCharFormat();

private:
    QTextCharFormat previousTextCharFormat;
};

class KoParagraphStyleChangeInformation : public KoTextStyleChangeInformation
{
public:
    KoParagraphStyleChangeInformation();
    void setPreviousBlockFormat(QTextBlockFormat &oldFormat);
    QTextBlockFormat &previousBlockFormat();

private:
    QTextBlockFormat previousTextBlockFormat;
};

class KoListItemNumChangeInformation : public KoFormatChangeInformation
{
public:
    typedef enum { eNumberingRestarted = 0, eRestartRemoved } ListItemNumChangeType;
    explicit KoListItemNumChangeInformation(KoListItemNumChangeInformation::ListItemNumChangeType eSubType);
    void setPreviousStartNumber(int oldRestartNumber);
    KoListItemNumChangeInformation::ListItemNumChangeType listItemNumChangeType();
    int previousStartNumber();

private:
    int oldStartNumber;
    KoListItemNumChangeInformation::ListItemNumChangeType eSubType;
};
#endif
