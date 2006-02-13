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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_COMMAND_H
#define KWORD_COMMAND_H
#include <kcommand.h>
#include <kurl.h>
#include <KoCommandHistory.h>
#include <KoPageLayout.h>
#include <KoRect.h>
#include <KoBorder.h>
#include <KoParagLayout.h>
#include <KoParagCounter.h>
#include "KWFrameStyle.h"
#include <KoTextCommand.h>
#include "KWVariable.h"
#include <KoPoint.h>
#include <KoPictureKey.h>

class KWFrameSet;
class KWTableStyle;
class KWTableTemplate;
class KWTableFrameSet;
class RemovedRow;
class RemovedColumn;
class KWPartFrameSet;
class KWDocument;
class KoCustomVariable;
class KoLinkVariable;
class KWFieldVariable;
class KWTimeVariable;
class KWDateVariable;
class KWPgNumVariable;
class KWFootNoteVariable;

// TODO: change most KNamedCommands into KCommands, removing name arguments
// and implementing name() instead (less memory used).

/**
 * Command created when pasting oasis-formatted text
 */
class KWOasisPasteCommand : public KoTextDocCommand
{
public:
    KWOasisPasteCommand( KoTextDocument *d, int parag, int idx,
                             const QByteArray & data );
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QByteArray m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KoParagLayout* m_oldParagLayout;
};

struct ParagBookmark {
    QString m_bookName;
    int m_startParagIndex;
    int m_endParagIndex;
};

class KWTextDeleteCommand : public KoTextDeleteCommand
{
public:
    KWTextDeleteCommand( KoTextDocument *d, int i, int idx, const QMemArray<KoTextStringChar> &str,
                         const CustomItemsMap & customItemsMap,
                         const QValueList<KoParagLayout> & oldParagLayouts );
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );
    void createBookmarkList();
private:
    QValueList<ParagBookmark> m_listParagBookmark;
};


////////////////////////// Frame commands ////////////////////////////////

/// Identifies a frame
struct FrameIndex {
    FrameIndex() {}
    FrameIndex( KWFrame *frame );

    KWFrameSet * m_pFrameSet;
    unsigned int m_iFrameIndex;
};


/**
 * Command created when changing frame border
 */
class KWFrameBorderCommand : public KNamedCommand
{
public:
    //enum FrameBorderType { FBLeft=0, FBRight=1, FBTop=2, FBBottom=3};
    struct FrameBorderTypeStruct {
        KoBorder::BorderType m_EFrameType;
        KoBorder m_OldBorder;
    };

    KWFrameBorderCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<FrameBorderTypeStruct> &_frameTypeBorder,const KoBorder & _newBorder ) ;
    ~ KWFrameBorderCommand();

    void execute();
    void unexecute();

protected:
    QPtrList<FrameIndex> m_indexFrame;
    QPtrList<FrameBorderTypeStruct> m_oldBorderFrameType;
    KoBorder m_newBorder;
};

/**
 * Command created when changing background color of one or more frames
 */
class KWFrameBackGroundColorCommand : public KNamedCommand
{
public:
    KWFrameBackGroundColorCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<QBrush> &_oldBrush, const QBrush & _newColor ) ;
    ~KWFrameBackGroundColorCommand();

    void execute();
    void unexecute();

protected:
    QPtrList<FrameIndex> m_indexFrame;
    QPtrList<QBrush> m_oldBackGroundColor;
    QBrush m_newColor;
};

/**
 * Command created when applying a framestyle
 */
class KWFrameStyleCommand : public KNamedCommand
{
public:
    KWFrameStyleCommand( const QString &name, KWFrame *_frame, KWFrameStyle *_fs, bool _repaintViews = true );
    ~ KWFrameStyleCommand() { if (m_oldValues) delete m_oldValues; }

    void execute();
    void unexecute();

protected:
    void applyFrameStyle( KWFrameStyle * _sty );
    KWFrame *m_frame ;
    KWFrameStyle * m_fs;
    KWFrameStyle * m_oldValues;
    bool repaintViews;
};


/**
 * Command created when applying a tablestyle
 */
class KWTableStyleCommand : public KNamedCommand
{
public:
    KWTableStyleCommand( const QString &name, KWFrame *_frame, KWTableStyle *_ts, bool _repaintViews = true );
    ~ KWTableStyleCommand();

