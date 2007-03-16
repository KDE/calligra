/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
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

#ifndef KWOPENDOCUMENTLOADER_H
#define KWOPENDOCUMENTLOADER_H

#include <QObject>
#include <KoStore.h>
#include <KoXmlReader.h>

class KWDocument;
class KWTextFrameSet;
//class KWFrameSet;
//class KWFrame;
//class KWPageSettings;
//class KWPageManager;
//class KWTextFrameSet;

//class KoParagraphStyle;
//class KoCharacterStyle;
//class KoStore;
class KoOasisStyles;
class KoOasisLoadingContext;

class QDomDocument;
class QTextCursor;
//class QColor;

class KWOpenDocumentLoader : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent the document this loader will work for.
     */
    explicit KWOpenDocumentLoader(KWDocument *parent);
    virtual ~KWOpenDocumentLoader();

    /**
     *  @brief Loads an OASIS OpenDocument from a store.
     *  This implements the KoDocument::loadOasis method.
     */
    bool load(const QDomDocument& doc, KoOasisStyles& styles, const QDomDocument& settings, KoStore* store);

signals:
    /**
     * This signal is emitted during loading with a percentage within 1-100 range
     * @param percent the progress as a percentage
     */
    void sigProgress(int percent);

private:
    void loadOasisSettings(const QDomDocument& settings);
    void loadOasisStyles(KoOasisLoadingContext& context);
    bool loadOasisPageLayout(const QString& masterPageName, KoOasisLoadingContext& context);
    bool loadMasterPageStyle(const QString& masterPageName, KoOasisLoadingContext& context);

    void loadOasisHeaderFooter(const QDomElement& headerFooter, bool hasEvenOdd, QDomElement& style, KoOasisLoadingContext& context);

    void loadOasisSpan(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor);

private:
    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

#endif
