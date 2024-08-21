/*
 * Assists in creating thumbnails for Gemini's file views
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef THUMBNAILHELPERIMPL_H
#define THUMBNAILHELPERIMPL_H

#include <QObject>

class KoPart;
class KoDocument;
class ThumbnailHelperImpl : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailHelperImpl(QObject *parent = nullptr);
    ~ThumbnailHelperImpl() override;

    bool convert(const QString &in, const QString &out, int width, int height);

private:
    KoPart *m_part;
    KoDocument *m_doc;

    bool m_loadingCompleted : 1;
};

#endif // THUMBNAILHELPERIMPL_H

class KoPart;