    void execute();
    void unexecute();

protected:
    KWFrame *m_frame ;
    KWTableStyle * m_ts;
    KWFrameStyleCommand * m_fsc;
    KCommand * m_sc;
    bool repaintViews;
};

/**
 * Command created when applying a tabletemplate
 */
class KWTableTemplateCommand : public KNamedCommand
{
public:
    KWTableTemplateCommand( const QString &name, KWTableFrameSet *_table, KWTableTemplate *_tt );
    ~ KWTableTemplateCommand();

    void execute();
    void unexecute();

protected:
    KWTableFrameSet *m_table;
    KWTableTemplate * m_tt;
    KMacroCommand * m_tableCommands;
};

struct FrameResizeStruct {
    // Note that the new "minimum frame height" is always s2.height(),
    // since this is called when the user manually resizes a frame (not when
    // some text changes the size of a frame).
    FrameResizeStruct() { } // default constructor;
    FrameResizeStruct( const KoRect& s1, double min1, const KoRect& s2 )
        : oldRect(s1), oldMinHeight(min1),
          newRect( s2), newMinHeight(s2.height()) {
    }
    KoRect oldRect;
    double oldMinHeight;
    KoRect newRect;
    double newMinHeight;
};

/**
 * Command created when a frame is resized
 * (or "moved and resized" as for KWPartFrameSet)
 */
class KWFrameResizeCommand : public KNamedCommand
{
public:
    KWFrameResizeCommand(const QString &name, const QValueList<FrameIndex> &frameIndex, const QValueList<FrameResizeStruct> &frameResize );
    KWFrameResizeCommand(const QString &name, FrameIndex frameIndex, const FrameResizeStruct &frameResize );
    ~KWFrameResizeCommand() {}

    void execute();
    void unexecute();

protected:
    QValueList<FrameIndex> m_indexFrame;
    QValueList<FrameResizeStruct> m_frameResize;
};

/**
 * Command created when we changed a clipart or picture
 */
class KWFrameChangePictureCommand : public KNamedCommand
{
public:
    KWFrameChangePictureCommand( const QString &name, FrameIndex _frameIndex, const KoPictureKey & _oldFile, const KoPictureKey & _newFile) ;
    ~KWFrameChangePictureCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_indexFrame;
    KoPictureKey m_oldKey;
    KoPictureKey m_newKey;
};

struct FrameMoveStruct {
    FrameMoveStruct() {} // for QValueList
    FrameMoveStruct( const KoPoint& p1, const KoPoint& p2 )
        : oldPos(p1), newPos(p2) {
    }
    KoPoint oldPos;
    KoPoint newPos;
};

/**
 * Command created when one or more frames are moved
 */
class KWFrameMoveCommand : public KNamedCommand
{
public:
    KWFrameMoveCommand( const QString &name,
                        const QValueList<FrameIndex> & _frameIndex,
                        const QValueList<FrameMoveStruct> & _frameMove );
    ~KWFrameMoveCommand() {}

    void execute();
    void unexecute();
    QValueList<FrameMoveStruct> & listFrameMoved() { return m_frameMove; }
protected:
    QValueList<FrameIndex> m_indexFrame;
    QValueList<FrameMoveStruct> m_frameMove;
};

/**
 * Command created when the properties of a frame are changed
 * (e.g. using frame dialog).
 * In the long run, KWFrameBackGroundColorCommand, KWFrameBorderCommand etc.
 * could be removed and KWFramePropertiesCommand could be used instead.
 * #### This solution is memory eating though, since all settings of the frame
 * are copied. TODO: evaluate using graphite's GenericCommand instead.
 */
class KWFramePropertiesCommand : public KNamedCommand
{
public:
    /** A copy of frameAfter is done internally.
     * But make sure frameBefore is already a copy, its ownership is moved to the command.
     */
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
class KWFramePartMoveCommand : public KNamedCommand
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
 * Command created when a part with an external document is changed to be stored internal
 */
class KWFramePartInternalCommand : public KNamedCommand
{
public:
    KWFramePartInternalCommand( const QString &name, KWPartFrameSet *part ) ;
    ~KWFramePartInternalCommand() {}

