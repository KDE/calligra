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
class KWPasteTextCommand : public QTextCommand
{
public:
    KWPasteTextCommand( QTextDocument *d, int parag, int idx,
                    const QCString & data );
    ~KWPasteTextCommand() {}
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QCString m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KWParagLayout m_oldParagLayout;
};

/**
 * Command created when changing formatted text
 */
class KWTextFormatCommand : public QTextFormatCommand
{
public:
    KWTextFormatCommand( QTextDocument *d, int sid, int sidx, int eid, int eidx, const QMemArray<QTextStringChar> &old, QTextFormat *f, int fl );
    virtual ~KWTextFormatCommand();

    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
    void resizeCustomItem();
};


////////////////////////// Frame commands ////////////////////////////////

// Identifies a frame
struct FrameIndex {
    FrameIndex() {}
    FrameIndex( KWFrame *frame );

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
    KWFrameBorderCommand( const QString &name, QList<FrameIndex> &_listFrameIndex, QList<FrameBorderTypeStruct> &_frameTypeBorder,const Border & _newBorder ) ;
    ~ KWFrameBorderCommand() {}

    void execute();
    void unexecute();

protected:
    QList<FrameIndex> m_indexFrame;
    QList<FrameBorderTypeStruct> m_oldBorderFrameType;
    Border m_newBorder;
};

/**
 * Command created when changing background color of one or more frames
 */
class KWFrameBackGroundColorCommand : public KCommand
{
public:
    KWFrameBackGroundColorCommand( const QString &name, QList<FrameIndex> &_listFrameIndex, QList<QBrush> &_oldBrush, const QBrush & _newColor ) ;
    ~KWFrameBackGroundColorCommand() {}

    void execute();
    void unexecute();

protected:
    QList<FrameIndex> m_indexFrame;
    QList<QBrush> m_oldBackGroundColor;
    QBrush m_newColor;
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
    KWFrameResizeCommand( const QString &name, FrameIndex _frameIndex, FrameResizeStruct _frameResize ) ;
    ~KWFrameResizeCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_indexFrame;
    FrameResizeStruct m_FrameResize;
};

/**
 * Command created when one or more frames are moved
 */
class KWFrameMoveCommand : public KCommand
{
public:
    KWFrameMoveCommand( const QString &name,QList<FrameIndex> &_frameIndex,QList<FrameResizeStruct>&_frameMove ) ;
    ~KWFrameMoveCommand() {}

    void execute();
    void unexecute();
    QList<FrameResizeStruct> & listFrameMoved() { return m_frameMove; }
protected:
    QList<FrameIndex> m_indexFrame;
    QList<FrameResizeStruct> m_frameMove;
};

/**
 * Command created when we change frame parameter
 */
class KWFramePropertiesCommand : public KCommand
{
public:
    KWFramePropertiesCommand( const QString &name, KWFrame *_frameBefore,  KWFrame *_frameAfter );	
    ~KWFramePropertiesCommand();
    void execute();
    void unexecute();
protected:
    FrameIndex m_frameIndex;
    KWFrame *m_frameBefore;
    KWFrame *m_frameAfter;
};

/**
 * Command created when a frameset is made floating or fixed using the frame dialog
 */
class KWFrameSetFloatingCommand : public KCommand
{
public:
    KWFrameSetFloatingCommand( const QString &name, KWFrameSet *frameset, bool floating );
    ~KWFrameSetFloatingCommand() {}

    void execute();
    void unexecute();

protected:
    KWFrameSet* m_pFrameSet;
    bool m_bFloating;
};

///////////////////////////////layout command///////////////////////////
struct pageLayout {
    KoPageLayout _pgLayout;
    KoColumns _cl;
    KoKWHeaderFooter _hf;
};

/**
 * Command created when changing the page layout
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
 * Command created when deleting a frame
 */
class KWDeleteFrameCommand : public KCommand
{
public:
    KWDeleteFrameCommand( const QString &name, KWFrame * frame) ;
    ~KWDeleteFrameCommand() {}

    void execute();
    void unexecute();
protected:
    FrameIndex m_frameIndex;
    KWFrame *m_copyFrame;
};

/**
 * Command created when creating a frame
 */
class KWCreateFrameCommand : public KWDeleteFrameCommand
{
public:
    KWCreateFrameCommand( const QString &name, KWFrame * frame);
    ~KWCreateFrameCommand() {}

    void execute() { KWDeleteFrameCommand::unexecute(); }
    void unexecute() { KWDeleteFrameCommand::execute(); }
};

/**
 * Command created when ungrouping a table
 */
class KWUngroupTableCommand : public KCommand
{
public:
    KWUngroupTableCommand( const QString &name, KWTableFrameSet * _table) ;
    ~KWUngroupTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrame;
};

/**
 * Command created when deleting a table
 */
class KWDeleteTableCommand : public KCommand
{
public:
    KWDeleteTableCommand( const QString &name, KWTableFrameSet * _table) ;
    ~KWDeleteTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
};


/**
 * Command created when creating a table
 */
class KWCreateTableCommand : public KWDeleteTableCommand
{
public:
    KWCreateTableCommand( const QString &name, KWTableFrameSet * _table)
        : KWDeleteTableCommand( name, _table ) {}
    ~KWCreateTableCommand() {}

    void execute() { KWDeleteTableCommand::unexecute(); }
    void unexecute() { KWDeleteTableCommand::execute(); }
};

/**
 * Command created when inserting a column
 */
class KWInsertColumnCommand : public KCommand
{
public:
    KWInsertColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWInsertColumnCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrameSet;
    unsigned int m_colPos;
};


/**
 * Command created when inserting a row
 */
class KWInsertRowCommand : public KCommand
{
public:
    KWInsertRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWInsertRowCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrameSet;
    unsigned int m_rowPos;
};

/**
 * Command created when removing a row
 */
class KWRemoveRowCommand : public KCommand
{
public:
    KWRemoveRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveRowCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrameSet;
    QList<KWFrame> m_copyFrame;
    unsigned int m_rowPos;
};


/**
 * Command created when removing a column
 */
class KWRemoveColumnCommand : public KCommand
{
public:
    KWRemoveColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveColumnCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QList<KWFrameSet> m_ListFrameSet;
    QList<KWFrame> m_copyFrame;
    unsigned int m_colPos;
};

/**
 * Command created when splitting a cell
 */
class KWSplitCellCommand : public KCommand
{
public:
    KWSplitCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd );
    ~KWSplitCellCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    unsigned int m_colBegin;
    unsigned int m_rowBegin;
    unsigned int m_colEnd;
    unsigned int m_rowEnd;
    QList<KWFrameSet> m_ListFrameSet;
};

/**
 * Command created when jointing a cell
 */
class KWJoinCellCommand : public KCommand
{
public:
    KWJoinCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd, QList<KWFrameSet> listFrameSet,QList<KWFrame> listCopyFrame);
    ~KWJoinCellCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    unsigned int m_colBegin;
    unsigned int m_rowBegin;
    unsigned int m_colEnd;
    unsigned int m_rowEnd;
    QList<KWFrameSet> m_ListFrameSet;
    QList<KWFrame> m_copyFrame;
};

#endif
