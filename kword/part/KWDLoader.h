/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWDLOADER_H
#define KWDLOADER_H

#include <QObject>

class KWDocument;
class KWFrameSet;
class KWPageSettings;
class KWPageManager;
class KWTextFrameSet;
class QDomElement;

class KWDLoader : public QObject
{
    Q_OBJECT
public:
    KWDLoader(KWDocument *parent);
    virtual ~KWDLoader();

    bool load(QDomElement &root);

signals:
    void sigProgress(int percent);

private:
    void loadFrameSets( const QDomElement &framesets );
    KWFrameSet *loadFrameSet( QDomElement framesetElem, bool loadFrames = true , bool loadFootnote = true);
    void fill(KWFrameSet *fs, QDomElement framesetElem);
    void fill(KWTextFrameSet *fs, QDomElement framesetElem);

private:
    KWDocument *m_document;
    KWPageSettings *m_pageSettings;
    KWPageManager *m_pageManager;
    bool m_foundMainFS;
    int m_nrItemsToLoad, m_itemsLoaded;
};

#endif
