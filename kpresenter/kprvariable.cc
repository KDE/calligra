/* This file is part of the KDE project
   Copyright (C) 2001 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "kprvariable.h"
#include "kprvariable.moc"
#include <koVariable.h>
#include "kpresenter_doc.h"
#include "kprcommand.h"
#include <klocale.h>
#include <kdebug.h>


KPrVariableCollection::KPrVariableCollection(KoVariableSettings *_setting)
    : KoVariableCollection(_setting)
{
}

KoVariable *KPrVariableCollection::createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc,  int _correct, bool _forceDefaultFormat )
{
    KPresenterDoc*m_doc=static_cast<KPresenterDoc*>(doc);
    switch(type) {
    case VT_PGNUM:
    {
        kdDebug(33001)<<" subtype == KoPgNumVariable::VST_CURRENT_SECTION :"<<(subtype == KPrPgNumVariable::VST_CURRENT_SECTION)<<endl;
        kdDebug(33001)<<" varFormat :"<<varFormat<<endl;
        if ( !varFormat )
            varFormat = (subtype == KPrPgNumVariable::VST_CURRENT_SECTION) ? coll->format("STRING") : coll->format("NUMBER");
        return new KPrPgNumVariable( textdoc,subtype, varFormat,this,m_doc  );
    }
    case VT_FIELD:
        if ( !varFormat )
            varFormat =  coll->format("STRING");
        return new KPrFieldVariable( textdoc, subtype, varFormat, this, m_doc );
    case VT_DATE:
        if ( !varFormat )
        {
            if ( _forceDefaultFormat || KoDateVariable::VST_DATE_LAST_PRINTING)
                varFormat = coll->format( KoDateVariable::defaultFormat() );
            else
            {
                QCString result = KoDateVariable::formatStr(_correct);
                if ( result == 0 )//we cancel insert variable
                    return 0L;
                varFormat =  coll->format( result );
            }
        }
        return new KPrDateVariable( textdoc, subtype, varFormat, this, m_doc,_correct );
    case VT_TIME:
        if ( !varFormat )
        {
            if ( _forceDefaultFormat )
                varFormat = coll->format( KoTimeVariable::defaultFormat() );
            else
            {
                QCString result = KoTimeVariable::formatStr(_correct);
                if ( result == 0 )//we cancel insert variable
                    return 0L;
                varFormat =  coll->format( result );
            }
        }
        return new KPrTimeVariable( textdoc, subtype, varFormat, this, m_doc,_correct );
    default:
        return KoVariableCollection::createVariable( type, subtype, coll, varFormat, textdoc, doc, _correct, _forceDefaultFormat);
    }
}

QPtrList<KAction> KPrVariableCollection::variableActionList()
{
    return selectedVariable()->actionList();
}


/******************************************************************/
/* Class: KPrPgNumVariable                                         */
/******************************************************************/
KPrPgNumVariable::KPrPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KPresenterDoc *doc  )
    : KoPgNumVariable( textdoc, subtype, varFormat ,_varColl ),m_doc(doc)
{
}

void KPrPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_TOTAL )
    {
        m_varValue = QVariant( (int)(m_doc->getPageNums()+m_varColl->variableSetting()->startingPage()-1));
        resize();
    }
}

void KPrPgNumVariable::setVariableSubType( short int type)
{
    m_subtype=type;
    setVariableFormat((m_subtype == KPrPgNumVariable::VST_CURRENT_SECTION) ? m_doc->variableFormatCollection()->format("STRING") : m_doc->variableFormatCollection()->format("NUMBER"));
}

QPtrList<KAction> KPrPgNumVariable::actionList()
{
    QPtrList<KAction> listAction=QPtrList<KAction>();
    QStringList list = subTypeText();
    QStringList::ConstIterator it = list.begin();
    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            KToggleAction * act = new KToggleAction( (*it));
            if ( m_subtype == i )
                act->setChecked( true );
            connect( act, SIGNAL(activated()),this, SLOT(slotChangeSubType()) );

            m_subTextMap.insert( act, i );
            listAction.append( act );
        }
    }

    return listAction;
}

