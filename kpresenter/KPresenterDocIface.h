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

#ifndef KPRESENTER_DOC_IFACE_H
#define KPRESENTER_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <qstring.h>

class KPresenterDoc;

class KPresenterDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KPresenterDocIface( KPresenterDoc *doc_ );

k_dcop:
    virtual int numPages();
    virtual DCOPRef page( int num );
    bool selectPage( int page,bool select);

    virtual double indentValue();
    virtual void setIndentValue(double _ind);

    virtual bool dontCheckUpperWord();
    virtual void setDontCheckUpperWord(bool _b);

    virtual bool dontCheckTitleCase();
    virtual void setDontCheckTitleCase(bool _b);
    virtual int maxRecentFiles();

    virtual void setUndoRedoLimit(int val);

    virtual void setShowRuler(bool );
    virtual bool showRuler() const;

    virtual void recalcVariables( int type );
    virtual void recalcAllVariables();
    virtual void recalcVariables(const QString &varName);
    virtual bool setCustomVariableValue(const QString & varname, const QString & value);
    virtual QString customVariableValue(const QString & varname) const;

    virtual int startingPage();
    virtual void setStartingPage(int nb);

    virtual void setDisplayLink(bool b);
    virtual bool displayLink();
    virtual bool insertNewPage(int pos );
    virtual bool deletePage( int _page );
    virtual void deSelectAllObj();
    virtual void recalcPageNum();

    virtual bool presentationDuration() const;
    virtual void setPresentationDuration( bool pd );

    virtual bool spInfinitLoop() const;
    virtual bool spManualSwitch() const;
    virtual void setInfinitLoop( bool il );
    virtual void setManualSwitch( bool ms );

    virtual void setHeader( bool b );
    virtual void setFooter( bool b );

    virtual void initConfig();
    virtual void saveConfig();

    virtual QString selectedForPrinting() const;

    virtual int leftBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

    //Be carefull ! when pgNum > num of page we return false
    virtual bool isSlideSelected( int pgNum);


    virtual bool hasFooter() const;
    virtual bool hasHeader() const;
    virtual DCOPRef header();
    virtual DCOPRef footer();

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

    void setConfigAutoCompletion( bool b );
    bool configAutoCompletion() const;

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

    bool showHelplines() const;
    void setShowHelplines(bool b);


private:
    KPresenterDoc *doc;

};

#endif
