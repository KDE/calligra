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

#include "koVariable.h"
#include "koVariable.moc"
#include <koDocumentInfo.h>
#include <kozoomhandler.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <qdom.h>
#include <koDocument.h>
#include <kdialogbase.h>
#include <kconfig.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qvaluelist.h>
#include <qradiobutton.h>
#include "timeformatwidget_impl.h"
#include "dateformatwidget_impl.h"

KoVariableSettings::KoVariableSettings()
{
    m_startingpage = 1;
    m_displayLink = true;
    m_displayComment = true;
    m_underlineLink = true;
    m_displayFieldCode = false;
}

void KoVariableSettings::save( QDomElement &parentElem )
{
     QDomElement elem = parentElem.ownerDocument().createElement( "VARIABLESETTINGS" );
     parentElem.appendChild( elem );
    if(m_startingpage!=1)
    {
        elem.setAttribute( "startingPageNumber", m_startingpage );
    }
    elem.setAttribute("displaylink",(int)m_displayLink);
    elem.setAttribute("underlinelink",(int)m_underlineLink);
    elem.setAttribute("displaycomment",(int)m_displayComment);
    elem.setAttribute("displayfieldcode", (int)m_displayFieldCode);
}

void KoVariableSettings::load( QDomElement &elem )
{
    QDomElement e = elem.namedItem( "VARIABLESETTINGS" ).toElement();
    if (!e.isNull())
    {
        if(e.hasAttribute("startingPageNumber"))
            m_startingpage = e.attribute("startingPageNumber").toInt();
        if(e.hasAttribute("displaylink"))
            m_displayLink=(bool)e.attribute("displaylink").toInt();
        if(e.hasAttribute("underlinelink"))
            m_underlineLink=(bool)e.attribute("underlinelink").toInt();
        if(e.hasAttribute("displaycomment"))
            m_displayComment=(bool)e.attribute("displaycomment").toInt();
        if (e.hasAttribute("displayfieldcode"))
            m_displayFieldCode=(bool)e.attribute("displayfieldcode").toInt();
    }
}

KoVariableDateFormat::KoVariableDateFormat() : KoVariableFormat()
{
    m_bShort = false;
    m_strFormat="";
}

QString KoVariableDateFormat::convert( const QVariant& data ) const
{
    if ( data.type() != QVariant::Date )
    {
        kdDebug(32500)<<" Error in KoVariableDateFormat::convert. Value is a " << data.typeName() << endl;
        return QString::null;
    }
    if(m_strFormat.lower()==QString("locale")||m_strFormat.isEmpty())  // FIXME: "Locale" is I18N !
	return KGlobal::locale()->formatDate( data.toDate(),m_bShort );
    return data.toDate().toString(m_strFormat);
}

QCString KoVariableDateFormat::key() const
{
    return QCString("DATE") + (m_bShort ? '1' : '0')+m_strFormat.utf8();
}

void KoVariableDateFormat::load( const QCString &key )
{
    QCString params( key.mid( 4 ) );
    if ( !params.isEmpty() )
    {
        m_bShort = (params[0] == '1');
        m_strFormat = QString::fromUtf8( key.mid( 5 ) ); // skip "DATE" and the 0/1
    }
    // TODO else: use the last setting ?  (useful for the interactive case)
}

KoVariableTimeFormat::KoVariableTimeFormat():KoVariableFormat(){
    m_strFormat="";
}

void KoVariableTimeFormat::load( const QCString &key )
{
    QCString params( key.mid( 4 ) );
    if ( !params.isEmpty() )
	m_strFormat = QString::fromUtf8(params);
}

QString KoVariableTimeFormat::convert( const QVariant & time ) const
{
    if ( time.type() != QVariant::Time )
    {
        kdDebug(32500)<<" Error in KoVariableTimeFormat::convert. Value is a " << time.typeName() << endl;
        return QString::null;
    }

    if(m_strFormat.lower()==QString("locale")||m_strFormat.isEmpty())   // FIXME: "Locale" is I18N !
	return KGlobal::locale()->formatTime( time.toTime() );
    return time.toTime().toString(m_strFormat);
}

QCString KoVariableTimeFormat::key() const
{
    return QCString("TIME")+m_strFormat.utf8();
}

QString KoVariableStringFormat::convert( const QVariant & string ) const
{
    if ( string.type() != QVariant::String )
    {
        kdDebug(32500)<<" Error in KoVariableStringFormat::convert. Value is a " << string.typeName() << endl;
        return QString::null;
    }

    return string.toString();
}

QCString KoVariableStringFormat::key() const
{
    return "STRING";
    // TODO prefix & suffix
}

QString KoVariableNumberFormat::convert( const QVariant &value ) const
{
    if ( value.type() != QVariant::Int )
    {
        kdDebug(32500)<<" Error in KoVariableNumberFormat::convert. Value is a " << value.typeName() << endl;
        return QString::null;
    }

    return QString::number( value.toInt() );
}

QCString KoVariableNumberFormat::key() const
{
    return "NUMBER";
}

/* for the prefix+suffix string format
    QString str;
    str.prepend( pre );
    str.append( post );
    return QString( str );
*/


KoVariableFormatCollection::KoVariableFormatCollection()
{
    m_dict.setAutoDelete( true );
}

KoVariableFormat * KoVariableFormatCollection::format( const QCString &key )
{
    KoVariableFormat *f = m_dict[ key.data() ];
    if (f)
        return f;
    else
        return createFormat( key );
}