    void execute();
    void unexecute();
protected:
    KWPartFrameSet *m_part;
    KURL m_url;
};

/**
 * Command created when a part with an external document is changed to be stored internal
 */
class KWFramePartExternalCommand : public KNamedCommand
{
public:
    KWFramePartExternalCommand( const QString &name, KWPartFrameSet *part ) ;
    ~KWFramePartExternalCommand() {}

    void execute();
    void unexecute();
protected:
    KWPartFrameSet *m_part;
};

/**
 * This command makes a frameset floating and non-floating
 */
class KWFrameSetInlineCommand : public KNamedCommand
{
public:
    KWFrameSetInlineCommand( const QString &name, KWFrameSet *frameset,
                             bool value );
    ~KWFrameSetInlineCommand() {}

    void execute();
    void unexecute();

protected:
    void setValue( bool value );

    KWFrameSet* m_pFrameSet;
    bool m_value;
    bool m_oldValue;
};

///////////////////////////////layout command///////////////////////////
struct KWPageLayoutStruct {
    KWPageLayoutStruct( const KoPageLayout& pgLayout, const KoColumns& cl, const KoKWHeaderFooter& hf )
        : _pgLayout(pgLayout), _cl(cl), _hf(hf) {
    }
    KoPageLayout _pgLayout;
    KoColumns _cl;
    KoKWHeaderFooter _hf;
};

/**
 * Command created when changing the page layout
 */
class KWPageLayoutCommand : public KNamedCommand
{
public:
    KWPageLayoutCommand( const QString &name, KWDocument *_doc, KWPageLayoutStruct &_oldLayout, KWPageLayoutStruct &_newLayout);
    ~KWPageLayoutCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    KWPageLayoutStruct m_oldLayout;
    KWPageLayoutStruct m_newLayout;
};


/**
 * Command created when deleting a frame
 */
class KWDeleteFrameCommand : public KNamedCommand
{
public:
    KWDeleteFrameCommand( const QString &name, KWFrame * frame) ;
    /**
     * Constructor to be used for chaining commands, i.e. inside another command.
     * @param frameIndex a frameIndex object that specifies which frame to delete
     */
    KWDeleteFrameCommand( const FrameIndex &frameIndex) ;
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
class KWUngroupTableCommand : public KNamedCommand
{
public:
    KWUngroupTableCommand( const QString &name, KWTableFrameSet * _table) ;
    ~KWUngroupTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrame;
};

/**
 * Command created when deleting a table
 */
class KWDeleteTableCommand : public KNamedCommand
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
class KWInsertColumnCommand : public KNamedCommand
{
public:
    /* for the last parameter, _maxRight, you should pass the maximum offset that the table can use at its right (normally m_maxRight - m_pTable->boundingRect().left())*/
    KWInsertColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos, double _maxRight);
    ~KWInsertColumnCommand();

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    RemovedColumn *m_rc;
    unsigned int m_colPos;
    double m_maxRight; // this is the maximum x of the right part of the table (used so that the table does no go off the page)
    double m_oldWidth; // will be 0 after execute() if the width of the table was not changed by the operation
    bool m_inserted;
};


/**
 * Command created when inserting a row
 */
class KWInsertRowCommand : public KNamedCommand
{
public:
    KWInsertRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWInsertRowCommand();

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    RemovedRow *m_rr;
    unsigned int m_rowPos;
    bool m_inserted;
};

/**
 * Command created when removing a row
 */
class KWRemoveRowCommand : public KNamedCommand
{
public:
    KWRemoveRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveRowCommand();

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    RemovedRow *m_rr;
    uint m_rowPos;
};

/**
 * Command created when removing a column
 */
class KWRemoveColumnCommand : public KNamedCommand
{
public:
    KWRemoveColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveColumnCommand();

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    RemovedColumn *m_rc;
    uint m_colPos;
};

/**
 * Command created when splitting a cell
 */
class KWSplitCellCommand : public KNamedCommand
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
    QPtrList<KWFrameSet> m_ListFrameSet;
};

/**
 * Command created when joining cells
 */
