/*
    SPDX-FileCopyrightText: 2000 Matthias Elter <elter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOPATTERN_H
#define KOPATTERN_H

#include "KoResource.h"
#include <pigment_export.h>

#include <QMetaType>

/// Write API docs here
class PIGMENTCMS_EXPORT KoPattern : public KoResource
{
public:
    /**
     * Creates a new KoPattern object using @p filename.  No file is opened
     * in the constructor, you have to call load.
     *
     * @param filename the file name to save and load from.
     */
    explicit KoPattern(const QString &filename);
    KoPattern(const QImage &image, const QString &name, const QString &folderName);
    ~KoPattern() override;

public:
    bool load() override;
    bool loadFromDevice(QIODevice *dev) override;
    bool save() override;
    bool saveToDevice(QIODevice *dev) const override;

    bool loadPatFromDevice(QIODevice *dev);
    bool savePatToDevice(QIODevice *dev) const;

    qint32 width() const;
    qint32 height() const;

    QString defaultFileExtension() const override;

    KoPattern &operator=(const KoPattern &pattern);

    KoPattern *clone() const;

    /**
     * @brief pattern the actual pattern image
     * @return a valid QImage. There are no guarantees to the image format.
     */
    QImage pattern() const;

private:
    bool init(QByteArray &data);
    void setPatternImage(const QImage &image);

private:
    QImage m_pattern;
    mutable QByteArray m_md5;
};

Q_DECLARE_METATYPE(KoPattern *)

#endif // KOPATTERN_H