KoVariableFormat * KoVariableFormatCollection::createFormat( const QCString &key )
{
    KoVariableFormat * format = 0L;
    // The first 4 chars identify the class
    QCString type = key.left(4);
    if ( type == "DATE" )
        format = new KoVariableDateFormat();
    else if ( type == "TIME" )
        format = new KoVariableTimeFormat();
    else if ( type == "NUMB" ) // this type of programming makes me numb ;)
        format = new KoVariableNumberFormat();
    else if ( type == "STRI" )
        format = new KoVariableStringFormat();

    if ( format )
    {
        format->load( key );
        m_dict.insert( format->key() /* not 'key', it could be incomplete */, format );
    }
    return format;
}

/******************************************************************/
/* Class:       KoVariableCollection                              */
/******************************************************************/
KoVariableCollection::KoVariableCollection(KoVariableSettings *_setting)
{
    m_variableSettings=_setting;
    m_varSelected=0L;
}

KoVariableCollection::~KoVariableCollection()
{
    delete m_variableSettings;
}

void KoVariableCollection::registerVariable( KoVariable *var )
{
    if ( !var )
        return;
    variables.append( var );
}

void KoVariableCollection::unregisterVariable( KoVariable *var )
{
    variables.take( variables.findRef( var ) );
}

void KoVariableCollection::recalcVariables(int type)
{
    bool update = false;
    QPtrListIterator<KoVariable> it( variables );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == type || type == VT_ALL )
        {
            update = true;
            it.current()->recalc();
            KoTextParag * parag = it.current()->paragraph();
            if ( parag )
            {
                //kdDebug(32500) << "KoDoc::recalcVariables -> invalidating parag " << parag->paragId() << endl;
                parag->invalidate( 0 );
                parag->setChanged( true );
            }
        }
    }
    if(update)
        emit repaintVariable();
}


void KoVariableCollection::setVariableValue( const QString &name, const QString &value )
{
    varValues[ name ] = value;
}

QString KoVariableCollection::getVariableValue( const QString &name ) const
{
    if ( !varValues.contains( name ) )
        return i18n( "No value" );
    return varValues[ name ];
}

bool KoVariableCollection::customVariableExist(const QString &varname) const
{
    return varValues.contains( varname );
}

void KoVariableCollection::changeTypeOfVariable()
{
    KAction * act = (KAction *)(sender());
    VariableSubTextMap::Iterator it = m_variableSubTextMap.find( act );
    if ( it == m_variableSubTextMap.end() )
        kdWarning() << "Action not found in m_variableSubTextMap." << endl;
    else
    {
        if( m_varSelected )
        {
            m_varSelected->setVariableSubType( m_varSelected->variableSubType(*it) );
            recalcVariables(m_varSelected);
        }
    }
}

void KoVariableCollection::recalcVariables(KoVariable *var)
{
    if( var )
    {
        var->recalc();
        KoTextParag * parag = var->paragraph();
        if ( parag )
        {
            parag->invalidate( 0 );
            parag->setChanged( true );
        }
        emit repaintVariable();
    }
}

void KoVariableCollection::changeFormatOfVariable()
{
    KAction * act = (KAction *)(sender());
    VariableSubFormatMap::Iterator it = m_variableSubFormatMap.find( act );
    if ( it == m_variableSubFormatMap.end() )
        kdWarning() << "Action not found in m_variableSubTextMap." << endl;
    else
    {
        if( m_varSelected )
        {
            KoDateVariable *date=dynamic_cast<KoDateVariable*>(m_varSelected);
            if(date)
            {
                static_cast<KoVariableDateFormat*>(date->variableFormat())->m_strFormat=(*it).format;
            }
            KoTimeVariable *time=dynamic_cast<KoTimeVariable*>(m_varSelected);
            if(time)
            {
                static_cast<KoVariableTimeFormat*>(time->variableFormat())->m_strFormat=(*it).format;
            }
            recalcVariables(m_varSelected);
        }
    }
 }

void KoVariableCollection::setVariableSelected(KoVariable * var)
{
    m_varSelected=var;
}

QPtrList<KAction> KoVariableCollection::variableActionList()
{
    QPtrList<KAction> listAction=QPtrList<KAction>();
    if( !m_varSelected)
        return listAction;
    else
    {
        QStringList list=m_varSelected->subTypeText();
        QStringList::ConstIterator it = list.begin();
        for ( int i = 0; it != list.end() ; ++it, ++i )
        {
            if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
            {
                KAction * act = new KAction( (*it));
                connect( act, SIGNAL(activated()),this, SLOT(changeTypeOfVariable()) );

                m_variableSubTextMap.insert( act, i );
                listAction.append( act );
            }
        }
        if(m_varSelected->type() == VT_DATE || m_varSelected->type() == VT_TIME )
        {
            list=m_varSelected->subTypeFormat();
            it = list.begin();
            for ( int i = 0; it != list.end() ; ++it, ++i )
            {
                if( i == 0)
                    listAction.append( new KActionSeparator() );

                if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
                {
                    VariableSubFormatDef v;
                    switch(m_varSelected->type())
                    {
                    case VT_DATE:
                    {
                        QDate ct=QDate::currentDate();
                        if((*it)==i18n("Locale").lower())
                            v.translatedString=KGlobal::locale()->formatDate( ct );
                        else
                            v.translatedString=ct.toString(*it);
                        v.format=*it;
                        break;
                    }
                    case  VT_TIME:
                    {
                        QTime ct=QTime::currentTime();
                        if((*it)==i18n("Locale").lower())
                            v.translatedString=KGlobal::locale()->formatTime( ct );
                        else
                            v.translatedString=ct.toString(*it);
                        v.format=*it;
                        break;
                    }
                    default:
                        break;
                    }
                    KAction * act = new KAction(v.translatedString);
                    connect( act, SIGNAL(activated()),this, SLOT(changeFormatOfVariable()) );
                    m_variableSubFormatMap.insert( act, v );
                    listAction.append( act );
                }
            }
        }
    }

    return listAction;
}



