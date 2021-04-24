/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSICXMLREADER_H
#define MUSICXMLREADER_H

#include <KoXmlReaderForward.h>
#include <QString>

namespace MusicCore {
    class Sheet;
    class Clef;
    class Staff;
    class TimeSignature;
    class Part;

class MusicXmlReader {
public:
    explicit MusicXmlReader(const char* musicNamespace = "http://www.calligra.org/music");
    
    Sheet* loadSheet(const KoXmlElement& scoreElement);
private:
    const char* m_namespace;
    
    QString getProperty(const KoXmlElement& elem, const char *propName);
    Clef* loadClef(const KoXmlElement& element, Staff* staff);
    TimeSignature* loadTimeSignature(const KoXmlElement& element, Staff* staff);
    void loadPart(const KoXmlElement& partElement, Part* part);
    
    KoXmlElement namedItem( const KoXmlNode& node, const char* localName );
    bool checkNamespace(const KoXmlNode& node);
};

} // namespace MusicCore

#endif // MUSICXMLREADER_H