void KPrPgNumVariable::slotChangeSubType()
{
    KAction * act = (KAction *)(sender());
    SubTextMap::Iterator it = m_subTextMap.find( act );
    if ( it == m_subTextMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else // if ( m_subtype != variableSubType(*it) )
    {
        short int oldvalue = m_subtype;
        if ( oldvalue != variableSubType(*it) )
        {
            setVariableSubType( variableSubType(*it) );
            KPrChangePgNumVariableValue *cmd=new KPrChangePgNumVariableValue(
                i18n( "Change Page Number Variable" ),
                m_doc, oldvalue, m_subtype, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_PGNUM);
        }
    }
}


/******************************************************************/
/* Class: KWFieldVariable                                         */
/******************************************************************/
KPrFieldVariable::KPrFieldVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPresenterDoc *_doc )
    : KoFieldVariable( textdoc, subtype, varFormat, _varColl ,_doc ),
      m_doc(_doc)

{
}

QPtrList<KAction> KPrFieldVariable::actionList()
{
    QPtrList<KAction> listAction=QPtrList<KAction>();
    QStringList list = subTypeText();
    QStringList::ConstIterator it = list.begin();
    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            KToggleAction * act = new KToggleAction( (*it));
            if ( variableSubType(i) == m_subtype )
                act->setChecked( true );
            connect( act, SIGNAL(activated()),this, SLOT(slotChangeSubType()) );

            m_subTextMap.insert( act, i );
            listAction.append( act );
        }
    }

    return listAction;
}

void KPrFieldVariable::slotChangeSubType()
{
    KAction * act = (KAction *)(sender());
    SubTextMap::Iterator it = m_subTextMap.find( act );
    if ( it == m_subTextMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else // if ( m_subtype != variableSubType(*it) )
    {
        short int oldvalue = m_subtype;
        if ( oldvalue != variableSubType(*it))
        {
            setVariableSubType( variableSubType(*it) );
            KPrChangeFieldVariableSubType *cmd=new KPrChangeFieldVariableSubType(
                i18n( "Change Field Variable" ),
                m_doc, oldvalue, m_subtype, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_FIELD);
        }
    }
}

KPrDateVariable::KPrDateVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPresenterDoc *_doc, int _correct )
    : KoDateVariable( textdoc, subtype, varFormat, _varColl,_correct ),
      m_doc(_doc)
{
}

QPtrList<KAction> KPrDateVariable::actionList()
{
    QPtrList<KAction> listAction=QPtrList<KAction>();
    QStringList list = subTypeText();
    QStringList::ConstIterator it = list.begin();
    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            KToggleAction * act = new KToggleAction( (*it));
            if ( i == m_subtype)
                act->setChecked( true );

            connect( act, SIGNAL(activated()),this, SLOT(slotChangeSubType()) );

            m_subTextMap.insert( act, i );
            listAction.append( act );
        }
    }
    list=subTypeFormat();
    it = list.begin();
    KoVariableDateFormat *frm = static_cast<KoVariableDateFormat *>(variableFormat());
    QString format = frm->m_strFormat;

    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if( i == 0)
            listAction.append( new KActionSeparator() );

        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            subFormatDef v;
            QDate ct=QDate::currentDate();
            if((*it)==i18n("Locale").lower())
                v.translatedString=KGlobal::locale()->formatDate( ct );
            else
                v.translatedString=ct.toString(*it);
            v.format=*it;

            KToggleAction * act = new KToggleAction(v.translatedString);
            if ( (*it) == format )
                act->setChecked( true );
            connect( act, SIGNAL(activated()),this, SLOT(slotChangeFormat()) );
            m_subFormatMap.insert( act, v );
            listAction.append( act );
        }
    }

    return listAction;
}