/******************************************************************/
/* Class: KoVariable                                              */
/******************************************************************/
KoVariable::KoVariable( KoTextDocument *textdoc, KoVariableFormat *varFormat, KoVariableCollection *_varColl)
    : KoTextCustomItem( textdoc )
{
    m_varColl=_varColl;
    m_varFormat = varFormat;
    m_varColl->registerVariable( this );
}

KoVariable::~KoVariable()
{
    //kdDebug(32500) << "KoVariable::~KoVariable " << this << endl;
    m_varColl->unregisterVariable( this );
}

QStringList KoVariable::subTypeText()
{
    return QStringList();
}

QStringList KoVariable::subTypeFormat()
{
    return QStringList();
}

void KoVariable::resize()
{
    if ( m_deleted )
        return;
    KoTextFormat *fmt = format();
    QFontMetrics fm = fmt->screenFontMetrics( 0, false );
    QString txt = text();
    width = 0;
    for ( int i = 0 ; i < (int)txt.length() ; ++i )
        width += fm.charWidth( txt, i ); // size at 100%
    // zoom to LU
    width = qRound( KoTextZoomHandler::ptToLayoutUnitPt( width ) );
    height = fmt->height();
    //kdDebug(32500) << "Before KoVariable::resize text=" << txt << " width=" << width << endl;
}

QString KoVariable::fieldCode()
{
    return i18n("Variable");
}

QString KoVariable::text()
{
    if (m_varColl->variableSetting()->displayFiedCode())
        return fieldCode();
    else
        return m_varFormat->convert( m_varValue );
}

void KoVariable::drawCustomItem( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, const int offset )
{
    KoTextFormat * fmt = format();
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    QFont font( fmt->screenFont( zh ) );
    drawCustomItemHelper( p, x, y, cg, selected, offset, fmt, font, fmt->color() );
}

void KoVariable::drawCustomItemHelper( QPainter* p, int x, int y, const QColorGroup& cg, bool selected, const int offset, KoTextFormat* fmt, const QFont& font, QColor textColor )
{
    int bl, _y;
    KoTextParag * parag = paragraph();
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    //kdDebug(32500) << "KoVariable::draw index=" << index() << " x=" << x << " y=" << y << endl;
    int h = parag->lineHeightOfChar( index(), &bl, &_y /*unused*/);

    h = zh->layoutUnitToPixelY( y, h );
    bl = zh->layoutUnitToPixelY( y, bl );

    p->save();

    if ( fmt->textBackgroundColor().isValid() )
        p->fillRect( x, y, zh->layoutUnitToPixelX( width ), h, fmt->textBackgroundColor() );

    if ( textDocument()->drawingShadow() ) // Use shadow color if drawing a shadow
    {
        textColor = parag->shadowColor();
        p->setPen( textColor );
    }
    else if ( selected )
    {
        textColor = cg.color( QColorGroup::HighlightedText );
        p->setPen( QPen( textColor ) );
        p->fillRect( x, y,  zh->layoutUnitToPixelX( width ), h, cg.color( QColorGroup::Highlight ) );
    }
    else if ( textDocument() && textDocument()->drawFormattingChars()
              && p->device()->devType() != QInternal::Printer )
    {
        textColor = cg.color( QColorGroup::Highlight );
        p->setPen( QPen ( textColor, 0, Qt::DotLine ) );
        p->drawRect( x, y, zh->layoutUnitToPixelX( width ), h );
    }
    else {
        if ( !textColor.isValid() ) // Resolve the color at this point
            textColor = KoTextFormat::defaultTextColor( p );
        p->setPen( QPen( textColor ) );
    }

    p->setFont( font ); // already done by KoTextCustomItem::draw but someone might
                        // change the font passed to drawCustomItemHelper (e.g. KoLinkVariable)

    KoTextParag::drawFontEffects( p, fmt, zh, font, textColor, x, bl, zh->layoutUnitToPixelX( width ), y, h );

    //kdDebug(32500) << "KoVariable::draw bl=" << bl << << endl;
    p->drawText( x, y + bl + offset, text() );
    p->restore();
}

void KoVariable::save( QDomElement &parentElem )
{
    //kdDebug(32500) << "KoVariable::save" << endl;
    QDomElement variableElem = parentElem.ownerDocument().createElement( "VARIABLE" );
    parentElem.appendChild( variableElem );
    QDomElement typeElem = parentElem.ownerDocument().createElement( "TYPE" );
    variableElem.appendChild( typeElem );
    typeElem.setAttribute( "type", static_cast<int>( type() ) );
    typeElem.setAttribute( "key", m_varFormat->key() );
    typeElem.setAttribute( "text", text() );
    saveVariable( variableElem );
}

void KoVariable::load( QDomElement & )
{
}

