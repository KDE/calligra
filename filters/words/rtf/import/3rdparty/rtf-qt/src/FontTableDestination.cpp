// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "FontTableDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"

namespace
{
struct CharsetEntry {
    int id;
    const char *name;
};

static constexpr CharsetEntry charsetToCodec[] = {
    {0, "Windows-1252"}, // ANSI
    {1, "Windows-1252"}, // Default
    {2, nullptr}, // Symbol,
    {3, nullptr}, // Invalid,
    {77, "Macintosh"}, // Mac
    {128, "Shift-JIS"}, // Shift Jis
    {129, "CP949"}, // Hangul
    {130, nullptr}, // Johab
    {134, "GB2312"}, // GB2312
    {136, "Big5"}, // Big5
    {161, "Windows-1253"}, // Greek
    {162, "Windows-1254"}, // Turkish
    {163, "Windows-1258"}, // Vietnamese
    {177, "Windows-1255"}, // Hebrew
    {178, "Windows-1256"}, // Arabic
    {179, nullptr}, // Arabic Traditional
    {180, nullptr}, // Arabic user
    {181, nullptr}, // Hebrew user
    {186, "Windows-1257"}, // Baltic
    {204, "Windows-1251"}, // Russian
    {222, "CP847"}, // Thai
    {238, "Windows-1250"}, // Eastern European
    {254, "IBM437"}, // PC 437
};

}

namespace RtfReader
{
FontTableDestination::FontTableDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
}

FontTableDestination::~FontTableDestination() = default;

void FontTableDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(hasValue);
    if (controlWord == "f") {
        m_currentFontTableIndex = value;
    } else if (controlWord == "froman") {
        m_fontTableEntry.setFontFamily(Roman);
    } else if (controlWord == "fswiss") {
        m_fontTableEntry.setFontFamily(Swiss);
    } else if (controlWord == "fnil") {
        m_fontTableEntry.setFontFamily(Nil);
    } else if (controlWord == "fmodern") {
        m_fontTableEntry.setFontFamily(Modern);
    } else if (controlWord == "fscript") {
        m_fontTableEntry.setFontFamily(Script);
    } else if (controlWord == "fdecor") {
        m_fontTableEntry.setFontFamily(Decor);
    } else if (controlWord == "ftech") {
        m_fontTableEntry.setFontFamily(Tech);
    } else if (controlWord == "fbidi") {
        m_fontTableEntry.setFontFamily(Bidi);
    } else if (controlWord == "fprq") {
        m_fontTableEntry.setFontPitch(static_cast<enum FontPitch>(value));
    } else if (controlWord == "fcharset") {
        for (const auto &entry : charsetToCodec) {
            if (entry.id == value) {
                const auto encoding = QStringConverter::encodingForName(entry.name);
                if (encoding) {
                    m_fontTableEntry.setEncoding(*encoding);
                    break;
                }
            }
        }
    } else {
        qCDebug(lcRtf) << "unhandled fonttbl control word:" << controlWord << "(" << value << ")";
    }
}

void FontTableDestination::handlePlainText(const QByteArray &plainText)
{
    if (plainText == ";") {
        m_output->insertFontTableEntry(m_fontTableEntry, m_currentFontTableIndex);
    } else if (plainText.endsWith(";")) {
        // probably a font name with a terminating delimiter
        int delimiterPosition = plainText.indexOf(";");
        if (delimiterPosition == (plainText.length() - 1)) {
            // It is at the end, chop it off
            QString fontName = plainText.left(delimiterPosition);
            m_fontTableEntry.setFontName(fontName);
            m_output->insertFontTableEntry(m_fontTableEntry, m_currentFontTableIndex);
        } else {
            // we were not expecting a name with a delimiter other than at the end
            qCDebug(lcRtf) << "Font name with embedded delimiter: " << plainText;
        }
    } else {
        // plain font name
        m_fontTableEntry.setFontName(plainText);
    }
}

void FontTableDestination::aboutToEndDestination()
{
    // TODO
}
}
