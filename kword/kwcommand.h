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
#include <koborder.h>
#include <koparaglayout.h>
class KWFrameSet;
class KWTableFrameSet;
class KWDocument;

/**
 * Command created when pasting formatted text
 * [relies on KWord's XML structure, so not moved to kotext]
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
    KoParagLayout m_oldParagLayout;
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
    KoBorder m_OldBorder;
};

/**
 * Command created when changing frame border
 */
class KWFrameBorderCommand : public KCommand
{
public:
    KWFrameBorderCommand( const QString &name, QList<FrameIndex> &_listFrameIndex, QList<FrameBorderTypeStruct> &_frameTypeBorder,const KoBorder & _newBorder ) ;
    ~ KWFrameBorderCommand() {}

    void execute();
    void unexecute();

protected:
    QList<FrameIndex> m_indexFrame;
    QList<FrameBorderTypeStruct> m_oldBorderFrameType;
    KoBorder m_newBorder;
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
 * Command created when we changed a clipart or picture
 */
class KWFrameChangePictureClipartCommand : public KCommand
{
public:
    KWFrameChangePictureClipartCommand( const QString &name, FrameIndex _frameIndex, const QString & _oldFile, const QString &_newFile, bool _isAPicture) ;
    ~KWFrameChangePictureClipartCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_indexFrame;
    QString m_oldFile;
    QString m_newFile;
    bool m_isAPicture;
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
 * Command created when the properties of a frame are changed
 * (e.g. using frame dialog).
 * In the long run, KWFrameBackGroundColorCommand, KWFrameBorderCommand etc.
 * could be removed and KWFramePropertiesCommand could be used instead.
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
 * Command created when one part is moved or resized
 */
class KWFramePartMoveCommand : public KCommand
{
public:
    KWFramePartMoveCommand( const QString &name,FrameIndex _frameIndex,FrameResizeStruct _frameMove ) ;
    ~KWFramePartMoveCommand() {}

    void execute();
    void unexecute();
    FrameResizeStruct & listFrameMoved() { return m_frameMove; }
    bool frameMoved();
protected:
    FrameIndex m_indexFrame;
    FrameResizeStruct m_frameMove;
};

/**
 * This command changes one property of a frameset.
 */
class KWFrameSetPropertyCommand : public KCommand
{
public:
    enum Property { FSP_NAME, FSP_FLOATING };
    KWFrameSetPropertyCommand( const QString &name, KWFrameSet *frameset,
		               Property prop, const QString & value );
    ~KWFrameSetPropertyCommand() {}

    void execute();
    void unexecute();

protected:
    void setValue( const QString &value );

    KWFrameSet* m_pFrameSet;
    Property m_property;
    QString m_value;
    QString m_oldValue;
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
    KWPageLayoutCommand( const QString &name, KWDocument *_doc, pageLayout &_oldLayout, pageLayout &_newLayout);
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
    ~KWDeleteFrameCommand();

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