KoVariable * KoVariableCollection::createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc, bool _forceDefaultFormat )
{
    QCString string;
    QStringList stringList;
    if ( varFormat == 0L )
    {
        // Get the default format for this variable (this method is only called in the interactive case, not when loading)
        switch ( type ) {
        case VT_DATE:
        case VT_DATE_VAR_KWORD10:  // compatibility with kword 1.0
        {
            if ( _forceDefaultFormat )
                varFormat = coll->format( KoDateVariable::defaultFormat() );
            else
                varFormat = coll->format( KoDateVariable::formatStr() );
        }
        case VT_TIME:
        case VT_TIME_VAR_KWORD10:  // compatibility with kword 1.0
        {
            if ( _forceDefaultFormat )
                varFormat = coll->format( KoTimeVariable::defaultFormat() );
            else
                varFormat = coll->format( KoTimeVariable::formatStr() );
        }
        case VT_PGNUM:
            varFormat = coll->format( "NUMBER" );
            break;
        case VT_FIELD:
        case VT_CUSTOM:
        case VT_MAILMERGE:
        case VT_LINK:
        case VT_NOTE:
            varFormat = coll->format( "STRING" );
            break;
        case VT_FOOTNOTE: // this is a KWord-specific variable
            kdError() << "Footnote type not handled in KoVariableCollection: VT_FOOTNOTE" << endl;
            return 0L;
        }
    }
    Q_ASSERT( varFormat );
    if ( varFormat == 0L ) // still 0 ? Impossible!
        return 0L ;

    KoVariable * var = 0L;
    switch ( type ) {
        case VT_DATE:
        case VT_DATE_VAR_KWORD10:  // compatibility with kword 1.0
            var = new KoDateVariable( textdoc, subtype, varFormat,this );
            break;
        case VT_TIME:
        case VT_TIME_VAR_KWORD10:  // compatibility with kword 1.0
            var = new KoTimeVariable( textdoc, subtype, varFormat, this );
            break;
        case VT_PGNUM:
            kdError() << "VT_PGNUM must be handled by the application's reimplementation of KoVariableCollection::createVariable" << endl;
            //var = new KoPgNumVariable( textdoc, subtype, varFormat, this );
            break;
        case VT_FIELD:
            var = new KoFieldVariable( textdoc, subtype, varFormat,this,doc );
            break;
        case VT_CUSTOM:
            var = new KoCustomVariable( textdoc, QString::null, varFormat, this);
            break;
        case VT_MAILMERGE:
            var = new KoMailMergeVariable( textdoc, QString::null, varFormat ,this);
            break;
        case VT_LINK:
            var = new KoLinkVariable( textdoc,QString::null, QString::null, varFormat ,this);
            break;
        case VT_NOTE:
            var = new KoNoteVariable( textdoc, QString::null, varFormat ,this);
            break;
    }
    Q_ASSERT( var );
    return var;
}

void KoVariable::setVariableFormat( KoVariableFormat *_varFormat )
{
    m_varFormat=_varFormat;
}


/******************************************************************/
/* Class: KoDateVariable                                          */
/******************************************************************/
KoDateVariable::KoDateVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *_varFormat, KoVariableCollection *_varColl)
    : KoVariable( textdoc, _varFormat,_varColl ), m_subtype( subtype )
{
}

QString KoDateVariable::fieldCode()
{
    return (m_subtype == VST_DATE_FIX)?i18n("Date (Fixed)"):i18n("Date");
}

void KoDateVariable::recalc()
{
    if ( m_subtype == VST_DATE_CURRENT )
        m_varValue = QVariant(QDate::currentDate());
    else
    {
        // Only if never set before (i.e. upon insertion)
        if ( m_varValue.toDate().isNull() )
            m_varValue= QVariant(QDate::currentDate());
    }
    resize();
}

void KoDateVariable::saveVariable( QDomElement& varElem )
{
    QDomElement elem = varElem.ownerDocument().createElement( "DATE" );
    varElem.appendChild( elem );
    QDate date = m_varValue.toDate();
    elem.setAttribute( "year", date.year() );
    elem.setAttribute( "month", date.month() );
    elem.setAttribute( "day", date.day() );
    elem.setAttribute( "fix", m_subtype == VST_DATE_FIX ); // to be extended
}

void KoDateVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );

    QDomElement e = elem.namedItem( "DATE" ).toElement();
    if (!e.isNull())
    {
        int y = e.attribute("year").toInt();
        int m = e.attribute("month").toInt();
        int d = e.attribute("day").toInt();
        bool fix = e.attribute("fix").toInt() == 1;
        if ( fix )
        {
            QDate date;
            date.setYMD( y, m, d );
            m_varValue = QVariant( date );
        }
        m_subtype = fix ? VST_DATE_FIX : VST_DATE_CURRENT;
    }
}

QStringList KoDateVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Current Date (fixed)" );
    lst << i18n( "Current Date (variable)" );
    // TODO add date created, date printed, date last modified( BR #24242 )
    return lst;
}

QStringList KoDateVariable::subTypeText()
{
    return KoDateVariable::actionTexts();
}

QStringList KoDateVariable::subTypeFormat()
{
    QStringList listDateFormat;
    listDateFormat<<i18n("locale");
    listDateFormat<<i18n("dd/MM/yy");
    listDateFormat<<i18n("dd/MM/yyyy");
    listDateFormat<<i18n("MMM dd,yy");
    listDateFormat<<i18n("MMM dd,yyyy");
    listDateFormat<<i18n("dd.MMM.yyyy");
    listDateFormat<<i18n("MMMM dd, yyyy");
    listDateFormat<<i18n("ddd, MMM dd,yy");
    listDateFormat<<i18n("dddd, MMM dd,yy");
    listDateFormat<<i18n("MM-dd");
    listDateFormat<<i18n("yyyy-MM-dd");
    listDateFormat<<i18n("dd/yy");
    listDateFormat<<i18n("MMMM");
    return listDateFormat;
}

QCString KoDateVariable::defaultFormat()
{
    return QCString(QCString("DATE")
                    + QCString("0") // no support for short locale dates yet - TODO
                    + QCString("locale"));
}

