/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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

#include "paraglayout.h"
#include "kword_doc.h"
#include "defs.h"
#include "kword_utils.h"

#include <kdebug.h>
#include <komlMime.h>

#include <strstream>
#include <fstream>

#include <unistd.h>

#include <qfont.h>
#include <qlist.h>

/******************************************************************/
/* Class: KWParagLayout                                           */
/******************************************************************/

/*================================================================*/
KWParagLayout::KWParagLayout( KWordDocument *_doc, bool _add, QString _name )
    : format( _doc ), paragFootOffset(), paragHeadOffset(), firstLineLeftIndent(), leftIndent(), lineSpacing()
{
    flow = LEFT;
    counter.counterType = CT_NONE;
    counter.counterDepth = 0;
    counter.counterBullet = QChar( '·' );
    counter.counterLeftText = "";
    counter.counterRightText = "";
    followingParagLayout = "Standard";
    name = _name;
    counter.startCounter = 1; // better than 0;
    counter.numberingType = NT_LIST;
    counter.bulletFont = "symbol";
    counter.customCounterDef = "";

    left.color = Qt::white;
    left.style = SOLID;
    left.ptWidth = 0;
    right.color = Qt::white;
    right.style = SOLID;
    right.ptWidth = 0;
    top.color = Qt::white;
    top.style = SOLID;
    top.ptWidth = 0;
    bottom.color = Qt::white;
    bottom.style = SOLID;
    bottom.ptWidth = 0;

    format.setDefaults( _doc );

    document = _doc;
    if ( _add )
        document->paragLayoutList.append( this );

    tabList.setAutoDelete( false );
    specialTabs = false;
}

/*================================================================*/
KWParagLayout::~KWParagLayout()
{
  int index = document->paragLayoutList.findRef(this);
  if (index >= 0)
    document->paragLayoutList.take( index );
  tabList.setAutoDelete( true );
}

/*================================================================*/
KWParagLayout& KWParagLayout::operator=( const KWParagLayout &_layout )
{
    flow = _layout.flow;
    paragFootOffset = _layout.paragFootOffset;
    paragHeadOffset = _layout.paragHeadOffset;
    firstLineLeftIndent = _layout.firstLineLeftIndent;
    leftIndent = _layout.leftIndent;
    counter.counterType = _layout.counter.counterType;
    counter.counterDepth = _layout.counter.counterDepth;
    counter.counterBullet = _layout.counter.counterBullet;
    counter.counterLeftText = _layout.counter.counterLeftText;
    counter.counterRightText = _layout.counter.counterRightText;
    followingParagLayout = _layout.followingParagLayout;
    lineSpacing = _layout.lineSpacing;
    counter.startCounter = _layout.counter.startCounter;
    counter.numberingType = _layout.counter.numberingType;
    counter.bulletFont = _layout.counter.bulletFont;
    counter.customCounterDef = _layout.counter.customCounterDef;
    name = _layout.name;

    left = _layout.left;
    right = _layout.right;
    top = _layout.top;
    bottom = _layout.bottom;

    format = _layout.format;

    tabList.setAutoDelete( true );
    tabList.clear();
    tabList.setAutoDelete( false );
    specialTabs = false;
    const QList<KoTabulator> *_tabList = &_layout.tabList;
    setTabList( _tabList );

    return *this;
}

/*================================================================*/
void KWParagLayout::setFollowingParagLayout( const QString& _name )
{
    followingParagLayout = _name;
}

/*================================================================*/
void KWParagLayout::setFormat( const KWFormat &_f )
{
    format = _f;
}

