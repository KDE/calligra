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

#ifndef KWORD_COMMAND_H
#define KWORD_COMMAND_H

#include <kcommand.h>
#include <koGlobal.h>
#include <koRect.h>
#include <qdom.h>
class KWGUI;

namespace Qt3 {
class QTextCommand;
}
class KWTextFrameSet;
class KWDocument;
class KWTableFrameSet;

/**
 * Wraps a QTextCommand into a KCommand, for the UI
 * In fact the QTextCommand isn't even known from here.
 * When the UI orders execute or unexecute, we simply call undo/redo
 * on the KWTextFrameset. Since one KCommand is created for each
 * command there, the two simply map.
 */
class KWTextCommand : public KCommand
{
public:
    KWTextCommand( KWTextFrameSet * textfs, const QString & name ) :
        KCommand( name ), m_textfs(textfs) {}
    ~KWTextCommand() {}

    virtual void execute();
    virtual void unexecute();

protected:
    KWTextFrameSet * m_textfs;
};

////// Extensions to qrichtext_p.h
#include <qrichtext_p.h>
#include <kwtextparag.h>
#include <kwtextdocument.h>
using namespace Qt3;

/**
 * Command created when deleting some text
 */
class KWTextDeleteCommand : public QTextDeleteCommand
{
public:
    KWTextDeleteCommand( QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
                         const CustomItemsMap & customItemsMap,
                         const QValueList<KWParagLayout> & oldParagLayouts );
    //KWTextDeleteCommand( QTextParag *p, int idx, const QArray<QTextStringChar> &str );
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    QValueList<KWParagLayout> m_oldParagLayouts;
    CustomItemsMap m_customItemsMap;
};

/**
 * Command created when inserting some text
 */
class KWTextInsertCommand : public KWTextDeleteCommand
{
public:
    KWTextInsertCommand( QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
                         const CustomItemsMap & customItemsMap,
                         const QValueList<KWParagLayout> &oldParagLayouts )
        : KWTextDeleteCommand( d, i, idx, str, customItemsMap, oldParagLayouts ) {}
    //KWTextInsertCommand( QTextParag *p, int idx, const QArray<QTextStringChar> &str )
    //    : KWTextDeleteCommand( p, idx, str ) {}
    Commands type() const { return Insert; };
    QTextCursor *execute( QTextCursor *c ) { return KWTextDeleteCommand::unexecute( c ); }
    QTextCursor *unexecute( QTextCursor *c ) { return KWTextDeleteCommand::execute( c ); }
};

/**
 * Command created when changing paragraph attributes
 */
class KWTextParagCommand : public QTextCommand
{
public:
    KWTextParagCommand( QTextDocument *d, int fParag, int lParag,
                        const QValueList<KWParagLayout> &oldParagLayouts,
                        KWParagLayout newParagLayout,
                        int /*KWParagLayout::Flags*/ flags,
                        QStyleSheetItem::Margin margin = (QStyleSheetItem::Margin)-1 );
                        // margin is only meaningful if flags==Margins only. -1 means all.
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    int firstParag, lastParag;
    QValueList<KWParagLayout> m_oldParagLayouts;
    KWParagLayout m_newParagLayout;
    int m_flags;
    int m_margin;
};

/**
 * Command created when changing the default format of paragraphs.
 * This is ONLY used for counters and bullet's formatting.
 * See QTextFormatCommand for the command used when changing the formatting of any set of characters.
 */
class KWParagFormatCommand : public QTextCommand
{
public:
    KWParagFormatCommand( QTextDocument *d, int fParag, int lParag,
                          const QValueList<QTextFormat *> &oldFormats,
                          QTextFormat * newFormat );
    ~KWParagFormatCommand();
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    int firstParag, lastParag;
    QValueList<QTextFormat *> m_oldFormats;
    QTextFormat * m_newFormat;
};

/**
 * Command created when pasting formatted text
 */
class KWPasteCommand : public QTextCommand
{
public:
    KWPasteCommand( QTextDocument *d, int parag, int idx,
                    const QCString & data );
    ~KWPasteCommand() {}
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QCString m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
};

////////////////////////// Frame commands ////////////////////////////////

