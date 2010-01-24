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

#include "KWPageStyle.h"

#include <KoXmlReader.h>
#include <QTextDocument>
#include <QObject>

class KWDocument;
class KWFrameSet;
class KWFrame;
class KWPageManager;
class KWTextFrameSet;
class KoParagraphStyle;
class KoCharacterStyle;
class QColor;
class KoShape;
class KoStore;
class KoInlineNote;

/// KWDocument delegates to this class the loading of (old style) KWD documents
class KWDLoader : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent the document this loader will work for.
     */
    KWDLoader(KWDocument *parent, KoStore *store);
    virtual ~KWDLoader();

    /**
     * Load a document from a xml structure.
     * @param root the root node from an xml document of the kword file format upto 1.4
     * @return return true on success, false on failure
     */
    bool load(KoXmlElement &root);

signals:
    /**
     * This signal is emitted during loading with a percentage within 1-100 range
     * @param percent the progress as a percentage
     */
    void progressUpdate(int percent);

private:
    // old kword files have a lot of fields for the image key, so we duplicate that here.
    struct ImageKey {
        QString year, month, day, hour, minute, second, milisecond;
        QString oldFilename, filename;
    };


    /// find and load all framesets
    void loadFrameSets(const KoXmlElement &framesets);
    /// load one frameset
    void loadFrameSet(const KoXmlElement &framesetElem, bool loadFrames = true , bool loadFootnote = true);
    /// fill the data of fs with the info from the element
    void fill(KWFrameSet *fs, const KoXmlElement &element);
    /// fill the data of fs with the info from the element
    void fill(KWTextFrameSet *fs, const KoXmlElement &framesetElem);
    /// fill the data of style with the info from the element
    void fill(KoParagraphStyle *style, const KoXmlElement &layoutElem);
    void fill(KoCharacterStyle *style, const KoXmlElement &formatElem);
    void fill(KWFrame *frame, const KoXmlElement &frameElem);
    void fill(ImageKey *key, const KoXmlElement &keyElement);

    void insertAnchors();
    void insertNotes();

    // load the document wide styles
    void loadStyleTemplates(const KoXmlElement &styles);

    // helper method. Gets the color from an element assuming there are 'red','green', 'blue' attributes on it.
    QColor colorFrom(const KoXmlElement &element);

private:
    KWDocument *m_document;
    KoStore *m_store;
    KWPageManager *m_pageManager;
    KWPageStyle m_pageStyle;
    KWPageStyle m_firstPageStyle;
    bool m_foundMainFS;
    int m_nrItemsToLoad, m_itemsLoaded;

    struct AnchorData {
        int cursorPosition;
        KoShape *textShape;
        QTextDocument *document;
        QString frameSetName;
    };
    QList<AnchorData> m_anchors;

    struct NotesData {
        KoInlineNote *note;
        QString frameSetName;
    };
    QList<NotesData> m_notes;

    QList<ImageKey> m_images;
};

#endif