class KWJoinCellCommand : public KNamedCommand
{
public:
    KWJoinCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd, QPtrList<KWFrameSet> listFrameSet,QPtrList<KWFrame> listCopyFrame);
    ~KWJoinCellCommand();

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    unsigned int m_colBegin;
    unsigned int m_rowBegin;
    unsigned int m_colEnd;
    unsigned int m_rowEnd;
    QPtrList<KWFrameSet> m_ListFrameSet;
    QPtrList<KWFrame> m_copyFrame;
};

/**
 * Command to starting page setting
 */
class KWChangeStartingPageCommand : public KNamedCommand
{
public:
    KWChangeStartingPageCommand( const QString &name, KWDocument *_doc, int _oldStartingPage, int _newStartingPage);
    ~KWChangeStartingPageCommand(){}

    void execute();
    void unexecute();
protected:
    KWDocument *m_doc;
    int oldStartingPage;
    int newStartingPage;
};

/**
 * Command to display link setting
 */
class KWChangeVariableSettingsCommand : public KNamedCommand
{
public:
    enum VariableProperties { VS_DISPLAYLINK, VS_UNDERLINELINK, VS_DISPLAYCOMMENT, VS_DISPLAYFIELDCODE};
    KWChangeVariableSettingsCommand( const QString &name, KWDocument *_doc, bool _oldDisplay, bool _newDisplay, VariableProperties _type);
    ~KWChangeVariableSettingsCommand(){}

    void execute();
    void unexecute();
protected:
    void changeValue( bool b );
    KWDocument *m_doc;
    VariableProperties type;
    bool m_bOldValue;
    bool m_bNewValue;
};

class KWChangeCustomVariableValue : public KNamedCommand
{
 public:
    KWChangeCustomVariableValue( const QString &name, KWDocument *_doc,const QString & _oldValue, const QString & _newValue, KoCustomVariable *var);
    ~KWChangeCustomVariableValue();
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    QString newValue;
    QString oldValue;
    KoCustomVariable *m_var;
};

class KWChangeVariableNoteText : public KNamedCommand
{
 public:
    KWChangeVariableNoteText( const QString &name, KWDocument *_doc, const QString &_oldValue,const QString &_newValue, KoNoteVariable *var);
    ~KWChangeVariableNoteText();
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    QString newValue;
    QString oldValue;
     KoNoteVariable *m_var;
};

class KWChangeLinkVariable : public KNamedCommand
{
 public:
    KWChangeLinkVariable( const QString &name, KWDocument *_doc,const QString & _oldHref, const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var);
    ~KWChangeLinkVariable(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    QString oldHref;
    QString newHref;
    QString oldLink;
    QString newLink;
    KoLinkVariable *m_var;
};

class KWHideShowHeader : public KNamedCommand
{
 public:
    KWHideShowHeader( const QString &name, KWDocument *_doc, bool _newValue);
    ~KWHideShowHeader(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    bool newValue;
};

class KWHideShowFooter : public KNamedCommand
{
 public:
    KWHideShowFooter( const QString &name, KWDocument *_doc, bool _newValue);
    ~KWHideShowFooter(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    bool newValue;
};


class KWProtectContentCommand : public KNamedCommand
{
public:
    KWProtectContentCommand( const QString &name, KWTextFrameSet*frameset,bool protect );
    ~KWProtectContentCommand() {}

    void execute();
    void unexecute();

protected:
    KWTextFrameSet* m_pFrameSet;
    bool m_bProtect;
};

class KWInsertRemovePageCommand : public KCommand
{
 public:
    enum Command { Insert, Remove };
    KWInsertRemovePageCommand( KWDocument *_doc, Command cmd, int pgNum);
    ~KWInsertRemovePageCommand();
    virtual QString name() const;
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    Command m_cmd;
    int m_pgNum;
    QValueList<KCommand*> childCommands;

    void doRemove(int pageNumber);
    void doInsert(int pageNumber);