QCString KoDateVariable::formatStr()
{
    QCString string;
    QStringList stringList;
    KDialogBase* dialog=new KDialogBase(0, 0, true, i18n("Date Format"), KDialogBase::Ok|KDialogBase::Cancel);
    DateFormatWidget* widget=new DateFormatWidget(dialog);
    int count=0;
    dialog->setMainWidget(widget);
    KConfig config( "kofficerc" );
    bool selectLast=false;
    if( config.hasGroup("Date format history") )
    {
        config.setGroup("Date format history");
        int noe=config.readNumEntry("Number Of Entries", 5);
        for(int i=0;i<noe;i++)
        {
            QString num, tmpString;
            num.setNum(i);
            tmpString=config.readEntry("Last Used"+num);
            if(tmpString.compare(i18n("Locale"))==0)
            {
                if(i==0) selectLast = true;
                continue;
            }
            if(stringList.contains(tmpString))
                continue;
            if(!tmpString.isEmpty())
            {
                stringList.append(tmpString);
                count++;
            }
        }

    }
    if(!stringList.isEmpty())
    {
        widget->combo1->insertItem("---");
        widget->combo1->insertStringList(stringList);
    }
    if(selectLast) {
        QComboBox *combo= widget->combo1;
        combo->setCurrentItem(combo->count() -1);
        widget->updateLabel();
    }

    if(dialog->exec()==QDialog::Accepted)
    {
        if ( widget->resultString() == i18n("Locale") )
            string = "locale"; // untranslated form
        else
            string=widget->resultString().utf8();
    }
    else
    {
        return 0;
    }
    config.setGroup("Date format history");
    stringList.remove(string);
    stringList.prepend(string);
    for(int i=0;i<=count;i++)
    {
        QString num;
        num.setNum(i);
        config.writeEntry("Last Used"+num, stringList[i]);
    }
    config.sync();
    delete dialog;
    return QCString(QCString("DATE") + QCString("0") + // no support for short locale dates yet - TODO
                    string );
}

/******************************************************************/
/* Class: KoTimeVariable                                          */
/******************************************************************/
KoTimeVariable::KoTimeVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat, KoVariableCollection *_varColl)
    : KoVariable( textdoc, varFormat,_varColl ), m_subtype( subtype )
{
}

QString KoTimeVariable::fieldCode()
{
    return (m_subtype == VST_TIME_FIX)?i18n("Time (Fixed)"):i18n("Time");
}


void KoTimeVariable::recalc()
{
    if ( m_subtype == VST_TIME_CURRENT )
        m_varValue = QVariant( QTime::currentTime());
    else
    {
        // Only if never set before (i.e. upon insertion)
        if ( m_varValue.toTime().isNull() )
            m_varValue = QVariant( QTime::currentTime());
    }
    resize();
}


void KoTimeVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement elem = parentElem.ownerDocument().createElement( "TIME" );
    parentElem.appendChild( elem );
    QTime time = m_varValue.toTime();
    elem.setAttribute( "hour", time.hour() );
    elem.setAttribute( "minute", time.minute() );
    elem.setAttribute( "second", time.second() );
//    elem.setAttribute( "msecond", m_time.msec() );
    elem.setAttribute( "fix", m_subtype == VST_TIME_FIX );
}

void KoTimeVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );

    QDomElement e = elem.namedItem( "TIME" ).toElement();
    if (!e.isNull())
    {
        int h = e.attribute("hour").toInt();
        int m = e.attribute("minute").toInt();
        int s = e.attribute("second").toInt();
        int ms = e.attribute("msecond").toInt();
        bool fix = static_cast<bool>( e.attribute("fix").toInt() );
        if ( fix )
        {
            QTime time;
            time.setHMS( h, m, s, ms );
            m_varValue = QVariant( time);
        }
        m_subtype = fix ? VST_TIME_FIX : VST_TIME_CURRENT;
    }
}

QStringList KoTimeVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Current Time (fixed)" );
    lst << i18n( "Current Time (variable)" );
    // TODO add time created, time printed, time last modified( BR #24242 )
    return lst;
}

QStringList KoTimeVariable::subTypeText()
{
    return KoTimeVariable::actionTexts();
}

QStringList KoTimeVariable::subTypeFormat()
{
    QStringList listTimeFormat;
    listTimeFormat<<i18n("locale");
    listTimeFormat<<i18n("hh:mm");
    listTimeFormat<<i18n("hh:mm:ss");
    listTimeFormat<<i18n("hh:mm AP");
    listTimeFormat<<i18n("hh:mm:ss AP");
    listTimeFormat<<i18n("mm:ss.zzz");
    return listTimeFormat;
}

QCString KoTimeVariable::formatStr()
{
    QCString string;
    QStringList stringList;
    KDialogBase* dialog=new KDialogBase(0, 0, true, i18n("Time Format"), KDialogBase::Ok|KDialogBase::Cancel);
    TimeFormatWidget* widget=new TimeFormatWidget(dialog);
    dialog->setMainWidget(widget);
    KConfig config( "kofficerc" );
    int count=0;
    bool selectLast=false;
    if( config.hasGroup("Time format history") )
    {
        config.setGroup("Time format history");
        int noe=config.readNumEntry("Number Of Entries", 5);
        for(int i=0;i<noe;i++)
        {
            QString num, tmpString;
            num.setNum(i);
            tmpString=config.readEntry("Last Used"+num);
            if(tmpString.compare(i18n("Locale"))==0)
            {
                if(i==0) selectLast = true;
                continue;
            }
            if(stringList.contains(tmpString))
                continue;
            if(!tmpString.isEmpty())
            {
                stringList.append(tmpString);
                count++;
            }
        }
    }
    if(!stringList.isEmpty())
    {
        widget->combo1->insertItem("---");
        widget->combo1->insertStringList(stringList);
    }
    if(selectLast)
    {
        QComboBox *combo= widget->combo1;
        combo->setCurrentItem(combo->count() -1);
    }

    if(dialog->exec()==QDialog::Accepted)
    {
        if ( widget->resultString() == i18n("Locale") )
            string = "locale"; // untranslated form
        else
            string=widget->resultString().utf8();
    }
    else
    {
        return 0;
    }
    config.setGroup("Time format history");
    stringList.remove(string);
    stringList.prepend(string);
    for(int i=0;i<=count;i++)
    {
        QString num;
        num.setNum(i);
        config.writeEntry("Last Used"+num, stringList[i]);
    }
    config.sync();
    delete dialog;
    return QCString("TIME"+string );
}

