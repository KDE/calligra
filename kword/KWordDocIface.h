/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KWORD_DOC_IFACE_H
#define KWORD_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>
#include <qstringlist.h>
class KWDocument;

class KWordDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KWordDocIface(  KWDocument *doc_ );

k_dcop:
    virtual DCOPRef textFrameSet( int num );
    virtual DCOPRef frameSet( int num );

    virtual int numPages();
    virtual int numFrameSets();
    virtual unsigned int nbColumns();

    //paper
    virtual double ptTopBorder();
    virtual double ptBottomBorder();
    virtual double ptLeftBorder();
    virtual double ptRightBorder();
    virtual double ptPaperHeight();
    virtual double ptPaperWidth();
    virtual double ptColumnWidth();
    virtual double ptColumnSpacing();
    virtual double ptPageTop( int pgNum );

    //configure
    virtual double gridX();
    virtual double gridY();
    virtual void setGridX(double _gridx);
    virtual void setGridY(double _gridy);
    virtual QString unitName();
    virtual double indentValue();
    virtual void setIndentValue(double _ind);

    virtual int nbPagePerRow();
    virtual void setNbPagePerRow(int _nb);
    virtual double defaultColumnSpacing();
    virtual void setDefaultColumnSpacing(int _val);
    virtual int maxRecentFiles();

    virtual void setUndoRedoLimit(int _val);

    virtual bool showRuler() const;
    virtual bool dontCheckUpperWord();
    virtual bool dontCheckTitleCase() const;
    virtual bool showdocStruct() const;
    virtual bool viewFrameBorders() const;

    virtual bool viewFormattingChars() const;
    virtual void setViewFormattingChars(bool _b);

    virtual void setHeaderVisible( bool );
    virtual void setFooterVisible( bool );

    virtual bool isHeaderVisible() const;
    virtual bool isFooterVisible() const;

    virtual void setViewFrameBorders( bool b );
    virtual void setShowRuler(bool b);
    virtual void setShowDocStruct(bool _b);

    virtual void recalcAllVariables();
    virtual void recalcVariables(int _var);
    virtual void recalcVariables(const QString &varName);
    bool setCustomVariableValue(const QString & varname, const QString & value);
    virtual QString customVariableValue(const QString & varname)const;

    virtual void setStartingPage(int nb);
    virtual int startingPage();

    virtual void setDisplayLink(bool b);
    virtual bool displayLink();

    virtual bool backgroundSpellCheckEnabled() const;
    virtual void enableBackgroundSpellCheck( bool b );

    virtual void startBackgroundSpellCheck();
    virtual void reactivateBgSpellChecking();

    virtual void setConfigUpperCase( bool _uc );
    virtual void setConfigUpperUpper( bool _uu );
    virtual void setConfigAdvancedAutocorrect( bool _aa );
    virtual void setConfigAutoDetectUrl(bool _au);
    virtual void setConfigIgnoreDoubleSpace( bool _ids);

    virtual bool configUpperCase() const;
    virtual bool configUpperUpper() const;
    virtual bool configAdvancedAutoCorrect() const;
    virtual bool configAutoDetectUrl() const;
    virtual bool configIgnoreDoubleSpace() const;

    virtual void setConfigRemoveSpaceBeginEndLine( bool _space);
    virtual void setConfigUseBulletStyle( bool _ubs);

    virtual bool configRemoveSpaceBeginEndLine() const;
    virtual bool configUseBulletSyle() const;

    virtual bool configAutoChangeFormat() const;
    virtual void setConfigAutoChangeFormat( bool _auto);

    virtual bool configAutoReplaceNumber() const;
    virtual void setConfigAutoReplaceNumber( bool b );

    virtual bool showStatusBar() const;
    virtual void setShowStatusBar( bool _status );

    virtual bool allowAutoFormat() const;
    virtual void setAllowAutoFormat(bool _b);


    void setConfigAutoNumberStyle( bool b );
    bool configAutoNumberStyle() const;

    void setConfigCompletion( bool b );
    bool configCompletion() const;

    void setConfigAppendSpace( bool b);
    bool configAppendSpace() const;

    void setConfigMinWordLength( uint val );
    uint configMinWordLength() const;

    void setConfigNbMaxCompletionWord( uint val );
    uint configNbMaxCompletionWord() const;

    void setConfigAddCompletionWord( bool b );
    bool configAddCompletionWord() const;

    bool configIncludeTwoUpperUpperLetterException() const;
    void setConfigIncludeTwoUpperUpperLetterException( bool b);

    bool configIncludeAbbreviation() const;
    void setConfigIncludeAbbreviation( bool b);

    bool displayComment()const;
    void setDisplayComment( bool b);

    bool configAutoSuperScript() const;
    void setConfigAutoSuperScript( bool b);

    bool pgUpDownMovesCaret() const;
    void setPgUpDownMovesCaret( bool b );

    void addIgnoreWordAll( const QString &);
    void clearIgnoreWordAll( );
    QStringList spellListIgnoreAll() const;

    //init/save config
    virtual void initConfig();
    virtual void saveConfig();
    virtual void initUnit();

    bool showScrollBar()const;
    void setShowScrollBar( bool _status );

    double ptTabStopValue() const;
    void setPtTabStopValue ( double _tabStop );

    bool cursorInProtectedArea()const;
    void setCursorInProtectedArea( bool b );

    bool viewFormattingEndParag() const;
    void setViewFormattingEndParag(bool _b);

    bool viewFormattingSpace() const;
    void setViewFormattingSpace(bool _b);

    bool viewFormattingTabs() const;
    void setViewFormattingTabs(bool _b);

    bool viewFormattingBreak() const;
    void setViewFormattingBreak(bool _b);
    int footNoteSeparatorLineLength() const;
    void setFootNoteSeparatorLineLength( int _length);
    void setFootNoteSeparatorLinePosition( const QString &pos);
    QString footNoteSeparatorLinePosition()const;

    double footNoteSeparatorLineWidth() const;
    void setFootNoteSeparatorLineWidth( double _width);

    void deleteBookMark(const QString &_name);
    void renameBookMark(const QString &_oldname, const QString &_newName);
    QStringList listOfBookmarkName()const;

private:
    KWDocument *doc;

};

#endif
