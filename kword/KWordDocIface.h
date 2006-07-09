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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_DOC_IFACE_H
#define KWORD_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>
#include <qstringlist.h>
class KWDocument;

class KWordDocIface : public KoDocumentIface
{
    K_DCOP
public:
    KWordDocIface(  KWDocument *doc_ );

k_dcop:
    virtual DCOPRef textFrameSet( int num );
    virtual DCOPRef frameSet( int num );

    /// deprecated method, use frameSetCount instead.
    virtual int numFrameSets()const;
    virtual int frameSetCount()const;
    virtual unsigned int nbColumns()const;

    //paper
    virtual double ptPageTop( int pgNum )const;

    //configure
    virtual QString unitName()const;

    virtual bool showRuler() const;
    virtual bool showdocStruct() const;
    virtual bool viewFrameBorders() const;

    virtual bool showGrid() const;
    virtual bool snapToGrid() const;

    virtual void setGridX( double _gridX );
    virtual void setGridY( double _gridY );
    virtual void setShowGrid( bool _b );
    virtual void setSnapToGrid( bool _b );
    virtual double gridY() const;
    virtual double gridX() const;

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
    virtual int startingPage()const;

    virtual void setDisplayLink(bool b);
    virtual bool displayLink()const;

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

    void setConfigAutoNumberStyle( bool b );
    bool configAutoNumberStyle() const;

    void setConfigToolTipCompletion( bool b );
    bool configToolTipCompletion() const;

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

    void addIgnoreWordAll( const QString &);
    void clearIgnoreWordAll( );
    QStringList spellListIgnoreAll() const;

    //init/save config
    virtual void initConfig();
    virtual void saveConfig();

    bool showScrollBar()const;
    void setShowScrollBar( bool _status );

    double ptTabStopValue() const;
    void setPtTabStopValue ( double _tabStop );

    int footNoteSeparatorLineLength() const;
    void setFootNoteSeparatorLineLength( int _length);
    void setFootNoteSeparatorLinePosition( const QString &pos);
    QString footNoteSeparatorLinePosition()const;

    double footNoteSeparatorLineWidth() const;
    void setFootNoteSeparatorLineWidth( double _width);

    void deleteBookMark(const QString &_name);
    void renameBookMark(const QString &_oldname, const QString &_newName);
    QStringList listOfBookmarkName()const;

    bool displayFieldCode()const;
    void setDisplayFieldCode( bool b);
    QString configAutoFormatLanguage( )const;

    bool configCapitalizeNameOfDays() const;
    void setConfigCapitalizeNameOfDays( bool b);

private:
    KWDocument *doc;

};

#endif