QCString KoTimeVariable::defaultFormat()
{
    return QCString(QCString("TIME")+QCString("locale") );
}


/******************************************************************/
/* Class: KoCustomVariable                                        */
/******************************************************************/
KoCustomVariable::KoCustomVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat, KoVariableCollection *_varColl )
    : KoVariable( textdoc, varFormat,_varColl )
{
    m_varValue = QVariant( name );
}

QString KoCustomVariable::fieldCode()
{
    return i18n("Custom Variable");
}

QString KoCustomVariable::text()
{
    if (m_varColl->variableSetting()->displayFiedCode())
        return fieldCode();
    else
        return value();
} // use a format when they are customizable



void KoCustomVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement elem = parentElem.ownerDocument().createElement( "CUSTOM" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", m_varValue.toString() );
    elem.setAttribute( "value", value() );
}

void KoCustomVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement e = elem.namedItem( "CUSTOM" ).toElement();
    if (!e.isNull())
    {
        m_varValue = QVariant (e.attribute( "name" ));
        setValue( e.attribute( "value" ) );
    }
}

QString KoCustomVariable::value() const
{
    return m_varColl->getVariableValue( m_varValue.toString() );
}

void KoCustomVariable::setValue( const QString &v )
{
    m_varColl->setVariableValue( m_varValue.toString(), v );
}

QStringList KoCustomVariable::actionTexts()
{
    return QStringList( i18n( "Custom..." ) );
}

void KoCustomVariable::recalc()
{
    resize();
}

/******************************************************************/
/* Class: KoMailMergeVariable                                  */
/******************************************************************/
KoMailMergeVariable::KoMailMergeVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl )
    : KoVariable( textdoc, varFormat, _varColl )
{
    m_varValue = QVariant ( name );
}

QString KoMailMergeVariable::fieldCode()
{
    return i18n("Mail Merge");
}


void KoMailMergeVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement elem = parentElem.ownerDocument().createElement( "MAILMERGE" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", m_varValue.toString() );
}

void KoMailMergeVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement e = elem.namedItem( "MAILMERGE" ).toElement();
    if (!e.isNull())
        m_varValue = QVariant( e.attribute( "name" ) );
}

QString KoMailMergeVariable::value() const
{
    return QString();//m_doc->getMailMergeDataBase()->getValue( m_name );
}

QString KoMailMergeVariable::text()
{
    // ## should use a format maybe
    QString v = value();
    if ( v == name() )
        return "<" + v + ">";
    return v;
}

QStringList KoMailMergeVariable::actionTexts()
{
    return QStringList( i18n( "&Mail Merge..." ) );
}

/******************************************************************/
/* Class: KoPgNumVariable                                         */
/******************************************************************/
KoPgNumVariable::KoPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl )
        : KoVariable( textdoc, varFormat, _varColl ), m_subtype( subtype )
{
}

QString KoPgNumVariable::fieldCode()
{
    if ( m_subtype == VST_PGNUM_CURRENT )
        return i18n("Page Current Num");
    else if (  m_subtype == VST_PGNUM_TOTAL )
        return i18n("Total Page Num");
    else
        return i18n("Current Section");
}


void KoPgNumVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement pgNumElem = parentElem.ownerDocument().createElement( "PGNUM" );
    parentElem.appendChild( pgNumElem );
    pgNumElem.setAttribute( "subtype", m_subtype );
    if ( m_subtype == VST_PGNUM_CURRENT || m_subtype == VST_PGNUM_TOTAL )
        pgNumElem.setAttribute( "value", m_varValue.toInt() );
    else
        pgNumElem.setAttribute( "value", m_varValue.toString() );
}

void KoPgNumVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement pgNumElem = elem.namedItem( "PGNUM" ).toElement();
    if (!pgNumElem.isNull())
    {
        m_subtype = pgNumElem.attribute("subtype").toInt();
        if ( m_subtype == VST_PGNUM_CURRENT || m_subtype == VST_PGNUM_TOTAL )
            m_varValue = QVariant(pgNumElem.attribute("value").toInt());
        else
            m_varValue = QVariant(pgNumElem.attribute("value"));
    }
}

QStringList KoPgNumVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Page Number" );
    lst << i18n( "Number of Pages" );
    lst << i18n( "Section Title" );
    return lst;
}

QStringList KoPgNumVariable::subTypeText()
{
    return KoPgNumVariable::actionTexts();
}

void KoPgNumVariable::setVariableSubType( short int type)
{
    m_subtype=type;
}

/******************************************************************/
/* Class: KoFieldVariable                                         */
/******************************************************************/
KoFieldVariable::KoFieldVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat, KoVariableCollection *_varColl ,KoDocument *_doc )
    : KoVariable( textdoc, varFormat,_varColl ), m_subtype( subtype ), m_doc(_doc)
{
}