/*================================================================*/
void KWParagLayout::save( QTextStream&out )
{
    out << indent << "<NAME value=\"" << correctQString( name ).latin1() << "\"/>" << endl;
    out << indent << "<FOLLOWING name=\"" << correctQString( followingParagLayout ).latin1() << "\"/>" << endl;
    out << indent << "<FLOW value=\"" << static_cast<int>( flow ) << "\"/>" << endl;
    out << indent << "<OHEAD " << paragHeadOffset << "/>" << endl;
    out << indent << "<OFOOT " << paragFootOffset << "/>" << endl;
    out << indent << "<IFIRST " << firstLineLeftIndent << "/>" << endl;
    out << indent << "<ILEFT " << leftIndent << "/>" << endl;
    out << indent << "<LINESPACE " << lineSpacing << "/>" << endl;
    out << indent << "<COUNTER type=\"" << static_cast<int>( counter.counterType ) << "\" depth=\"" << counter.counterDepth
        << "\" bullet=\"" << static_cast<unsigned short>( counter.counterBullet.unicode() ) << "\" start=\""
        << QString::number(counter.startCounter).latin1() << "\" numberingtype=\""
        << static_cast<int>( counter.numberingType ) << "\" lefttext=\""
        << correctQString( counter.counterLeftText ).latin1() << "\" righttext=\""
        << correctQString( counter.counterRightText ).latin1() << "\" bulletfont=\""
        << correctQString( counter.bulletFont ).latin1() << "\" customdef=\""
        << correctQString( counter.customCounterDef ).latin1() << "\" />" << endl;
    out << indent << "<LEFTBORDER red=\"" << left.color.red() << "\" green=\"" << left.color.green() << "\" blue=\""
        << left.color.blue() << "\" style=\"" << static_cast<int>( left.style ) << "\" width=\"" << left.ptWidth << "\"/>" << endl;
    out << indent << "<RIGHTBORDER red=\"" << right.color.red() << "\" green=\"" << right.color.green() << "\" blue=\""
        << right.color.blue() << "\" style=\"" << static_cast<int>( right.style ) << "\" width=\"" << right.ptWidth << "\"/>" << endl;
    out << indent << "<TOPBORDER red=\"" << top.color.red() << "\" green=\"" << top.color.green() << "\" blue=\""
        << top.color.blue() << "\" style=\"" << static_cast<int>( top.style ) << "\" width=\"" << top.ptWidth << "\"/>" << endl;
    out << indent << "<BOTTOMBORDER red=\"" << bottom.color.red() << "\" green=\"" << bottom.color.green() << "\" blue=\""
        << bottom.color.blue() << "\" style=\"" << static_cast<int>( bottom.style )
        << "\" width=\"" << bottom.ptWidth << "\"/>" << endl;
    out << otag << "<FORMAT>" << endl;
    format.save( out );
    out << etag << "</FORMAT> " << endl;

    for ( unsigned int i = 0; i < tabList.count(); i++ )
        out << indent << "<TABULATOR mmpos=\"" << tabList.at( i )->mmPos << "\" ptpos=\"" << tabList.at( i )->ptPos
            << "\" inchpos=\"" << tabList.at( i )->inchPos << "\" type=\""
            << static_cast<int>( tabList.at( i )->type ) << "\"/>" << endl;
}

