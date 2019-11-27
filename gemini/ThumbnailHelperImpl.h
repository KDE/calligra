/*
 * Assists in creating thumbnails for Gemini's file views
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    explicit ThumbnailHelperImpl(QObject* parent = 0);
    ~ThumbnailHelperImpl() override;

    bool convert(const QString& in, const QString& out, int width, int height);
private:
    KoPart *m_part;
    KoDocument *m_doc;

    bool m_loadingCompleted :1;
};

#endif // THUMBNAILHELPERIMPL_H

class KoPart;
