/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef CFBWRITER_H
#define CFBWRITER_H

#include <QHash>
#include <QList>
#include <QString>
#include <QUuid>

class QIODevice;

class CFBWriter
{
public:
    explicit CFBWriter(bool largeSectors);
    ~CFBWriter();
    bool open(QIODevice* device);
    bool open(const QString& fileName);

    void close();

    QIODevice* openSubStream(const QString& streamName);
    void setRootClassId(const QUuid& classId);
private:
    void init();
    void writeHeader();

    QIODevice* m_device;
    bool m_ownsDevice;

    unsigned m_sectorSize;

    /// returns the sector id of the newly written sector
    unsigned writeSector(const QByteArray& data, unsigned previousSector = -1);

    unsigned writeMiniSector(const QByteArray& data, unsigned previousSector = -1);

    void writeData(unsigned sector, unsigned sectorOffset, const QByteArray& data);

    unsigned fatSectorCount() const;

    QList<unsigned> m_difat;
    unsigned m_firstDifatSector;
    unsigned m_difatSectorCount;
    unsigned m_directorySectorCount;
    unsigned m_firstDirectorySector;
    unsigned m_firstMiniFatSector;
    unsigned m_miniFatSectorCount;

    QList<unsigned> m_fat;
    QList<unsigned> m_miniFat;

    struct DirectoryEntry {
        enum Type {
            Unknown = 0x00,
            Storage = 0x01,
            Stream  = 0x02,
            RootStorage = 0x05
        };

        int id;
        QString name;
        QUuid uuid;

        Type type;
        QHash<QString, DirectoryEntry*> children;
        DirectoryEntry *leftSibling, *rightSibling, *firstChild;
        quint32 firstSector;
        quint64 streamSize;
        DirectoryEntry(int id, const QString& name, Type type) : id(id), name(name), type(type), leftSibling(0), rightSibling(0), firstSector(-1), streamSize(0) {}
        void buildChildrenTree();
    private:
        void buildSiblingTree(const QList<DirectoryEntry*>& nodes, int first, int middle, int last);
    };
    QList<DirectoryEntry> m_entries;

    class StreamIODevice;
    QList<StreamIODevice*> m_openStreams;

    QIODevice* m_miniFatDataStream;
};

#endif // CFBWRITER_H