QString KoFieldVariable::fieldCode()
{
    switch( m_subtype ) {
    case VST_FILENAME:
        return i18n("Filename");
        break;
    case VST_DIRECTORYNAME:
        return i18n("Directory Name");
        break;
    case VST_PATHFILENAME:
        return i18n("Path Filename");
        break;
    case VST_FILENAMEWITHOUTEXTENSION:
        return i18n("Filename Without Extension");
        break;
    case VST_AUTHORNAME:
        return i18n("Author Name");
        break;
    case VST_EMAIL:
        return i18n("Email");
        break;
    case VST_COMPANYNAME:
        return i18n("Company Name");
        break;
    case VST_TELEPHONE:
        return i18n("Telephone");
        break;
    case VST_FAX:
        return i18n("Fax");
        break;
    case VST_COUNTRY:
        return i18n("Country");
        break;
    case VST_POSTAL_CODE:
        return i18n("Postal Code");
        break;
    case VST_CITY:
        return i18n("City");
        break;
    case VST_STREET:
        return i18n("Street");
        break;
    case VST_AUTHORTITLE:
        return i18n("Author Title");
        break;
    case VST_TITLE:
        return i18n("Title");
        break;
    case VST_ABSTRACT:
        return i18n("Abstract");
        break;
    }
    return i18n("Field");
}

QString KoFieldVariable::text()
{
    if (m_varColl->variableSetting()->displayFiedCode())
        return fieldCode();
    else
        return value();
} // use a format when they are customizable


void KoFieldVariable::saveVariable( QDomElement& parentElem )
{
    //kdDebug(32500) << "KoFieldVariable::saveVariable" << endl;
    QDomElement elem = parentElem.ownerDocument().createElement( "FIELD" );
    parentElem.appendChild( elem );
    elem.setAttribute( "subtype", m_subtype );
    elem.setAttribute( "value", m_varValue.toString() );
}

void KoFieldVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement e = elem.namedItem( "FIELD" ).toElement();
    if (!e.isNull())
    {
        m_subtype = e.attribute( "subtype" ).toInt();
        if ( m_subtype == VST_NONE )
            kdWarning() << "Field subtype of -1 found in the file !" << endl;
        m_varValue = QVariant( e.attribute( "value" ) );
    } else
        kdWarning() << "FIELD element not found !" << endl;
}

void KoFieldVariable::recalc()
{
    QString value;
    switch( m_subtype ) {
        case VST_NONE:
            kdWarning() << "KoFieldVariable::recalc() called with m_subtype = VST_NONE !" << endl;
            break;
        case VST_FILENAME:
            value = m_doc->url().fileName();
            break;
        case VST_DIRECTORYNAME:
            value = m_doc->url().directory();
            break;
	case VST_PATHFILENAME:
            value=m_doc->url().path();
            break;
        case VST_FILENAMEWITHOUTEXTENSION:
        {
            QString file=m_doc->url().fileName();
            int pos=file.findRev(".");
            if(pos !=-1)
                value=file.mid(0,pos);
            else
                value=file;
        }
        break;
        case VST_AUTHORNAME:
        case VST_EMAIL:
        case VST_COMPANYNAME:
        case VST_TELEPHONE:
        case VST_FAX:
        case VST_COUNTRY:
        case VST_POSTAL_CODE:
        case VST_CITY:
        case VST_STREET:
        case VST_AUTHORTITLE:
        {
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
            if ( !authorPage )
                kdWarning() << "Author information not found in documentInfo !" << endl;
            else
            {
                if ( m_subtype == VST_AUTHORNAME )
                    value = authorPage->fullName();
                else if ( m_subtype == VST_EMAIL )
                    value = authorPage->email();
                else if ( m_subtype == VST_COMPANYNAME )
                    value = authorPage->company();
                else if ( m_subtype == VST_TELEPHONE )
                    value = authorPage->telephone();
                else if ( m_subtype == VST_FAX )
                    value = authorPage->fax();
                else if ( m_subtype == VST_COUNTRY )
                    value = authorPage->country();
                else if ( m_subtype == VST_POSTAL_CODE )
                    value = authorPage->postalCode();
                else if ( m_subtype == VST_CITY )
                    value = authorPage->city();
                else if ( m_subtype == VST_STREET )
                    value = authorPage->street();
                else if ( m_subtype == VST_AUTHORTITLE )
                    value = authorPage->title();
            }
        }
        break;
        case VST_TITLE:
        case VST_ABSTRACT:
        {
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
            if ( !aboutPage )
                kdWarning() << "'About' page not found in documentInfo !" << endl;
            else
            {
                if ( m_subtype == VST_TITLE )
                    value = aboutPage->title();
                else
                    value = aboutPage->abstract();
            }
        }
        break;
    }

    if ( value.isEmpty() )
        value = i18n("<None>");
    m_varValue = QVariant( value );
    resize();
}

QStringList KoFieldVariable::actionTexts()
{
    // NOTE: if you change here, also change fieldSubType()
    QStringList lst;
    lst << i18n( "Author Name" );
    lst << i18n( "Title" );
    lst << i18n( "Company" );
    lst << i18n( "Email" );
    lst << i18n( "Telephone");
    lst << i18n( "Fax");
    lst << i18n( "Street" );
    lst << i18n( "Postal Code" );
    lst << i18n( "City" );
    lst << i18n( "Country");

    lst << i18n( "Document Title" );
    lst << i18n( "Document Abstract" );

    lst << i18n( "File Name" );
    lst << i18n( "File Name Without Extension" );
    lst << i18n( "Directory Name" ); // is "Name" necessary ?
    lst << i18n( "Directory and File Name" );
    return lst;
}

short int KoFieldVariable::variableSubType( short int menuNumber )
{
    return fieldSubType(menuNumber);
}

