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
    left.style = Border::SOLID;
    left.ptWidth = 0;
    right.color = Qt::white;
    right.style = Border::SOLID;
    right.ptWidth = 0;
    top.color = Qt::white;
    top.style = Border::SOLID;
    top.ptWidth = 0;
    bottom.color = Qt::white;
    bottom.style = Border::SOLID;
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
    out << indent << "<NAME value=\"" << correctQString( name ) << "\"/>" << endl;
    out << indent << "<FOLLOWING name=\"" << correctQString( followingParagLayout ) << "\"/>" << endl;
    if(flow!=0) {
        out << indent << "<FLOW value=\"" << static_cast<int>( flow ) << "\"/>" << endl;
    }
    if(paragHeadOffset.pt()!=0) {
        out << indent << "<OHEAD " << paragHeadOffset << "/>" << endl;
    }
    if(paragFootOffset.pt()!=0) {
        out << indent << "<OFOOT " << paragFootOffset << "/>" << endl;
    }
    if(firstLineLeftIndent.pt()!=0) {
        out << indent << "<IFIRST " << firstLineLeftIndent << "/>" << endl;
    }
    if(leftIndent.pt()!=0) {
        out << indent << "<ILEFT " << leftIndent << "/>" << endl;
    }
    if(lineSpacing.pt()!=0) {
        out << indent << "<LINESPACE " << lineSpacing << "/>" << endl;
    }
    if(counter.counterType!=CT_NONE) {
        out << indent << "<COUNTER type=\"" << static_cast<int>( counter.counterType ) << "\" depth=\"" << counter.counterDepth
            << "\" bullet=\"" << counter.counterBullet.unicode() << "\" start=\""
            << QString::number(counter.startCounter) << "\" numberingtype=\""
            << static_cast<int>( counter.numberingType ) << "\" lefttext=\""
            << correctQString( counter.counterLeftText ) << "\" righttext=\""
            << correctQString( counter.counterRightText ) << "\" bulletfont=\""
            << correctQString( counter.bulletFont ) << "\" customdef=\""
            << correctQString( counter.customCounterDef ) << "\" />" << endl;
    }
    if(left.color.red()!=255 || left.color.green()!=255 || left.color.blue()!=255 || 
            left.style!=0 || left.ptWidth!=0) {
        out << indent << "<LEFTBORDER ";
        if(left.color.red()!=255 || left.color.green()!=255 || left.color.blue()!=255) {
            out << "red=\"" << left.color.red() << "\" green=\"" << left.color.green() << "\" blue=\""
                << left.color.blue() << "\" ";
        }
        if(left.style!=0 || left.ptWidth!=0) {
            out << "style=\"" << static_cast<int>( left.style ) << "\" width=\"" << left.ptWidth << "\"";
        }
        out << "/>" << endl;
    }
    if(right.color.red()!=255 || right.color.green()!=255 || right.color.blue()!=255 || 
            right.style!=0 || right.ptWidth!=0) {
        out << indent << "<RIGHTBORDER ";
        if(right.color.red()!=255 || right.color.green()!=255 || right.color.blue()!=255) {
            out << "red=\"" << right.color.red() << "\" green=\"" << right.color.green() << "\" blue=\""
                << right.color.blue() << "\" ";
        }
        if(right.style!=0 || right.ptWidth!=0) {
            out << "style=\"" << static_cast<int>( right.style ) << "\" width=\"" << right.ptWidth << "\"";
        }
        out << "/>" << endl;
    }
    if(top.color.red()!=255 || top.color.green()!=255 || top.color.blue()!=255 || 
            top.style!=0 || top.ptWidth!=0) {
        out << indent << "<TOPBORDER ";
        if(top.color.red()!=255 || top.color.green()!=255 || top.color.blue()!=255) {
            out << "red=\"" << top.color.red() << "\" green=\"" << top.color.green() << "\" blue=\""
                << top.color.blue() << "\" ";
        }
        if(top.style!=0 || top.ptWidth!=0) {
            out << "style=\"" << static_cast<int>( top.style ) << "\" width=\"" << top.ptWidth << "\"";
        }
        out << "/>" << endl;
    }
    if(bottom.color.red()!=255 || bottom.color.green()!=255 || bottom.color.blue()!=255 || 
            bottom.style!=0 || bottom.ptWidth!=0) {
        out << indent << "<BOTTOMBORDER ";
        if(bottom.color.red()!=255 || bottom.color.green()!=255 || bottom.color.blue()!=255) {
            out << "red=\"" << bottom.color.red() << "\" green=\"" << bottom.color.green() << "\" blue=\""
                << bottom.color.blue() << "\" ";
        }
        if(bottom.style!=0 || bottom.ptWidth!=0) {
            out << "style=\"" << static_cast<int>( bottom.style ) << "\" width=\"" << bottom.ptWidth << "\"";
        }
        out << "/>" << endl;
    }
    out << otag << "<FORMAT>" << endl;
    format.save( out );
    out << etag << "</FORMAT> " << endl;

    for ( unsigned int i = 0; i < tabList.count(); i++ ){
        out << indent << "<TABULATOR mmpos=\"" << tabList.at( i )->mmPos << "\" ptpos=\"" 
            << tabList.at( i )->ptPos << "\" inchpos=\"" << tabList.at( i )->inchPos << "\"";
        if(tabList.at(i)->type!=T_LEFT) {
            out << "type=\"" << static_cast<int>( tabList.at( i )->type ) << "\"";
        }
        out << "/>" << endl;
    }
}

