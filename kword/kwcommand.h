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
namespace Qt3 {
class QTextCommand;
}
class KWTextFrameSet;
class KWDocument;

struct FrameIndex {
    unsigned int m_iFrameIndex;	
    unsigned int m_iFrameSetIndex;
};

struct FrameResizeStruct {
    QRect sizeOfBegin;
    QRect sizeOfEnd;
};

enum FrameBorderType { FBLeft=0, FBRight=1, FBTop=2, FBBottom=3};

struct FrameBorderTypeStruct {
    FrameBorderType m_EFrameType;
    Border m_OldBorder;
};


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

using namespace Qt3;

/**
 * Command created when deleting some text
 */
class KWTextDeleteCommand : public QTextDeleteCommand
{
public:
    KWTextDeleteCommand( QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
                         const QValueList< QVector<QStyleSheetItem> > &os,
                         const QValueList<KWParagLayout> &oldParagLayouts );
    //KWTextDeleteCommand( QTextParag *p, int idx, const QArray<QTextStringChar> &str );
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    QValueList<KWParagLayout> m_oldParagLayouts;
};

/**
 * Command created when inserting some text
 */
class KWTextInsertCommand : public KWTextDeleteCommand
{
public:
    KWTextInsertCommand( QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
                         const QValueList< QVector<QStyleSheetItem> > &os,
                         const QValueList<KWParagLayout> &oldParagLayouts )
        : KWTextDeleteCommand( d, i, idx, str, os, oldParagLayouts ) {}
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
    enum Flags { All, Alignment, Counter, Margin, LineSpacing, Borders, Tabulator };
    KWTextParagCommand( QTextDocument *d, int fParag, int lParag,
                        const QValueList<KWParagLayout> &oldParagLayouts,
                        KWParagLayout newParagLayout,
                        Flags flags,
                        QStyleSheetItem::Margin margin = QStyleSheetItem::MarginLeft ); // Only meaningful if flags==Margin
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );
protected:
    int firstParag, lastParag;
    QValueList<KWParagLayout> m_oldParagLayouts;
    KWParagLayout m_newParagLayout;
    int m_flags:4;
    int m_margin:3;
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
 * Command created when changing backgroundcolor
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

/**
 * Command created when you resize a frame
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
 * Command created when you move a frame
 */
class KWFrameMoveCommand : public KCommand
{
public:
    KWFrameMoveCommand( const QString &name,KWDocument *_doc,QList<FrameIndex> &_frameIndex,QList<FrameResizeStruct>&_frameMove ) ;
    ~KWFrameMoveCommand() {}
    
    void execute();
    void unexecute();
    QList<FrameResizeStruct> getListFrameMoved() {return m_frameMove;}
    void setListFrameMoved(QList<FrameResizeStruct>_listMove) {m_frameMove=_listMove;}
protected:
    QList<FrameIndex> m_IndexFrame;
    QList<FrameResizeStruct> m_frameMove;
    KWDocument *m_pDoc;
};
#endif
