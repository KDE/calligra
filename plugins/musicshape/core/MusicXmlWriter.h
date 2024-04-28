/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSICXMLWRITER_H
#define MUSICXMLWRITER_H

class KoXmlWriter;

namespace MusicCore
{
class Sheet;

class MusicXmlWriter
{
public:
    MusicXmlWriter();
    ~MusicXmlWriter();

    void writeSheet(KoXmlWriter &writer, MusicCore::Sheet *sheet, bool writeNamespaceDef = true);
};

} // namespace MusicCore

#endif // MUSICXMLWRITER_H
