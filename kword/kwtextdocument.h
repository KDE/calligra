/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kwtextdocument_h
#define kwtextdocument_h

#include "qrichtext_p.h"
using namespace Qt3;
class KWTextFrameSet;
class KoZoomHandler;
class KWTextFormatCollection;
class KCommand;
class QDomElement;
class KMacroCommand;
class KWParagVisitor;

/**
 * This is our QTextDocument reimplementation, to create KoTextParags instead of QTextParags,
 */
class KoTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    // don't mind p (we don't use parent documents)
    KoTextDocument( KoZoomHandler * zoomHandler, QTextDocument *p, KWTextFormatCollection *fc );

    ~KoTextDocument();

    // Factory method for paragraphs
    virtual QTextParag * createParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE );

    // Return the zoom handler associated with this document.
    // (Usually the KWDocument, but a simple zoom handler in the paragdia preview)
    KoZoomHandler * zoomHandler() const { return m_zoomHandler; }

    // Visit all the parts of a selection.
    // Returns true, unless canceled. See KWParagVisitor.
    bool visitSelection( int selectionId, KWParagVisitor *visitor, bool forward = true );

    // Visit all paragraphs of the document.
    // Returns true, unless canceled. See KWParagVisitor.
    bool visitDocument( KWParagVisitor *visitor, bool forward = true );

    // Visit the document between those two point.
    // Returns true, unless canceled. See KWParagVisitor.
    bool visitFromTo( QTextParag *firstParag, int firstIndex, QTextParag* lastParag, int lastIndex, KWParagVisitor* visitor, bool forw = true );

    // Used by ~KoTextParag to know if it should die quickly
    bool isDestroying() const { return m_bDestroying; }

private:
    KoZoomHandler * m_zoomHandler;
    bool m_bDestroying;
};

/**
 * This is our QTextDocument reimplementation, to create KWTextParag instead of QTextParags,
 * and to relate it to the text frameset it's in.
 */
class KWTextDocument : public KoTextDocument
{
    Q_OBJECT
public:
    // A real text document inside a frameset
    KWTextDocument( KWTextFrameSet * textfs, QTextDocument *p, KWTextFormatCollection *fc );
    // A standalone text document, for a preview
    KWTextDocument( KoZoomHandler * zoomHandler );

    ~KWTextDocument() { }

    virtual QTextParag * createParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE );

    // Return the text frameset in which this document is.
    // Note that this can be 0L (e.g. for paragraphs in the paragdia preview)
    KWTextFrameSet * textFrameSet() const { return m_textfs; }

protected:
    void init();
private:
    KWTextFrameSet * m_textfs;
};

class KWParagVisitor
{
protected: // abstract base class
    KWParagVisitor() {}
    virtual ~KWParagVisitor() {}
public:
    // Visit the paragraph @p parag, from index @p start to index @p end
    virtual bool visit( QTextParag *parag, int start, int end ) = 0;
};

/**
 * KWord's base class for QRT custom items (i.e. special chars)
 * Custom items include:
 * - variables ( KWVariable, variable.h )
 * - inline images ( KWTextImage, kwtextimage.h )
 * - anchors, i.e. floating frames ( KWAnchor, kwanchor.h )
 */
class KWTextCustomItem : public QTextCustomItem
{
public:
    KWTextCustomItem( KWTextDocument *textdoc ) : QTextCustomItem( textdoc )
    { m_deleted = false; }

    // The text document in which this customitem is
    KWTextDocument * textDocument() const { return static_cast<KWTextDocument *>( parent ); }

    // When the user deletes a custom item, it isn't destroyed but
    // moved into the undo/redo history - setDeleted( true )
    // and it can be then copied back from there into the real world - setDeleted( false ).
    virtual void setDeleted( bool b ) { m_deleted = b; }

    bool isDeleted() const { return m_deleted; }

    // Called when the item is created or 'deleted' by the user
    // Most custom items don't need to reimplement those, since
    // the custom item is simply moved into the undo/redo history
    // when deleting (or undoing a creation).
    // It is not deleted and re-created later.
    virtual KCommand * createCommand() { return 0L; }
    virtual KCommand * deleteCommand() { return 0L; }

    // Save to XML
    virtual void save( QDomElement & /*formatElem*/ ) = 0;

    // Reimplement this to calculate the item width
    // It is important to start with "if ( m_deleted ) return;"
    virtual void resize() {}

    // The index in paragraph(), where this anchor is
    // Slightly slow (does a linear search in the paragraph)
    int index() const;

    // The formatting given to this 'special' character
    // Slightly slow (does a linear search in the paragraph)
    QTextFormat * format() const;

protected:
    virtual void adjustToPainter( QPainter* ) { resize(); }
    bool m_deleted;
};

// A CustomItemsMap associates a custom item to an index
// Used in the undo/redo info for insert/delete text.
class CustomItemsMap : public QMap<int, KWTextCustomItem *>
{
public:

    // Insert all the items from the map, into the existing text
    void insertItems( const QTextCursor & startCursor, int size );

    // Delete all the items from the map, adding their commands into macroCmd
    void deleteAll( KMacroCommand *macroCmd );
};

#endif