/*================================================================*/
void KWParagLayout::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string _name;
    double pt, mm, inch;

    while ( parser.open( 0L, tag ) ) {
        parser.parseTag( tag.c_str(), _name, lst );

        // name
        if ( _name == "NAME" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "value" )
                    name = correctQString( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "FOLLOWING" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "name" )
                    followingParagLayout = correctQString( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "TABULATOR" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            KoTabulator *tab = new KoTabulator;
            bool noinch = true;
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "mmpos" )
                    tab->mmPos = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "ptpos" )
                    tab->ptPos = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inchpos" ) {
                    noinch = false;
                    tab->inchPos = atof( ( *it ).m_strValue.c_str() );
                }
                if ( ( *it ).m_strName == "type" )
                    tab->type = static_cast<KoTabulators>( atoi( ( *it ).m_strValue.c_str() ) );
            }
            if ( noinch ) tab->inchPos = MM_TO_INCH( tab->mmPos );
            tabList.append( tab );
        } else if ( _name == "FLOW" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "value" )
                    flow = static_cast<Flow>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        } else if ( _name == "OHEAD" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "mm" )
                    mm = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inch" )
                    inch = atof( ( *it ).m_strValue.c_str() );
            }
            paragHeadOffset.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "OFOOT" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "mm" )
                    mm = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inch" )
                    inch = atof( ( *it ).m_strValue.c_str() );
            }
            paragFootOffset.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "IFIRST" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "mm" )
                    mm = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inch" )
                    inch = atof( ( *it ).m_strValue.c_str() );
            }
            firstLineLeftIndent.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "ILEFT" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "mm" )
                    mm = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inch" )
                    inch = atof( ( *it ).m_strValue.c_str() );
            }
            leftIndent.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "LINESPACE" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "mm" )
                    mm = atof( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "inch" )
                    inch = atof( ( *it ).m_strValue.c_str() );
            }
            lineSpacing.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "OFFSETS" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "head" )
                    paragHeadOffset.setMM( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "foot" )
                    paragFootOffset.setMM( atof( ( *it ).m_strValue.c_str() ) );
            }
        } else if ( _name == "INDENTS" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "first" )
                    firstLineLeftIndent.setMM( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "left" )
                    leftIndent.setMM( atof( ( *it ).m_strValue.c_str() ) );
            }
        } else if ( _name == "LINESPACING" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "value" )
                    lineSpacing.setPT( atof( ( *it ).m_strValue.c_str() ) );
            }
        } else if ( _name == "COUNTER" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "type" )
                    counter.counterType = static_cast<CounterType>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "depth" )
                    counter.counterDepth = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "bullet" )
                    counter.counterBullet = QChar( static_cast<unsigned short>( atoi( ( *it ).m_strValue.c_str() ) ) );
                else if ( ( *it ).m_strName == "lefttext" )
                    counter.counterLeftText = correctQString( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "righttext" )
                    counter.counterRightText = correctQString( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "start" )
                {
                    QString s = QString::fromUtf8(( *it ).m_strValue.c_str());
                    if ( s[0].isDigit() )
                      counter.startCounter = atoi( s.latin1() );
                    else // support for old files (DF)
                      counter.startCounter = s.lower()[0].latin1() - 'a' + 1;
                }
                else if ( ( *it ).m_strName == "numberingtype" )
                    counter.numberingType = static_cast<NumType>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "bulletfont" )
                    counter.bulletFont = correctQString( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "customdef" )
                    counter.customCounterDef = correctQString( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "LEFTBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = atoi( ( *it ).m_strValue.c_str() );
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = atoi( ( *it ).m_strValue.c_str() );
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = atoi( ( *it ).m_strValue.c_str() );
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    left.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "width" )
                    left.ptWidth = atoi( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "RIGHTBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = atoi( ( *it ).m_strValue.c_str() );
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = atoi( ( *it ).m_strValue.c_str() );
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = atoi( ( *it ).m_strValue.c_str() );
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    right.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "width" )
                    right.ptWidth = atoi( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "BOTTOMBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = atoi( ( *it ).m_strValue.c_str() );
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = atoi( ( *it ).m_strValue.c_str() );
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = atoi( ( *it ).m_strValue.c_str() );
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    bottom.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "width" )
                    bottom.ptWidth = atoi( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "TOPBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = atoi( ( *it ).m_strValue.c_str() );
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = atoi( ( *it ).m_strValue.c_str() );
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = atoi( ( *it ).m_strValue.c_str() );
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    top.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "width" )
                    top.ptWidth = atoi( ( *it ).m_strValue.c_str() );
            }
        } else if ( _name == "FORMAT" ) {
            parser.parseTag( tag.c_str(), _name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            format.load( parser, lst, document );
        } else
            kdError(32001) << "Unknown tag '" << tag.c_str() << "' in PARAGRAPHLAYOUT" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag.c_str() << endl;
            return;
        }
    }
}

/*================================================================*/
void KWParagLayout::setTabList( const QList<KoTabulator> *_tabList )
{
    tabList.setAutoDelete( true );
    tabList.clear();
    tabList.setAutoDelete( false );
    specialTabs = false;

    QListIterator<KoTabulator> it(*_tabList);
    for ( it.toFirst(); it.current(); ++it ) {
        KoTabulator *t = new KoTabulator;
        t->type = it.current()->type;
        t->mmPos = it.current()->mmPos;
        t->ptPos = it.current()->ptPos;
        t->inchPos = it.current()->inchPos;
        tabList.append( t );
        if ( t->type != T_LEFT ) specialTabs = true;
    }
}

/*================================================================*/
bool KWParagLayout::getNextTab( unsigned int _ptPos, unsigned int _lBorder, unsigned int _rBorder,
                                unsigned int &_tabPos, KoTabulators &_tabType )
{
    _tabPos = 0;
    _tabType = T_LEFT;

    if ( tabList.isEmpty() ) return false;

    int _mostLeft = -1, _best = -1;
    unsigned int ptPos = 0;

    for ( unsigned int i = 0; i < tabList.count(); i++ ) {
        ptPos = static_cast<int>(tabList.at( i )->ptPos) + _lBorder;
        if ( ptPos > _ptPos && ptPos < _rBorder && ( _best == -1 ||
                                                     ptPos < static_cast<unsigned int>( tabList.at( _best )->ptPos ) ) )
            _best = i;
        if ( ptPos <= _ptPos && ptPos > _lBorder && ( _mostLeft == -1 ||
                                                      ptPos < static_cast<unsigned int>( tabList.at( _mostLeft )->ptPos ) ) )
            _mostLeft = i;
    }

    if ( _best != -1 ) {
        _tabPos = static_cast<int>(tabList.at( _best )->ptPos) + _lBorder;
        _tabType = tabList.at( _best )->type;
        return true;
    }

    if ( _mostLeft != -1 ) {
        _tabPos = static_cast<int>(tabList.at( _mostLeft )->ptPos) + _lBorder;
        _tabType = tabList.at( _mostLeft )->type;
        return true;
    }

    return false;
}
