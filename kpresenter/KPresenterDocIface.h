// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#include <qstringlist.h>
#include <qcolor.h>
class KPresenterDoc;

class KPresenterDocIface : public KoDocumentIface
{
    K_DCOP
public:
    KPresenterDocIface( KPresenterDoc *doc_ );

k_dcop:
    virtual int numPages() const;
    /// 0-based
    virtual DCOPRef page( int num );

    virtual DCOPRef masterPage();

    bool selectPage( int page,bool select);


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
    virtual bool displayLink() const;
    virtual bool insertNewPage(int pos );
    virtual bool deletePage( int _page );
    virtual void deSelectAllObj();
    virtual void recalcPageNum();


    virtual void initConfig();
    virtual void saveConfig();

    virtual QString selectedForPrinting() const;

    //Be carefull ! when pgNum > num of page we return false
    virtual bool isSlideSelected( int pgNum);


    virtual DCOPRef header();
    virtual DCOPRef footer();


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

    void setConfigToolTipCompletion( bool b );
    bool configToolTipCompletion() const;

    bool configIncludeTwoUpperUpperLetterException() const;
    void setConfigIncludeTwoUpperUpperLetterException( bool b);

    bool configIncludeAbbreviation() const;
    void setConfigIncludeAbbreviation( bool b);

    bool displayComment()const;
    void setDisplayComment( bool b);

    bool showHelplines() const;
    void setShowHelplines(bool b);

    void addVertHelpLine( double val);

    void addHoriHelpLine( double val);

    double horizHelpLineValue(int index) const;

    double vertHelpLineValue(int index) const;

    //return false if we can remove help line
    bool removeVertHelpLine( int index );
    bool removeHorizHelpLine( int index );


    unsigned int nbHorizontalHelpLine() const;
    unsigned int nbVerticalHelpLine() const;

    //return false if we can find help line
    bool moveHorizontalHelpLine( int index, double newPos);
    bool moveVerticalHelpLine( int index, double newPos);


    bool showGrid() const;
    void setShowGrid ( bool _grid );

    double gridX() const;
    void setGridX(double _x);

    double gridY() const;
    void setGridY(double _y);

    bool gridToFront() const;
    void setGridToFront( bool _front );

    bool helpLineToFront() const;
    void setHelpLineToFront( bool _front );

    //return false when we idx doesn't exist
    bool updateHelpPoint( int idx, double posX, double posY ) const ;

    //return -1.0 if idx doesn't exist
    double helpPointPosX( int idx ) const ;
    double helpPointPosY( int idx ) const ;

    unsigned int nbHelpPoint() const;

    void addHelpPoint( double posX, double posY );

    //return false when index doesn't exist
    bool removeHelpPoint( int index );

    bool configAutoSuperScript() const;
    void setConfigAutoSuperScript( bool b);

    void addIgnoreWordAll( const QString &);
    void clearIgnoreWordAll( );

    QStringList spellListIgnoreAll() const;


    bool displayFieldCode()const;
    void setDisplayFieldCode( bool b);
    QString configAutoFormatLanguage( )const;

    bool configCapitalizeNameOfDays() const;
    void setConfigCapitalizeNameOfDays( bool b);

    QString presentationName() const;
    void setPresentationName( const QString &_name );

    QStringList presentationList();

    /// Repaint document. Call this after moving or resizing an object, for instance.
    /// @since 1.4
    void repaint();

private:
    KPresenterDoc *doc;

};

#endif
