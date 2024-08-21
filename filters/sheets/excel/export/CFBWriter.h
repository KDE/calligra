/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    bool open(QIODevice *device);
    bool open(const QString &fileName);

    void close();

    QIODevice *openSubStream(const QString &streamName);
    void setRootClassId(const QUuid &classId);

private:
    void init();
    void writeHeader();

    QIODevice *m_device;
    bool m_ownsDevice;

    unsigned m_sectorSize;

    /// returns the sector id of the newly written sector
    unsigned writeSector(const QByteArray &data, unsigned previousSector = -1);

    unsigned writeMiniSector(const QByteArray &data, unsigned previousSector = -1);

    void writeData(unsigned sector, unsigned sectorOffset, const QByteArray &data);

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
        enum Type { Unknown = 0x00, Storage = 0x01, Stream = 0x02, RootStorage = 0x05 };

        int id;
        QString name;
        QUuid uuid;

        Type type;
        QHash<QString, DirectoryEntry *> children;
        DirectoryEntry *leftSibling, *rightSibling, *firstChild;
        quint32 firstSector;
        quint64 streamSize;
        DirectoryEntry(int id, const QString &name, Type type)
            : id(id)
            , name(name)
            , type(type)
            , leftSibling(nullptr)
            , rightSibling(nullptr)
            , firstSector(-1)
            , streamSize(0)
        {
        }
        void buildChildrenTree();

    private:
        void buildSiblingTree(const QList<DirectoryEntry *> &nodes, int first, int middle, int last);
    };
    QList<DirectoryEntry> m_entries;

    class StreamIODevice;
    QList<StreamIODevice *> m_openStreams;

    QIODevice *m_miniFatDataStream;
};

#endif // CFBWRITER_H
