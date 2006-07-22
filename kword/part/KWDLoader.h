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

/// KWDocument delegates to this class the loading of (old style) KWD documents
class KWDLoader : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent the document this loader will work for.
     */
    KWDLoader(KWDocument *parent);
    virtual ~KWDLoader();

    /**
     * Load a document from a xml structure.
     * @param root the root node from an xml document of the kword file format upto 1.4
     * @return return true on success, false on failure
     */
    bool load(QDomElement &root);

signals:
    /**
     * This signal is emitted during loading with a percentage within 1-100 range
     * @param percent the progress as a percentage
     */
    void sigProgress(int percent);

private:
    /// find and load all framesets
    void loadFrameSets( const QDomElement &framesets );
    /// load one frameset
    KWFrameSet *loadFrameSet( QDomElement framesetElem, bool loadFrames = true , bool loadFootnote = true);
    /// fill the data of fs with the info from the element
    void fill(KWFrameSet *fs, QDomElement element);
    /// fill the data of fs with the info from the element
    void fill(KWTextFrameSet *fs, QDomElement framesetElem);

private:
    KWDocument *m_document;
    KWPageSettings *m_pageSettings;
    KWPageManager *m_pageManager;
    bool m_foundMainFS;
    int m_nrItemsToLoad, m_itemsLoaded;
};

#endif
