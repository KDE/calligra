// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_FONTTABLEENTRY_H
#define RTFREADER_FONTTABLEENTRY_H
#include "rtfreader_export.h"

namespace RtfReader
{
enum FontFamily {
    Nil,
    Roman,
    Swiss,
    Modern,
    Script,
    Decor,
    Tech,
    Bidi
};

enum FontPitch {
    Default = 0,
    Fixed = 1,
    Variable = 2
};

class RTFREADER_EXPORT FontTableEntry
{
public:
    FontTableEntry()
        : m_fontFamily(Nil)
        , m_fontPitch(Default)
    {
    }

    enum FontFamily fontFamily() const
    {
        return m_fontFamily;
    }

    void setFontFamily(enum FontFamily fontFamily)
    {
        m_fontFamily = fontFamily;
    }

    enum FontPitch fontPitch() const
    {
        return m_fontPitch;
    }

    void setFontPitch(enum FontPitch fontPitch)
    {
        m_fontPitch = fontPitch;
    }

    QString fontName() const
    {
        return m_fontName;
    }

    void setFontName(const QString &fontName)
    {
        m_fontName = fontName;
    }

    QString encoding() const
    {
        return m_encoding;
    }

    void setEncoding(const QString &encoding)
    {
        m_encoding = encoding;
    }

protected:
    enum FontFamily m_fontFamily;
    enum FontPitch m_fontPitch;
    QString m_fontName;
    QString m_encoding;
};
}

#endif