void KPrDateVariable::slotChangeSubType()
{
    KAction * act = (KAction *)(sender());
    SubTextMap::Iterator it = m_subTextMap.find( act );
    if ( it == m_subTextMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else // if ( m_subtype != variableSubType(*it) )
    {
        short int oldvalue = m_subtype;
        if (oldvalue != variableSubType(*it) )
        {
            setVariableSubType( variableSubType(*it) );
            KPrChangeDateVariableSubType *cmd=new KPrChangeDateVariableSubType(
                i18n( "Change Date Variable Type" ),
                m_doc, oldvalue, m_subtype, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_DATE);
        }
    }
}

void KPrDateVariable::slotChangeFormat()
{
    KAction * act = (KAction *)(sender());
    SubFormatMap::Iterator it = m_subFormatMap.find( act );
    if ( it == m_subFormatMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else
    {
        KoVariableDateFormat *frm = static_cast<KoVariableDateFormat *>(variableFormat());
        QString oldvalue = frm->m_strFormat;
        if ( oldvalue != (*it).format)
        {
            frm->m_strFormat = (*it).format;
            KPrChangeDateVariableFormat *cmd=new KPrChangeDateVariableFormat(
                i18n( "Change Date Variable Format" ),
                m_doc, oldvalue, frm->m_strFormat, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_DATE);
        }
    }
}

KPrTimeVariable::KPrTimeVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPresenterDoc *_doc,int _correct )
    : KoTimeVariable( textdoc, subtype, varFormat, _varColl,_correct ),
      m_doc(_doc)
{
}

QPtrList<KAction> KPrTimeVariable::actionList()
{
    QPtrList<KAction> listAction=QPtrList<KAction>();
    QStringList list = subTypeText();
    QStringList::ConstIterator it = list.begin();
    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            KToggleAction * act = new KToggleAction( (*it));
            if ( i == m_subtype)
                act->setChecked( true );

            connect( act, SIGNAL(activated()),this, SLOT(slotChangeSubType()) );

            m_subTextMap.insert( act, i );
            listAction.append( act );
        }
    }
    list=subTypeFormat();
    it = list.begin();
    KoVariableTimeFormat *frm = static_cast<KoVariableTimeFormat *>(variableFormat());
    QString format = frm->m_strFormat;

    for ( int i = 0; it != list.end() ; ++it, ++i )
    {
        if( i == 0)
            listAction.append( new KActionSeparator() );

        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            subFormatDef v;
            QTime ct=QTime::currentTime();
            if((*it)==i18n("Locale").lower())
                v.translatedString=KGlobal::locale()->formatTime( ct );
            else
                v.translatedString=ct.toString(*it);
            v.format=*it;

            KToggleAction * act = new KToggleAction(v.translatedString);
            if ( *it == format )
                act->setChecked( true );

            connect( act, SIGNAL(activated()),this, SLOT(slotChangeFormat()) );
            m_subFormatMap.insert( act, v );
            listAction.append( act );
        }
    }

    return listAction;
}

void KPrTimeVariable::slotChangeSubType()
{
    KAction * act = (KAction *)(sender());
    SubTextMap::Iterator it = m_subTextMap.find( act );
    if ( it == m_subTextMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else // if ( m_subtype != variableSubType(*it) )
    {
        short int oldvalue = m_subtype;
        if ( oldvalue != variableSubType(*it))
        {
            setVariableSubType( variableSubType(*it) );
            KPrChangeTimeVariableSubType *cmd=new KPrChangeTimeVariableSubType(
                i18n( "Change Time Variable Type" ),
                m_doc, oldvalue, m_subtype, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_TIME);
        }
    }
}

void KPrTimeVariable::slotChangeFormat()
{
    KAction * act = (KAction *)(sender());
    SubFormatMap::Iterator it = m_subFormatMap.find( act );
    if ( it == m_subFormatMap.end() )
        kdWarning() << "Action not found in m_subTextMap." << endl;
    else
    {
        KoVariableTimeFormat *frm = static_cast<KoVariableTimeFormat *>(variableFormat());
        QString oldvalue = frm->m_strFormat;
        if ( oldvalue != (*it).format )
        {
            frm->m_strFormat = (*it).format;
            KPrChangeTimeVariableFormat *cmd=new KPrChangeTimeVariableFormat(
                i18n( "Change Time Variable Format" ),
                m_doc, oldvalue, frm->m_strFormat, this );
            m_doc->addCommand(cmd);
            paragraph()->invalidate( 0 );
            paragraph()->setChanged( true );
            m_doc->recalcVariables(VT_TIME);
        }
    }
}