// Identifies a frame
struct FrameIndex {
    KWFrameSet * m_pFrameSet;
    unsigned int m_iFrameIndex;
};

enum FrameBorderType { FBLeft=0, FBRight=1, FBTop=2, FBBottom=3};

struct FrameBorderTypeStruct {
    FrameBorderType m_EFrameType;
    Border m_OldBorder;
};

/**
 * Command created when changing frame border
 */
class KWFrameBorderCommand : public KCommand
{
public:
    KWFrameBorderCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_listFrameIndex, QList<FrameBorderTypeStruct> &_frameTypeBorder,const Border & _newBorder ) ;
    ~ KWFrameBorderCommand() {}

    void execute();
    void unexecute();

protected:
    QList<FrameIndex> m_IndexFrame;
    QList<FrameBorderTypeStruct> m_oldBorderFrameType;
    Border m_newBorder;
    KWDocument *m_pDoc;
};

/**
 * Command created when changing background color of one or more frames
 */
class KWFrameBackGroundColorCommand : public KCommand
{
public:
    KWFrameBackGroundColorCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_listFrameIndex, QList<QBrush> &_oldBrush,const QBrush & _newColor ) ;
    ~KWFrameBackGroundColorCommand() {}

    void execute();
    void unexecute();

protected:
    QList<FrameIndex> m_IndexFrame;
    QList<QBrush> m_oldBackGroundColor;
    QBrush m_newColor;
    KWDocument *m_pDoc;
};

struct FrameResizeStruct {
    KoRect sizeOfBegin;
    KoRect sizeOfEnd;
};

/**
 * Command created when a frame is resized
 */
class KWFrameResizeCommand : public KCommand
{
public:
    KWFrameResizeCommand( const QString &name,KWDocument *_doc,FrameIndex _frameIndex,FrameResizeStruct _frameResize ) ;
    ~KWFrameResizeCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_IndexFrame;
    FrameResizeStruct m_FrameResize;
    KWDocument *m_pDoc;
};

/**
 * Command created when one or more frames are moved
 */
class KWFrameMoveCommand : public KCommand
{
public:
    KWFrameMoveCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_frameIndex,QList<FrameResizeStruct>&_frameMove ) ;
    ~KWFrameMoveCommand() {}

    void execute();
    void unexecute();
    QList<FrameResizeStruct> & listFrameMoved() { return m_frameMove; }
protected:
    QList<FrameIndex> m_IndexFrame;
    QList<FrameResizeStruct> m_frameMove;
    KWDocument *m_pDoc;
};


///////////////////////////////layout command///////////////////////////
struct pageLayout {
    KoPageLayout _pgLayout;
    KoColumns _cl;
    KoKWHeaderFooter _hf;
};

/**
 * Command created when you change layout
 */
class KWPageLayoutCommand : public KCommand
{
public:
    KWPageLayoutCommand( const QString &name,KWDocument *_doc,pageLayout &_oldLayout, pageLayout &_newLayout) ;
    ~KWPageLayoutCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    pageLayout m_OldLayout;
    pageLayout m_NewLayout;
};


/**
 * Command created when you delete a frame
 */
class KWDeleteFrameCommand : public KCommand
{
public:
    KWDeleteFrameCommand( const QString &name, KWDocument *_doc, KWFrame * frame) ;
    ~KWDeleteFrameCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    FrameIndex frameIndex;
    KWFrame *copyFrame;
};

/**
 * Command created when you create a frame
 */
class KWCreateFrameCommand : public KCommand
{
public:
    KWCreateFrameCommand( const QString &name, KWDocument *_doc, KWFrame * frame) ;
    ~KWCreateFrameCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    FrameIndex frameIndex;
    KWFrame *copyFrame;
};

/**
 * Command created when you ungroup a table
 */
class KWUngroupTableCommand : public KCommand
{
public:
    KWUngroupTableCommand( const QString &name, KWDocument *_doc, KWTableFrameSet * _table) ;
    ~KWUngroupTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrame;
};

/**
 * Command created when you delete a table
 */
class KWDeleteTableCommand : public KCommand
{
public:
    KWDeleteTableCommand( const QString &name, KWDocument *_doc, KWTableFrameSet * _table) ;
    ~KWDeleteTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    KWTableFrameSet *m_pTable;
};


#endif
