/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>

    SPDX-License-Identifier: LGPL-2.1-or-later

 */
#ifndef KOCOLORSET
#define KOCOLORSET

#include <QColor>
#include <QObject>
#include <QVector>

#include "KoColor.h"
#include "KoResource.h"

struct KoColorSetEntry {
    KoColor color;
    QString name;
    bool operator==(const KoColorSetEntry &rhs) const
    {
        return color == rhs.color && name == rhs.name;
    }
};

/**
 * Open Gimp, Photoshop or RIFF palette files. This is a straight port
 * from the Gimp.
 */
class PIGMENTCMS_EXPORT KoColorSet : public QObject, public KoResource
{
    Q_OBJECT
public:
    enum PaletteType {
        UNKNOWN = 0,
        GPL, // GIMP
        RIFF_PAL, // RIFF
        ACT, // Photoshop binary
        PSP_PAL, // PaintShop Pro
        ACO // Photoshop Swatches
    };

    /**
     * Load a color set from a file. This can be a Gimp
     * palette, a RIFF palette or a Photoshop palette.
     */
    explicit KoColorSet(const QString &filename);

    /// Create an empty color set
    KoColorSet();

    /// Explicit copy constructor (KoResource copy constructor is private)
    KoColorSet(const KoColorSet &rhs);

    ~KoColorSet() override;

    bool load() override;
    bool loadFromDevice(QIODevice *dev) override;
    bool save() override;
    bool saveToDevice(QIODevice *dev) const override;

    QString defaultFileExtension() const override;

    void setColumnCount(int columns);
    int columnCount();

public:
    void add(const KoColorSetEntry &);
    void remove(const KoColorSetEntry &);
    void removeAt(quint32 index);
    KoColorSetEntry getColor(quint32 index);
    qint32 nColors();

private:
    bool init();

    bool loadGpl();
    bool loadAct();
    bool loadRiff();
    bool loadPsp();
    bool loadAco();

    QByteArray m_data;
    bool m_ownData;
    QString m_name;
    QString m_comment;
    qint32 m_columns;
    QVector<KoColorSetEntry> m_colors;
};
#endif // KOCOLORSET
