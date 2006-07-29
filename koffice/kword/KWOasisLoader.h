/* This file is part of the KDE project
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWOASISLOADER_H
#define KWOASISLOADER_H

class KoPoint;
class KWTableFrameSet;
class KWLoadingInfo;
class KoOasisContext;
class QDomDocument;
class KoOasisSettings;
class QDomElement;
class KWFrame;
class KoStore;
class KoTextCursor;
class KWDocument;
template <class T> class QValueList;

// KWDocument delegates to this class the loading of OASIS documents or fragments
class KWOasisLoader
{
public:
    KWOasisLoader( KWDocument* doc );

    /// Insert everything from an oasis store - used when pasting
    /// (either text or entire frames)
    /// TODO: dontCreateFootNote
    QValueList<KWFrame *> insertOasisData( KoStore* store, KoTextCursor* cursor );

    void loadOasisSettings( const QDomDocument&settingsDoc );
    void loadOasisHeaderFooter( const QDomElement& headerFooter, bool hasEvenOdd,
                                QDomElement& style, KoOasisContext& context );

    KWFrame* loadFrame( const QDomElement& frameTag, KoOasisContext& context, const KoPoint& offset );
    KWFrame* loadOasisTextBox( const QDomElement& frame, const QDomElement& tag,
                               KoOasisContext& context );
    KWTableFrameSet* loadOasisTable( const QDomElement& tag,
                                     KoOasisContext& context );

private:
    void loadOasisIgnoreList( const KoOasisSettings& settings );

private:
    KWDocument* m_doc;
};


#endif