KoFieldVariable::FieldSubType KoFieldVariable::fieldSubType(short int menuNumber)
{
    // NOTE: if you change here, also change actionTexts()
    FieldSubType v;
    switch (menuNumber)
    {
        case 0: v = VST_AUTHORNAME;
                break;
        case 1: v = VST_AUTHORTITLE;
                break;
        case 2: v = VST_COMPANYNAME;
                break;
        case 3: v = VST_EMAIL;
                break;
        case 4: v = VST_TELEPHONE;
                break;
        case 5: v = VST_FAX;
                break;
        case 6: v = VST_STREET;
                break;
        case 7: v = VST_POSTAL_CODE;
                break;
        case 8: v = VST_CITY;
                break;
        case 9: v = VST_COUNTRY;
                break;
        case 10: v = VST_TITLE;
                break;
        case 11: v = VST_ABSTRACT;
                break;
        case 12: v = VST_FILENAME;
                break;
        case 13: v = VST_FILENAMEWITHOUTEXTENSION;
                break;
        case 14: v = VST_DIRECTORYNAME;
                break;
        case 15: v = VST_PATHFILENAME;
                break;
        default:
            v = VST_NONE;
            break;
    }
    return v;
}

QStringList KoFieldVariable::subTypeText()
{
    return KoFieldVariable::actionTexts();
}

/******************************************************************/
/* Class: KoLinkVariable                                          */
/******************************************************************/
KoLinkVariable::KoLinkVariable( KoTextDocument *textdoc, const QString & _linkName, const QString & _ulr,KoVariableFormat *varFormat,KoVariableCollection *_varColl )
    : KoVariable( textdoc, varFormat,_varColl )
    ,m_url(_ulr)
{
    m_varValue = QVariant( _linkName );
}

QString KoLinkVariable::fieldCode()
{
    return i18n("Link");
}

QString KoLinkVariable::text()
{
    if (m_varColl->variableSetting()->displayFiedCode())
        return fieldCode();
    else
        return value();
}

void KoLinkVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement linkElem = parentElem.ownerDocument().createElement( "LINK" );
    parentElem.appendChild( linkElem );
    linkElem.setAttribute( "linkName", m_varValue.toString() );
    linkElem.setAttribute( "hrefName", m_url );
}

void KoLinkVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement linkElem = elem.namedItem( "LINK" ).toElement();
    if (!linkElem.isNull())
    {
        m_varValue = QVariant(linkElem.attribute("linkName"));
        m_url = linkElem.attribute("hrefName");
    }
}

void KoLinkVariable::recalc()
{
    resize();
}

QStringList KoLinkVariable::actionTexts()
{
    return QStringList( i18n( "Link..." ) );
}


void KoLinkVariable::drawCustomItem( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, const int offset )
{
    KoTextFormat * fmt = format();
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();

    bool displayLink = m_varColl->variableSetting()->displayLink();
    QFont font( fmt->screenFont( zh ) );
    if ( m_varColl->variableSetting()->underlineLink() )
        font.setUnderline( true );
    QColor textColor = displayLink ? cg.color( QColorGroup::Link ) : fmt->color();

    drawCustomItemHelper( p, x, y, cg, selected, offset, fmt, font, textColor );
}


/******************************************************************/
/* Class: KoNoteVariable                                          */
/******************************************************************/
KoNoteVariable::KoNoteVariable( KoTextDocument *textdoc, const QString & _note,KoVariableFormat *varFormat,KoVariableCollection *_varColl )
    : KoVariable( textdoc, varFormat,_varColl )
{
    m_varValue = QVariant( _note );
}

QString KoNoteVariable::fieldCode()
{
    return i18n("Note");
}

void KoNoteVariable::saveVariable( QDomElement& parentElem )
{
    QDomElement linkElem = parentElem.ownerDocument().createElement( "NOTE" );
    parentElem.appendChild( linkElem );
    linkElem.setAttribute( "note", m_varValue.toString() );
}

void KoNoteVariable::load( QDomElement& elem )
{
    KoVariable::load( elem );
    QDomElement linkElem = elem.namedItem( "NOTE" ).toElement();
    if (!linkElem.isNull())
    {
        m_varValue = QVariant(linkElem.attribute("note"));
    }
}

void KoNoteVariable::recalc()
{
    resize();
}

QStringList KoNoteVariable::actionTexts()
{
    return QStringList( i18n( "Note..." ) );
}

QString KoNoteVariable::text()
{
    if (m_varColl->variableSetting()->displayFiedCode())
        return fieldCode();
    else
        //for a note return just a "space" we can look at
        //note when we "right button"
        return QString(" ");

}

void KoNoteVariable::drawCustomItem( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, const int offset )
{
    if ( !m_varColl->variableSetting()->displayComment())
        return;

    KoTextFormat * fmt = format();
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    int _bl, _y;
    KoTextParag * parag = paragraph();
    //kdDebug(32500) << "KoVariable::draw index=" << index() << " x=" << x << " y=" << y << endl;
    int h = parag->lineHeightOfChar( index(), &_bl /*unused*/, &_y /*unused*/);

    h = zh->layoutUnitToPixelY( y, h );

    p->save();
    p->setPen( QPen( fmt->color() ) );
    if ( fmt->textBackgroundColor().isValid() )
        p->fillRect( x, y, zh->layoutUnitToPixelX( width ), h, fmt->textBackgroundColor() );
    if ( selected )
    {
        p->setPen( QPen( cg.color( QColorGroup::HighlightedText ) ) );
        p->fillRect( x, y,  zh->layoutUnitToPixelX( width ), h, cg.color( QColorGroup::Highlight ) );
    }
    else if ( textDocument() && p->device()->devType() != QInternal::Printer )
    {
        p->fillRect( x, y, zh->layoutUnitToPixelX( width ), h,  Qt::yellow);
        p->setPen( QPen( cg.color( QColorGroup::Highlight ), 0, Qt::DotLine ) );
        p->drawRect( x, y, zh->layoutUnitToPixelX( width ), h );
    }
    //call it for use drawCustomItemHelper just for draw font effect
    KoVariable::drawCustomItem( p, x, y, cx, cy, cw, ch, cg, selected, offset );

    p->restore();
}
