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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRESENTER_DOC_IFACE_H
#define KPRESENTER_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <QString>
#include <qstringlist.h>
#include <QColor>
class KPrDocument;

class KPrDocumentIface : public KoDocumentIface
{
    K_DCOP
public:
    KPrDocumentIface( KPrDocument *doc_ );

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

    //Be careful ! when pgNum > num of page we return false
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

    bool showGuideLines() const;
    void setShowGuideLines( bool b );

    void addGuideLine( bool horizontal, double pos );

    unsigned int nbHorizontalHelpLine() const;
    unsigned int nbVerticalHelpLine() const;

    bool showGrid() const;
    void setShowGrid ( bool _grid );

    double gridX() const;
    void setGridX(double _x);

    double gridY() const;
    void setGridY(double _y);

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
    KPrDocument *doc;

};

#endif