 private:
    static bool compareIndex(const FrameIndex &index1, const FrameIndex &index2);
};

struct FramePaddingStruct {
    FramePaddingStruct() {}
    FramePaddingStruct( KWFrame *frame );
    FramePaddingStruct( double _left, double top, double right, double bottom );
    double topPadding;
    double bottomPadding;
    double leftPadding;
    double rightPadding;
};

class KWFrameChangeFramePaddingCommand :public KNamedCommand
{
public:
    KWFrameChangeFramePaddingCommand( const QString &name, FrameIndex _frameIndex, FramePaddingStruct _framePaddingBegin, FramePaddingStruct _framePaddingEnd );
    ~KWFrameChangeFramePaddingCommand() {}
    void execute();
    void unexecute();
protected:
    FrameIndex m_indexFrame;
    FramePaddingStruct m_framePaddingBegin;
    FramePaddingStruct m_framePaddingEnd;
};


class KWChangeFootEndNoteSettingsCommand : public KNamedCommand
{
public:
    KWChangeFootEndNoteSettingsCommand( const QString &name, KoParagCounter _oldCounter, KoParagCounter _newCounter ,bool _footNote ,KWDocument *_doc );
    ~KWChangeFootEndNoteSettingsCommand() {}

    void execute();
    void unexecute();

protected:
    void changeCounter( KoParagCounter counter);
    KoParagCounter m_oldCounter;
    KoParagCounter m_newCounter;
    bool m_footNote;
    KWDocument *m_doc;
};

class KWChangeTabStopValueCommand : public KNamedCommand
{
public:
    KWChangeTabStopValueCommand( const QString &name,  double _oldValue, double _newValue, KWDocument *_doc);

    ~KWChangeTabStopValueCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KWDocument *m_doc;
    double m_oldValue;
    double m_newValue;
};


struct FootNoteParameter {
    FootNoteParameter() {}
    FootNoteParameter( KWFootNoteVariable *_var );
    FootNoteParameter( NoteType _noteType, KWFootNoteVariable::Numbering _numberingType, const QString &_manualString);
    NoteType noteType;
    KWFootNoteVariable::Numbering numberingType;
    QString manualString;
};

class KWChangeFootNoteParametersCommand : public KNamedCommand
{
public:
    KWChangeFootNoteParametersCommand( const QString &name, KWFootNoteVariable * _var, FootNoteParameter _oldParameter, FootNoteParameter _newParameter, KWDocument *_doc);

    ~KWChangeFootNoteParametersCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    void changeVariableParameter( FootNoteParameter _param );
    KWDocument *m_doc;
    KWFootNoteVariable *m_var;
    FootNoteParameter m_oldParameter;
    FootNoteParameter m_newParameter;
};


class KWChangeFootNoteLineSeparatorParametersCommand : public KNamedCommand
{
public:
    KWChangeFootNoteLineSeparatorParametersCommand( const QString &name, SeparatorLinePos _oldValuePos, SeparatorLinePos _newValuePos, int _oldLength, int _newLength, double _oldWidth, double _newWidth,SeparatorLineLineType _oldLineType, SeparatorLineLineType _newLineType, KWDocument *_doc);

    ~KWChangeFootNoteLineSeparatorParametersCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    void changeLineSeparatorParameter( SeparatorLinePos _pos, int length, double _width, SeparatorLineLineType _type);
    KWDocument *m_doc;
    SeparatorLinePos m_oldValuePos;
    SeparatorLinePos m_newValuePos;
    int m_oldLength;
    int m_newLength;
    double m_oldWidth;
    double m_newWidth;
    SeparatorLineLineType m_oldLineType;
    SeparatorLineLineType m_newLineType;
};

class KWRenameBookmarkCommand : public KNamedCommand
{
public:
    KWRenameBookmarkCommand( const QString &name, const QString & _oldname, const QString & _newName, KWDocument *_doc);

    ~KWRenameBookmarkCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KWDocument *m_doc;
    QString m_oldName;
    QString m_newName;
};

class KWResizeColumnCommand : public KNamedCommand
{
public:
    KWResizeColumnCommand( KWTableFrameSet *table, int col, double oldSize, double newSize );

    ~KWResizeColumnCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KWTableFrameSet *m_table;
    double m_oldSize;
    double m_newSize;
    int m_col;
};

class KWResizeRowCommand : public KNamedCommand
{
  public:
    KWResizeRowCommand( KWTableFrameSet *table, int row, double oldSize, double newSize );

    ~KWResizeRowCommand() {}

    virtual void execute();
    virtual void unexecute();
  protected:
    KWTableFrameSet *m_table;
    double m_oldSize;
    double m_newSize;
    int m_row;
};

#endif