/*================================================================*/
void KWParagLayout::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString _name;
    double pt, mm, inch;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, _name, lst );

        // name
        if ( _name == "NAME" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    name = correctQString( ( *it ).m_strValue );
            }
        } else if ( _name == "FOLLOWING" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "name" )
                    followingParagLayout = correctQString( ( *it ).m_strValue );
            }
        } else if ( _name == "TABULATOR" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            KoTabulator *tab = new KoTabulator;
            bool noinch = true;
            tab->type=T_LEFT;
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "mmpos" )
                    tab->mmPos = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "ptpos" )
                    tab->ptPos = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inchpos" ) {
                    noinch = false;
                    tab->inchPos = ( *it ).m_strValue.toDouble();
                }
                if ( ( *it ).m_strName == "type" )
                    tab->type = static_cast<KoTabulators>( ( *it ).m_strValue.toInt() );
            }
            if ( noinch ) tab->inchPos = MM_TO_INCH( tab->mmPos );
            tabList.append( tab );
        } else if ( _name == "FLOW" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    flow = static_cast<Flow>( ( *it ).m_strValue.toInt() );
            }
        } else if ( _name == "OHEAD" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "mm" )
                    mm = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inch" )
                    inch = ( *it ).m_strValue.toDouble();
            }
            paragHeadOffset.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "OFOOT" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "mm" )
                    mm = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inch" )
                    inch = ( *it ).m_strValue.toDouble();
            }
            paragFootOffset.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "IFIRST" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "mm" )
                    mm = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inch" )
                    inch = ( *it ).m_strValue.toDouble();
            }
            firstLineLeftIndent.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "ILEFT" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "mm" )
                    mm = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inch" )
                    inch = ( *it ).m_strValue.toDouble();
            }
            leftIndent.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "LINESPACE" ) {
            pt = mm = inch = 0.0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "pt" )
                    pt = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "mm" )
                    mm = ( *it ).m_strValue.toDouble();
                if ( ( *it ).m_strName == "inch" )
                    inch = ( *it ).m_strValue.toDouble();
            }
            lineSpacing.setPT_MM_INCH( pt, mm, inch );
        } else if ( _name == "OFFSETS" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "head" )
                    paragHeadOffset.setMM( ( *it ).m_strValue.toDouble() );
                else if ( ( *it ).m_strName == "foot" )
                    paragFootOffset.setMM( ( *it ).m_strValue.toDouble() );
            }
        } else if ( _name == "INDENTS" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "first" )
                    firstLineLeftIndent.setMM( ( *it ).m_strValue.toDouble() );
                else if ( ( *it ).m_strName == "left" )
                    leftIndent.setMM( ( *it ).m_strValue.toDouble() );
            }
        } else if ( _name == "LINESPACING" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    lineSpacing.setPT( ( *it ).m_strValue.toDouble() );
            }
        } else if ( _name == "COUNTER" ) {
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "type" )
                    counter.counterType = static_cast<CounterType>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "depth" )
                    counter.counterDepth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bullet" )
                    counter.counterBullet = QChar( static_cast<unsigned int>( ( *it ).m_strValue.toInt() ) );
                else if ( ( *it ).m_strName == "lefttext" )
                    counter.counterLeftText = correctQString( ( *it ).m_strValue );
                else if ( ( *it ).m_strName == "righttext" )
                    counter.counterRightText = correctQString( ( *it ).m_strValue );
                else if ( ( *it ).m_strName == "start" )
                {
                    QString s = ( *it ).m_strValue;
                    if ( s[0].isDigit() )
                      counter.startCounter = s.toInt();
                    else // support for old files (DF)
                      counter.startCounter = s.lower()[0].latin1() - 'a' + 1;
                }
                else if ( ( *it ).m_strName == "numberingtype" )
                    counter.numberingType = static_cast<NumType>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bulletfont" )
                    counter.bulletFont = correctQString( ( *it ).m_strValue );
                else if ( ( *it ).m_strName == "customdef" )
                    counter.customCounterDef = correctQString( ( *it ).m_strValue );
            }
        } else if ( _name == "LEFTBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = ( *it ).m_strValue.toInt();
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = ( *it ).m_strValue.toInt();
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = ( *it ).m_strValue.toInt();
                    left.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    left.style = static_cast<Border::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "width" )
                    left.ptWidth = ( *it ).m_strValue.toInt();
            }
        } else if ( _name == "RIGHTBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = ( *it ).m_strValue.toInt();
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = ( *it ).m_strValue.toInt();
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = ( *it ).m_strValue.toInt();
                    right.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    right.style = static_cast<Border::BorderStyle>(( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "width" )
                    right.ptWidth = ( *it ).m_strValue.toInt();
            }
        } else if ( _name == "BOTTOMBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = ( *it ).m_strValue.toInt();
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = ( *it ).m_strValue.toInt();
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = ( *it ).m_strValue.toInt();
                    bottom.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    bottom.style = static_cast<Border::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "width" )
                    bottom.ptWidth = ( *it ).m_strValue.toInt();
            }
        } else if ( _name == "TOPBORDER" ) {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag, _name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" ) {
                    r = ( *it ).m_strValue.toInt();
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "green" ) {
                    g = ( *it ).m_strValue.toInt();
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "blue" ) {
                    b = ( *it ).m_strValue.toInt();
                    top.color.setRgb( r, g, b );
                } else if ( ( *it ).m_strName == "style" )
                    top.style = static_cast<Border::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "width" )
                    top.ptWidth = ( *it ).m_strValue.toInt();
            }
        } else if ( _name == "FORMAT" ) {
            parser.parseTag( tag, _name, lst );
            //vector<KOMLAttrib>::const_iterator it = lst.begin();
            //for( ; it != lst.end(); it++ ) {
            //}
            format.load( parser, lst, document );
        } else
            kdError(32001) << "Unknown tag '" << tag << "' in PARAGRAPHLAYOUT" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
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
        ptPos = static_cast<int>(tabList.at( i )->ptPos * document->getZoom()/100) + _lBorder;
        if ( ptPos > _ptPos && ptPos < _rBorder && ( _best == -1 ||
                                                     ptPos < static_cast<unsigned int>( tabList.at( _best )->ptPos * document->getZoom()/100) ) )
            _best = i;
        if ( ptPos <= _ptPos && ptPos > _lBorder && ( _mostLeft == -1 ||
                                                      ptPos < static_cast<unsigned int>( tabList.at( _mostLeft )->ptPos * document->getZoom()/100) ) )
            _mostLeft = i;
    }

    if ( _best != -1 ) {
        _tabPos = static_cast<int>(tabList.at( _best )->ptPos * document->getZoom()) / 100
                  + _lBorder;
        _tabType = tabList.at( _best )->type;
        return true;
    }

    if ( _mostLeft != -1 ) {
        _tabPos = static_cast<int>(tabList.at( _mostLeft )->ptPos * document->getZoom()) / 100
                  + _lBorder;
        _tabType = tabList.at( _mostLeft )->type;
        return true;
    }

    return false;
}
