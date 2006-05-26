// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002-2004 Laurent MONTEL <montel@kde.org>
   Copyright (C) 2004-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include <kurl.h>

#include "KPrPage.h"
#include "KPrView.h"
#include "KPrPageIface.h"

#include "KPrBackground.h"
#include "KPrLineObject.h"
#include "KPrRectObject.h"
#include "KPrEllipseObject.h"
#include "KPrAutoformObject.h"
#include "KPrTextObject.h"
#include "KPrPixmapObject.h"
#include "KPrPieObject.h"
#include "KPrPartObject.h"
#include "KPrGroupObject.h"
#include "KPrCommand.h"
#include "KPrFreehandObject.h"
#include "KPrPolylineObject.h"
#include "KPrBezierCurveObject.h"
#include "KPrPolygonObject.h"
#include "KPrClosedLineObject.h"
#include "KPrUtils.h"
#include <kdebug.h>
#include <KoQueryTrader.h>
#include "KPrDocument.h"

#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoTextZoomHandler.h>
#include <KoXmlNS.h>
#include "KoPointArray.h"
#include "KPrTextDocument.h"
#include <KoTextObject.h>
#include <KoTextParag.h>
#include <KoRect.h>
#include <qapplication.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>
#include <KoStyleCollection.h>
#include <KoVariable.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <ktempfile.h>
#include <qbuffer.h>
#include <QRegExp>
#include <QFile>

struct listAnimation {
    KPrObject *obj;
    int objIndex;
    bool appear;
};

typedef QMap<int, Q3PtrList<listAnimation> > lstMap;


KPrPage::KPrPage(KPrDocument *_doc, KPrPage *masterPage )
    : m_doc( _doc )
    , m_masterPage( masterPage )
    , m_dcop( 0 )
    , m_selectedSlides( true )
    , m_bHasHeader( false )
    , m_bHasFooter( false )
    , m_useMasterBackground( false )
    , m_displayObjectFromMasterPage( true )
    , m_displayBackground( true )
    , m_pageEffect( PEF_NONE )
    , m_pageEffectSpeed( ES_MEDIUM )
    , m_soundEffect( false )
    , m_soundFileName( QString::null )
    , m_pageTimer( 1 )
{
    kDebug(33001)<<"create page : KPrPage::KPrPage(KPrDocument *_doc )"<<this<<endl;
    m_objectList.setAutoDelete( false );

    m_kpbackground= new KPrBackGround( this );

    //don't create dcopobject by default
    //dcopObject();
}

KPrPage::~KPrPage()
{
    kDebug(33001)<<"Delete page :KPrPage::~KPrPage() :"<<this<<endl;
    //delete object list.
    m_objectList.setAutoDelete( true );
    m_objectList.clear();
    delete m_kpbackground;
    delete m_dcop;
}

DCOPObject* KPrPage::dcopObject()
{
    if ( !m_dcop ) {
        // 0-based. 1-based would be nicer for the dcop user, but well, docs and views are 0-based,
	// and the page(int) DCOP call is 0-based.
        int pgnum = m_doc->pageList().findRef( this );
        m_dcop = new KPrPageIface( this, pgnum );
    }

    return m_dcop;
}


void KPrPage::saveOasisObject( KoStore *store, KoXmlWriter &xmlWriter, KoSavingContext& context, int & indexObj, int &partIndexObj,  KoXmlWriter* manifestWriter ) const
{
    Q_UNUSED( store );
    Q_UNUSED( manifestWriter );
    KPrObject::KPOasisSaveContext sc( xmlWriter, context, indexObj, partIndexObj, isMasterPage() );

    KTempFile animationTmpFile;
    animationTmpFile.setAutoDelete( true );
    QFile* tmpFile = animationTmpFile.file();
    KoXmlWriter animationTmpWriter( tmpFile );
    lstMap listObjectAnimation;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        it.current()->saveOasisObject( sc );

        if ( it.current()->hasAnimation() )
        {
            if ( it.current()->getEffect() != EF_NONE ||
                 it.current()->getAppearStep() != 0 ||
                 !it.current()->getAppearSoundEffectFileName().isEmpty() )
            {
                kDebug(33001) << "has animation" << endl;
                listAnimation *lstappear = new listAnimation;
                lstappear->obj = it.current();
                lstappear->objIndex = indexObj;
                lstappear->appear = true;
                //kDebug()<<" indexObj :"<<indexObj<<endl;
                lstMap::Iterator tmp = listObjectAnimation.find( it.current()->getAppearStep() );
                if ( tmp!= listObjectAnimation.end() )
                {
                    //kDebug()<<" group already exist \n";
                    tmp.data().append( lstappear );
                }
                else
                {
                    //kDebug()<<" create new list \n";
                    Q3PtrList<listAnimation> tmp2;
                    tmp2.append( lstappear );
                    listObjectAnimation.insert( it.current()->getAppearStep(), tmp2 );
                }
            }
            if ( it.current()->getEffect3() != EF3_NONE ||
                 it.current()->getDisappear() ||
                 ! it.current()->getDisappearSoundEffectFileName().isEmpty())
            {
                listAnimation *lstappear = new listAnimation;
                lstappear->obj = it.current();
                lstappear->objIndex = indexObj;
                lstappear->appear = false;
                //kDebug()<<" indexObj :"<<indexObj<<endl;
                lstMap::Iterator tmp = listObjectAnimation.find( it.current()->getDisappearStep() );
                if ( tmp!= listObjectAnimation.end() )
                {
                    //kDebug()<<" group already exist \n";
                    tmp.data().append( lstappear );
                }
                else
                {
                    //kDebug()<<" create new list \n";
                    Q3PtrList<listAnimation> tmp2;
                    tmp2.append( lstappear );
                    listObjectAnimation.insert( it.current()->getDisappearStep(), tmp2 );
                }
            }

        }
        ++indexObj;
    }

    if ( !listObjectAnimation.isEmpty() )
    {
        kDebug()<<"! listObjectAnimation.isEmpty() :"<<listObjectAnimation.count()<<endl;
        animationTmpWriter.startElement( "presentation:animations" );
        lstMap::Iterator it = listObjectAnimation.begin();
        lstMap::Iterator end = listObjectAnimation.end();
        for (; it != end; ++it )
        {
            if ( it.data().count() == 1 )
            {
                kDebug()<<" add unique element \n";
                if ( it.data().at( 0 )->appear )
                    it.data().at( 0 )->obj->saveOasisObjectStyleShowAnimation( animationTmpWriter, it.data().at( 0 )->objIndex );
                else
                    it.data().at( 0 )->obj->saveOasisObjectStyleHideAnimation( animationTmpWriter, it.data().at( 0 )->objIndex );
            }
            else if ( it.data().count() > 1 )
            {
                Q3PtrList<listAnimation> list = it.data();
                animationTmpWriter.startElement( "presentation:animation-group" );
                for ( uint i = 0; i < list.count(); ++i )
                {
                    if ( list.at(i) )
                    {
                        kDebug()<<" add group element : "<<i<<endl;
                        if ( list.at(i)->appear )
                            list.at(i)->obj->saveOasisObjectStyleShowAnimation( animationTmpWriter, list.at(i)->objIndex );
                        else
                            list.at(i)->obj->saveOasisObjectStyleHideAnimation( animationTmpWriter, list.at(i)->objIndex );
                    }
                }
                animationTmpWriter.endElement();
            }

        }
        animationTmpWriter.endElement();//close "presentation:animations"
        tmpFile->close();
        xmlWriter.addCompleteElement( tmpFile );

    }
    else
        tmpFile->close();
    animationTmpFile.close();
}

void KPrPage::load( const QDomElement &element )
{
    m_kpbackground->load( element );
    QDomElement e=element.namedItem("PGEFFECT").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setPageEffect(static_cast<PageEffect>(tmp));
        tmp = (int)ES_MEDIUM;
        if(e.hasAttribute("speed"))
            tmp=e.attribute("speed").toInt();
        setPageEffectSpeed( static_cast<EffectSpeed>(tmp) );
    }
    e=element.namedItem("PGTIMER").toElement();
    if(!e.isNull()) {
        int timer = 1;
        if(e.hasAttribute("timer"))
            timer=e.attribute("timer").toInt();
        setPageTimer(timer);
    }
    else
        setPageTimer(1);
    e=element.namedItem("PGSOUNDEFFECT").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("soundEffect"))
            m_soundEffect=static_cast<bool>(e.attribute("soundEffect").toInt());
        else
            m_soundEffect=false;

        if(e.hasAttribute("soundFileNmae")) // old typo
            m_soundFileName=e.attribute("soundFileNmae");
        else
            m_soundFileName=e.attribute("soundFileName");
    }
    else {
        m_soundFileName=QString::null;
    }

    e=element.namedItem("HEADERFOOTER").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("header"))
            m_bHasHeader =static_cast<bool>(e.attribute("header").toInt());
        else
            m_bHasHeader = false;

        if(e.hasAttribute("footer"))
            m_bHasFooter =static_cast<bool>(e.attribute("footer").toInt());
        else
            m_bHasFooter = false;
    }
}

//TODO: implement display/hide background into old file format
void KPrPage::loadOasis(KoOasisContext & context )
{
    m_kpbackground->loadOasis( context );

    KoStyleStack& styleStack = context.styleStack();
    kDebug()<<"KPrPage::loadOasis()\n";
    styleStack.setTypeProperties( "drawing-page" );

    if ( !isMasterPage() )
    {
        if ( !styleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
        {
            // if non is set the fill style of the master slide is used: see OD 14.13.2 Drawing Page Style
            m_useMasterBackground = true;
        }
        //FIXME: fix it in old file format
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "background-objects-visible" ) )
        {
            const QString str = styleStack.attributeNS( KoXmlNS::presentation, "background-objects-visible" );
            m_displayObjectFromMasterPage = ( str == "true" ) ? true : false;
        }

        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "background-visible" ) )
        {
            const QString str = styleStack.attributeNS( KoXmlNS::presentation, "background-visible" );
            m_displayBackground = ( str == "true" ) ? true : false;
        }

        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "visibility" ) )
        {
            const QString str = styleStack.attributeNS( KoXmlNS::presentation, "visibility" );
            if ( str=="hidden" )
                slideSelected( false );
            else
                kDebug()<<" presentation:visibility parameter not implemented :"<<str<<endl;
        }

        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "transition-speed" ) )
        {
            // this argument is not defined into kpresenter_doc and not into kprpage
            // TODO add it into each page.
            QString speed = styleStack.attributeNS( KoXmlNS::presentation, "transition-speed" );
            if ( speed == "slow" )
            {
                m_pageEffectSpeed = ES_SLOW;
            }
            else if ( speed == "medium" )
            {
                m_pageEffectSpeed = ES_MEDIUM;
            }
            else if ( speed == "fast" )
            {
                m_pageEffectSpeed = ES_FAST;
            }
            else
                kDebug()<<" transition-speed not defined :"<<speed<<endl;
        }
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "duration" ))
        {
            m_pageTimer = loadOasisTimer( styleStack.attributeNS( KoXmlNS::presentation, "duration") );
        }
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "transition-type" ) )
        {
            //Not defined into kpresenter
            //it's global for the moment.
            kDebug()<<" presentation:transition-type :"<<styleStack.attributeNS( KoXmlNS::presentation, "transition-type" )<<endl;
        }
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "display-header" ) )
        {
            QString tmp = styleStack.attributeNS( KoXmlNS::presentation, "display-header" );
            setHeader( tmp =="true" ? true : false );
        }
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "display-footer" ) )
        {
            QString tmp = styleStack.attributeNS( KoXmlNS::presentation, "display-footer" );
            setFooter(tmp =="true" ? true : false);
        }
        //TODO presentation:display-date-time; presentation:display-page-number
        if ( styleStack.hasAttributeNS( KoXmlNS::presentation, "transition-style"))
        {
            //kDebug()<<" have a presentation:transition-style------------\n";
            const QString effect = styleStack.attributeNS( KoXmlNS::presentation, "transition-style");
            QString additionalEffect;
            if ( styleStack.hasAttributeNS( KoXmlNS::koffice, "additional-transition-style" ) )
            {
                additionalEffect = styleStack.attributeNS( KoXmlNS::koffice, "additional-transition-style" );
            }
            kDebug() << "Transition name: " << effect << "additional name: " << additionalEffect << endl;
            PageEffect pef;
            if ( effect=="none" )
                pef=PEF_NONE;
            else if (effect=="vertical-stripes" || effect=="vertical-lines") // PEF_BLINDS_VER
                pef=PEF_BLINDS_VER;
            else if (effect=="horizontal-stripes" || effect=="horizontal-lines") // PEF_BLINDS_HOR
                pef=PEF_BLINDS_HOR;
            else if (effect=="spiralin-left" || effect=="spiralin-right"
                    || effect== "spiralout-left" || effect=="spiralout-right") // PEF_SURROUND1
                pef=PEF_SURROUND1;
            else if (effect=="fade-from-top") // PEF_COVER_DOWN
                pef=PEF_COVER_DOWN;
            else if (effect=="fade-from-bottom") // PEF_COVER_UP
                pef=PEF_COVER_UP;
            else if (effect=="fade-from-left") // PEF_COVER_RIGHT
                pef=PEF_COVER_RIGHT;
            else if (effect=="fade-from-right") // PEF_COVER_LEFT
                pef=PEF_COVER_LEFT;
            else if (effect=="fade-from-lowerleft")
            {   // PEF_COVER_RIGHT_UP
                if ( additionalEffect.isEmpty() )
                    pef=PEF_COVER_RIGHT_UP;
                else
                    pef=PEF_STRIPS_RIGHT_UP;
            }
            else if (effect=="fade-from-lowerright") 
            {   // PEF_COVER_LEFT_UP
                if ( additionalEffect.isEmpty() )
                    pef=PEF_COVER_LEFT_UP;
                else
                    pef=PEF_STRIPS_LEFT_UP;
            }
            else if (effect=="fade-from-upperleft") 
            {   // PEF_COVER_RIGHT_DOWN
                if ( additionalEffect.isEmpty() )
                    pef=PEF_COVER_RIGHT_DOWN;
                else
                    pef=PEF_STRIPS_RIGHT_DOWN;
            }
            else if (effect=="fade-from-upperright")
            {   // PEF_COVER_LEFT_DOWN
                if ( additionalEffect.isEmpty() )
                    pef=PEF_COVER_LEFT_DOWN;
                else
                    pef=PEF_STRIPS_LEFT_DOWN;
            }
            else if (effect=="fade-to-center") // PEF_BOX_IN
                pef=PEF_BOX_IN;
            else if (effect=="fade-from-center") // PEF_BOX_OUT
                pef=PEF_BOX_OUT;
            else if (effect=="open-vertical") // PEF_OPEN_HORZ; really, no kidding ;)
                pef=PEF_OPEN_HORZ;
            else if (effect=="open-horizontal") // PEF_OPEN_VERT
                pef=PEF_OPEN_VERT;
            else if (effect=="open") // PEF_OPEN_ALL
                pef=PEF_OPEN_ALL;
            else if (effect=="close-vertical") // PEF_CLOSE_HORZ
                pef=PEF_CLOSE_HORZ;
            else if (effect=="close-horizontal") // PEF_CLOSE_VERT
                pef=PEF_CLOSE_VERT;
            else if (effect=="close") // PEF_CLOSE_ALL
                pef=PEF_CLOSE_ALL;
            else if (effect=="dissolve") // PEF_DISSOLVE; perfect hit ;)
                pef=PEF_DISSOLVE;
            else if (effect=="horizontal-checkerboard") // PEF_CHECKBOARD_ACROSS
                pef=PEF_CHECKBOARD_ACROSS;
            else if (effect=="vertical-checkerboard") // PEF_CHECKBOARD_DOWN
                pef=PEF_CHECKBOARD_DOWN;
            else if (effect=="uncover-to-right" || effect=="roll-from-left") // PEF_UNCOVER_RIGHT
                pef=PEF_UNCOVER_RIGHT;
            else if (effect=="uncover-to-left" || effect=="roll-from-right") // PEF_UNCOVER_LEFT
                pef=PEF_UNCOVER_LEFT;
            else if (effect=="uncover-to-top" || effect=="roll-from-bottom") // PEF_UNCOVER_UP
                pef=PEF_UNCOVER_UP;
            else if (effect=="uncover-to-bottom" || effect=="roll-from-top") // PEF_UNCOVER_DOWN
                pef=PEF_UNCOVER_DOWN;
            else if (effect=="uncover-to-upperleft") // PEF_UNCOVER_LEFT_UP
                pef=PEF_UNCOVER_LEFT_UP;
            else if (effect=="uncover-to-upperright") // PEF_UNCOVER_RIGHT_UP
                pef=PEF_UNCOVER_RIGHT_UP;
            else if (effect=="uncover-to-lowerleft") // PEF_UNCOVER_LEFT_DOWN
                pef=PEF_UNCOVER_LEFT_DOWN;
            else if (effect=="uncover-to-lowerright") // PEF_UNCOVER_RIGHT_DOWN
                pef=PEF_UNCOVER_RIGHT_DOWN;
            else if (effect=="interlocking-horizontal-left")
                pef=PEF_INTERLOCKING_HORZ_1;
            else if (effect=="interlocking-horizontal-right")
                pef=PEF_INTERLOCKING_HORZ_2;
            else if (effect=="interlocking-vertical-top")
                pef=PEF_INTERLOCKING_VERT_1;
            else if (effect=="interlocking-vertical-bottom")
                pef=PEF_INTERLOCKING_VERT_2;
            else if ( effect=="melt" )
                pef=PEF_MELTING;
            else if ( effect=="fly-away" )
                pef=PEF_FLY1;
            else if ( effect=="random" )
                pef=PEF_RANDOM;
            else         // we choose a random transition instead of the unsupported ones ;)
                pef=PEF_RANDOM;
            setPageEffect( pef );
        }
        if ( styleStack.hasChildNodeNS( KoXmlNS::presentation, "sound"))
        {
            //kDebug()<<" presentation:sound !!!!!!!!!!!!!!!!!!!!!\n";
            QDomElement sound = styleStack.childNodeNS( KoXmlNS::presentation, "sound");
            m_soundEffect = true;
            m_soundFileName = sound.attributeNS( KoXmlNS::xlink, "href", QString::null );
        }
    }
}

QDomElement KPrPage::save( QDomDocument &doc )
{
    QDomElement page = m_kpbackground->save( doc );
    QDomElement element;

    if ( m_pageEffect != PEF_NONE) {
        element = doc.createElement("PGEFFECT");
        element.setAttribute("value", static_cast<int>( m_pageEffect ));
        element.setAttribute("speed", static_cast<int>( m_pageEffectSpeed ));
        page.appendChild(element);
    }

    if ( m_pageTimer != 1 ) {
        element = doc.createElement( "PGTIMER" );
        element.setAttribute( "timer", m_pageTimer );
        page.appendChild( element );
    }

    if ( m_soundEffect || !m_soundFileName.isEmpty() ) {
        element = doc.createElement( "PGSOUNDEFFECT" );
        element.setAttribute( "soundEffect", static_cast<int>(m_soundEffect) );
        element.setAttribute( "soundFileName", m_soundFileName );
        page.appendChild( element );
    }
    element = doc.createElement( "HEADERFOOTER" );
    element.setAttribute( "header", static_cast<int>(m_bHasHeader ) );
    element.setAttribute( "footer", static_cast<int>(m_bHasFooter ) );
    page.appendChild( element );
    return page;
}

QString KPrPage::oasisNamePage( int posPage ) const
{
    return  ( m_manualTitle.isEmpty() ?  QString( "page%1" ).arg( posPage ) : m_manualTitle );
}

bool KPrPage::saveOasisPage( KoStore *store, KoXmlWriter &xmlWriter, int posPage, KoSavingContext& context, 
                             int & indexObj, int &partIndexObj, KoXmlWriter* manifestWriter, QMap<QString, int> &pageNames ) const
{
    if ( isMasterPage() )
    {
        KoGenStyle pageLayout = m_doc->pageLayout().saveOasis();
        pageLayout.addAttribute( "style:page-usage", "all" ); // needed?
        QString pageLayoutName( context.mainStyles().lookup( pageLayout, "pm" ) );

        xmlWriter.startElement( "style:master-page" );
        xmlWriter.addAttribute( "style:name", "Standard" );
        xmlWriter.addAttribute( "style:page-layout-name", pageLayoutName );

        QString styleName = saveOasisPageStyle( store, context.mainStyles() );
        if ( !styleName.isEmpty() )
            xmlWriter.addAttribute( "draw:style-name", styleName );

        saveOasisObject( store, xmlWriter, context,  indexObj,partIndexObj, manifestWriter );

        xmlWriter.startElement( "style:header" );
        m_doc->header()->textObject()->saveOasisContent( xmlWriter, context );
        xmlWriter.endElement();

        xmlWriter.startElement( "style:footer" );
        m_doc->footer()->textObject()->saveOasisContent( xmlWriter, context );
        xmlWriter.endElement();

        xmlWriter.endElement();
    }
    else
    {
        //store use to save picture and co
        xmlWriter.startElement( "draw:page" );

        QString drawName( m_manualTitle );
        QRegExp rx( "^page[0-9]+$" );
        if ( drawName.isEmpty() || pageNames.contains( drawName ) || rx.search( drawName ) != -1 )
        {
            drawName = "page" + QString::number( posPage );
        }
        pageNames.insert( drawName, posPage );
        xmlWriter.addAttribute( "draw:name", drawName ); //we must store a name
        xmlWriter.addAttribute( "koffice:name", m_manualTitle ); // so that we can have the same name for different pages
        xmlWriter.addAttribute( "draw:id", "page" + QString::number( posPage ) );
        xmlWriter.addAttribute( "draw:master-page-name", "Standard"); //by default name of page is Standard

        QString styleName = saveOasisPageStyle( store, context.mainStyles() );
        kDebug()<<" styleName :"<<styleName<<endl;
        if ( !styleName.isEmpty() )
            xmlWriter.addAttribute( "draw:style-name", styleName );

        saveOasisObject( store, xmlWriter, context,  indexObj,partIndexObj, manifestWriter  );

        saveOasisNote( xmlWriter );
        xmlWriter.endElement();
    }
    return true;
}

QString KPrPage::saveOasisPageStyle( KoStore *, KoGenStyles& mainStyles ) const
{
    KoGenStyle stylepageauto( isMasterPage() ? KPrDocument::STYLE_BACKGROUNDPAGE : KPrDocument::STYLE_BACKGROUNDPAGEAUTO, "drawing-page" );

    if ( ! isMasterPage() )
    {
        stylepageauto.addProperty( "presentation:background-visible", ( m_displayBackground == true ) ? "true" : "false" );
        stylepageauto.addProperty( "presentation:background-objects-visible", ( m_displayObjectFromMasterPage == true ) ? "true" : "false" );
        QString transition = saveOasisPageEffect();
        if ( !transition.isEmpty() )
        {
            stylepageauto.addProperty( "presentation:transition-style", transition );
            QString additionalTransition = saveOasisAdditionalPageEffect();
            if ( !additionalTransition.isEmpty() )
            {
                stylepageauto.addProperty( "koffice:additional-transition-style", additionalTransition );    
            }
        }
        stylepageauto.addProperty( "presentation:display-header", hasHeader());
        stylepageauto.addProperty( "presentation:display-footer", hasFooter());

        if ( m_pageTimer != 1 )
        {
            stylepageauto.addProperty("presentation:duration", saveOasisTimer( m_pageTimer ));
            //not used into kpresenter but necessary into ooimpress
            //keep compatible
            stylepageauto.addProperty( "presentation:transition-type", "automatic" );
        }
        if ( m_pageEffectSpeed != ES_MEDIUM ) // we don't save the default value
        {
            if ( m_pageEffectSpeed == ES_FAST )
                stylepageauto.addProperty( "presentation:transition-speed", "fast" );
            else if ( m_pageEffectSpeed == ES_SLOW )
                stylepageauto.addProperty( "presentation:transition-speed", "slow" );
        }
        if ( !isSlideSelected() )
            stylepageauto.addProperty( "presentation:visibility", "hidden" );
        if ( !m_soundFileName.isEmpty() && m_soundEffect )
        {
            QBuffer buffer;
            buffer.open( QIODevice::WriteOnly );
            KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
            elementWriter.startElement( "presentation:sound" );
            elementWriter.addAttribute( "xlink:href", m_soundFileName );
            elementWriter.addAttribute( "xlink:type", "simple" );
            elementWriter.addAttribute( "xlink:show", "new" );
            elementWriter.addAttribute( "xlink:actuate", "onRequest");

            elementWriter.endElement();
            QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
            stylepageauto.addChildElement( "sound effect", elementContents );
        }
    }

    if ( ! m_useMasterBackground )
    {
        m_kpbackground->saveOasisBackgroundPageStyle( stylepageauto, mainStyles );
    }

    // todo return mainStyles.lookup( stylepageauto, "dp", isMasterPage() ? KoGenStyles::AutoStyleInStylesDotXml : KoGenStyles::NoFlag );
    return mainStyles.lookup( stylepageauto, "dp" );
}

bool KPrPage::saveOasisNote( KoXmlWriter &xmlWriter ) const
{
    if ( m_noteText.isEmpty() )
        return true;

    //todo : add size for draw:text-box otherwise we can't import into oo
    //<draw:text-box presentation:style-name="pr1" draw:text-style-name="P1" draw:layer="layout" svg:width="14.5cm" svg:height="11.408cm" svg:x="3.247cm" svg:y="14.126cm" presentation:class="notes">
    xmlWriter.startElement( "presentation:notes" );
    xmlWriter.startElement( "draw:frame" );
    //todo save style

    xmlWriter.startElement( "draw:text-box" );
    QStringList text = QStringList::split( "\n", m_noteText );
    for ( QStringList::Iterator it = text.begin(); it != text.end(); ++it ) {
        xmlWriter.startElement( "text:p" );
        xmlWriter.addTextNode( *it );
        xmlWriter.endElement();
    }
    xmlWriter.endElement();
    xmlWriter.endElement();
    xmlWriter.endElement();

    return true;
}

QString KPrPage::saveOasisPageEffect() const
{
    QString transition;
    switch( m_pageEffect )
    {
    case PEF_NONE:
        transition="none";
        break;
    case PEF_CLOSE_HORZ:
        transition="close-vertical";
        break;
    case PEF_CLOSE_VERT:
        transition="close-horizontal";
        break;
    case PEF_CLOSE_ALL:
        transition="close";
        break;
    case PEF_OPEN_HORZ:
        transition="open-vertical";
        break;
    case PEF_OPEN_VERT:
        transition="open-horizontal";
        break;
    case PEF_OPEN_ALL:
        transition="open";
        break;
    case PEF_INTERLOCKING_HORZ_1:
        transition="interlocking-horizontal-left";
        break;
    case PEF_INTERLOCKING_HORZ_2:
        transition="interlocking-horizontal-right";
        break;
    case PEF_INTERLOCKING_VERT_1:
        transition="interlocking-vertical-top";
        break;
    case PEF_INTERLOCKING_VERT_2:
        transition="interlocking-vertical-bottom";
        break;
    case PEF_SURROUND1:
        transition="spiralin-left";
        break;
    case PEF_FLY1:
        transition="fly-away";
        break;
    case PEF_BLINDS_HOR:
        transition="horizontal-stripes";
        break;
    case PEF_BLINDS_VER:
        transition="vertical-stripes";
        break;
    case PEF_BOX_IN:
        transition="fade-to-center";
        break;
    case PEF_BOX_OUT:
        transition="fade-from-center";
        break;
    case PEF_CHECKBOARD_ACROSS:
        transition="horizontal-checkerboard";
        break;
    case PEF_CHECKBOARD_DOWN:
        transition="vertical-checkerboard";
        break;
    case PEF_COVER_DOWN:
        transition="fade-from-top";
        break;
    case PEF_COVER_UP:
        transition="fade-from-bottom";
        break;
    case PEF_COVER_LEFT:
        transition="fade-from-right";
        break;
    case PEF_COVER_RIGHT:
        transition="fade-from-left";
        break;
    case PEF_COVER_LEFT_UP:
        transition="fade-from-lowerright";
        break;
    case PEF_COVER_LEFT_DOWN:
        transition="fade-from-upperright";
        break;
    case PEF_COVER_RIGHT_UP:
        transition="fade-from-lowerleft";
        break;
    case PEF_COVER_RIGHT_DOWN:
        transition="fade-from-upperleft";
        break;
    case PEF_UNCOVER_LEFT:
        transition="uncover-to-left";
        break;
    case PEF_UNCOVER_UP:
        transition="uncover-to-top";
        break;
    case PEF_UNCOVER_RIGHT:
        transition="uncover-to-right";
        break;
    case PEF_UNCOVER_DOWN:
        transition="uncover-to-bottom";
        break;
    case PEF_UNCOVER_LEFT_UP:
        transition="uncover-to-upperleft";
        break;
    case PEF_UNCOVER_LEFT_DOWN:
        transition="uncover-to-lowerleft";
        break;
    case PEF_UNCOVER_RIGHT_UP:
        transition="uncover-to-upperright";
        break;
    case PEF_UNCOVER_RIGHT_DOWN:
        transition="uncover-to-lowerright";
        break;
    case PEF_DISSOLVE:
        transition="dissolve";
        break;
    case PEF_STRIPS_LEFT_UP:
        transition="fade-from-lowerright";
        break;
    case PEF_STRIPS_LEFT_DOWN:
        transition="fade-from-upperright";
        break;
    case PEF_STRIPS_RIGHT_UP:
        transition="fade-from-lowerleft";
        break;
    case PEF_STRIPS_RIGHT_DOWN:
        transition="fade-from-upperleft";
        break;
    case PEF_MELTING:
        transition="melt";
        break;
    case PEF_LAST_MARKER://don't use it !!!
        break;
    case PEF_RANDOM:
        transition="random";
        break;

    }
    return transition;
}


QString KPrPage::saveOasisAdditionalPageEffect() const
{
    QString additionalEffect;
    switch( m_pageEffect )
    {
        case PEF_STRIPS_LEFT_UP:
            additionalEffect="stripe-from-lowerright";
            break;
        case PEF_STRIPS_LEFT_DOWN:
            additionalEffect="stripe-from-upperright";
            break;
        case PEF_STRIPS_RIGHT_UP:
            additionalEffect="stripe-from-lowerleft";
            break;
        case PEF_STRIPS_RIGHT_DOWN:
            additionalEffect="stripe-from-upperleft";
            break;
        default:
            break;
    }
    return additionalEffect;
}


KPrObject *KPrPage::getObject(int num)
{
    Q_ASSERT( num < (int)m_objectList.count() );
    return m_objectList.at(num);
}

/*
 * Check if object name already exists.
 */
bool KPrPage::objectNameExists( KPrObject *object, Q3PtrList<KPrObject> &list ) {
    Q3PtrListIterator<KPrObject> it( list );

    for ( it.toFirst(); it.current(); ++it ) {
        // object name can exist in current object.
        if ( it.current()->getObjectName() == object->getObjectName() &&
             it.current() != object ) {
            return true;
        }
        else if ( it.current()->getType() == OT_GROUP ) {
            Q3PtrList<KPrObject> objectList( static_cast<KPrGroupObject*>(it.current())->getObjects() );
            if ( objectNameExists( object, objectList ) ) {
                return true;
            }
        }
    }
    return false;
}

void KPrPage::unifyObjectName( KPrObject *object ) {
    if ( object->getObjectName().isEmpty() ) {
        object->setObjectName( object->getTypeString() );
    }
    QString objectName( object->getObjectName() );

    Q3PtrList<KPrObject> list( m_objectList );

    int count = 1;

    while ( objectNameExists( object, list ) ) {
        count++;
        QRegExp rx( " \\(\\d{1,}\\)$" );
        if ( rx.search( objectName ) != -1 ) {
            objectName.remove( rx );
        }
        objectName += QString(" (%1)").arg( count );
        object->setObjectName( objectName );
    }
}

void KPrPage::appendObject(KPrObject *_obj)
{
    unifyObjectName(_obj);
    m_objectList.append(_obj);
}

void KPrPage::appendObjects( const Q3ValueList<KPrObject *> &objects )
{
    QMap <QString, int> usedPageNames;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    // find the biggest number 
    for ( ; it.current() ; ++it )
    {
        QString objectName( it.current()->getObjectName() );

        QRegExp rx( "(.*) \\((\\d{1,})\\)$" );
        rx.setMinimal( true );
        if ( rx.search( objectName ) != -1 && rx.numCaptures() == 2 ) 
        {
            int id = rx.cap( 2 ).toInt();
            if ( usedPageNames[rx.cap( 1 )] < id )
            {
                usedPageNames[rx.cap( 1 )] = id;
            }
        }
        else
        {
            usedPageNames[objectName] = 1;
        }
    }

    Q3ValueListConstIterator<KPrObject *> oIt( objects.begin() );
    for ( ; oIt != objects.end(); ++oIt )
    {
        QString objectName( ( *oIt )->getObjectName() );
        QRegExp rx( " \\(\\d{1,}\\)$" );
        if ( rx.search( objectName ) != -1 ) 
        {
            objectName.remove( rx );
        }

        if ( usedPageNames.contains( objectName ) )
        {
            usedPageNames[objectName]++;
            objectName += QString(" (%1)").arg( usedPageNames[objectName] );
            ( *oIt )->setObjectName( objectName );
        }
        m_objectList.append( *oIt );
    }
}

int KPrPage::takeObject( KPrObject *object )
{
    int pos = m_objectList.findRef( object );
    m_objectList.take( pos );
    return pos;
}

void KPrPage::replaceObject( KPrObject *oldObject, KPrObject *newObject )
{
    //XXX check if object name gets set
    unsigned int pos = m_objectList.findRef( oldObject );
    m_objectList.take( pos );
    m_objectList.insert( pos, newObject );
}


void KPrPage::insertObject( KPrObject *object, int pos )
{
    unifyObjectName( object );
    m_objectList.insert( pos, object );
}

KCommand * KPrPage::deleteSelectedObjects()
{
    Q3PtrList<KPrObject> objects = getSelectedObjects( true );

    KPrDeleteCmd *deleteCmd=0L;

    if ( objects.count() > 0 ) {
        deleteCmd = new KPrDeleteCmd( i18n( "Delete Objects" ), objects, m_doc, this );
        deleteCmd->execute();
    }
    else
        m_doc->setModified(true);

    return deleteCmd ;
}

void KPrPage::copyObjs(QDomDocument &doc, QDomElement &presenter, Q3ValueList<KoPictureKey> & savePictures) const
{
    if ( !numSelected() )
        return;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            if ( it.current()->getType() == OT_PICTURE )
            {
                KoPictureKey key = static_cast<KPrPixmapObject *>( it.current() )->getKey();
                if ( !savePictures.contains( key ) )
                    savePictures.append( key );
            }
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( it.current()->getType() ));
            object.appendChild(it.current()->save( doc,0 ));
            presenter.appendChild(object);
        }
    }
}

void KPrPage::pasteObjs( const QByteArray & data,int nbCopy, double angle,
                         double increaseX, double increaseY, double moveX, double moveY )
{
    m_doc->deSelectAllObj();
    int num = m_objectList.count();
    if ( !data.size() )
        return;
    QBuffer buffer( data );
    KoStore* store = KoStore::createStore( &buffer, KoStore::Read );
    int nbNewObject = -1 ;
    KMacroCommand *macro = 0L;
    if ( !store->bad() )
    {
        if ( store->open( "root" ) )
        {
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument domDoc;
            if ( !domDoc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn ) )
            {
                kError (30003) << "Parsing Error! Aborting! (in KPrPage::PasteObj)" << endl
                                << "  Line: " << errorLine << " Column: " << errorColumn << endl
                                << "  Message: " << errorMsg << endl;
                delete store;
                return;
            }
            QDomElement topElem = domDoc.documentElement();

            for ( int i = 0 ; i < nbCopy ; i++ )
            {
                KCommand *cmd = m_doc->loadPastedObjs( domDoc.toString(),this );
                if (cmd )
                {
                    if ( !macro )
                        macro = new KMacroCommand( i18n("Paste Objects" ));
                    macro->addCommand( cmd );
                }
                if ( nbNewObject == -1 )
                    nbNewObject = m_objectList.count() - num;
            }

            m_doc->loadPictureMap( topElem );
            store->close();
            int nbNewPartObject = -1;
            int nbElementBefore = m_objectList.count();
            for ( int i = 0 ; i < nbCopy ; i++ )
            {
                m_doc->insertEmbedded( store, topElem, macro, this, num );
                if ( nbNewPartObject ==-1 )
                    nbNewObject = nbNewObject + (m_objectList.count() - nbElementBefore);
            }
        }
    }
    delete store;

    //move and select all new pasted in objects
    KPrObject *_tempObj;
    int i = 0;
    int mod = 1;
    for (_tempObj = m_objectList.at(num); _tempObj; _tempObj = m_objectList.next(),  i++ ) {
        if ( i >= nbNewObject )
            mod++;
        _tempObj->moveBy( moveX*(double)mod,moveY*(double)mod);
        _tempObj->setSelected( true );
        if ( angle == 0.0 || (increaseY == 0.0 && increaseX == 0.0))
            m_doc->repaint(_tempObj);
    }

    if ( angle != 0.0)
    {
        KCommand *cmd = rotateSelectedObjects(angle, true);
        if (cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Paste Objects" ));
            macro->addCommand( cmd );
        }
    }
    if ( increaseX != 0.0 || increaseY != 0.0 )
    {
        Q3PtrListIterator<KPrObject> it( m_objectList );
        for ( ; it.current() ; ++it )
        {
            if(it.current()->isSelected())
            {
                KCommand *cmd =new KPrResizeCmd( i18n("Resize"), KoPoint(0, 0),
                                              KoSize(increaseX, increaseY), it.current(), m_doc );
                if ( cmd )
                {
                    cmd->execute();
                    if ( !macro )
                        macro = new KMacroCommand( i18n("Paste Objects" ));
                    macro->addCommand( cmd );
                }
            }
        }
    }

    if (macro)
        m_doc->addCommand(macro);
    //update sidebar
    if ( angle == 0.0 || (increaseY == 0.0 && increaseX == 0.0) )
    {
        m_doc->updateSideBarItem( this );
    }

    m_doc->setModified(true);
}

KPrTextObject * KPrPage::textFrameSet ( unsigned int _num ) const
{
    unsigned int i = 0;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            if(i==_num)
                return static_cast<KPrTextObject*>(it.current());
            i++;
        }
    }
    return 0L;
}

int KPrPage::numSelected() const
{
    int num = 0;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            num++;
    }

    return num;
}

int KPrPage::numTextObject() const
{
    int num = 0;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
            num++;
    }

    return num;
}

KPrObject* KPrPage::getSelectedObj() const
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            return it.current();
    }
    return 0L;
}


Q3PtrList<KPrObject> KPrPage::getSelectedObjects( bool withoutHeaderFooter ) const
{
    Q3PtrList<KPrObject> objects;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected()
            && ( !withoutHeaderFooter
                 || it.current() != m_doc->header() && it.current()!= m_doc->footer() ) )
        {
            objects.append( it.current() );
        }
    }
    return objects;
}


void KPrPage::groupObjects()
{
    Q3PtrList<KPrObject> objects( getSelectedObjects( true ) );

    if ( objects.count() > 1 ) {
        KPrGroupObjCmd *groupObjCmd = new KPrGroupObjCmd( i18n( "Group Objects" ), objects, m_doc,this );
        m_doc->addCommand( groupObjCmd );
        groupObjCmd->execute();
    }
}

void KPrPage::ungroupObjects( KMacroCommand ** macro )
{
    Q3PtrList<KPrObject> objects( getSelectedObjects( true ) );
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        KPrObject *object = it.current();
        if ( object->getType() == OT_GROUP )
        {
            UnGroupObjCmd *cmd = new UnGroupObjCmd( i18n( "Ungroup Objects" ),
                (KPrGroupObject*)object, m_doc, this );
            cmd->execute();

            if ( !*macro )
                *macro = new KMacroCommand(i18n( "Ungroup Objects" ));
            (*macro)->addCommand( cmd );
        }
    }
}

void KPrPage::lowerObjs(bool backward)
{
    KPrObject *kpobject = 0;

    Q3PtrList<KPrObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
        _new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );
    bool createCmd = false;
    int insertPos = 0;
    for ( int i = 0; i < static_cast<int>( _new.count() ); i++ ) {
        kpobject = _new.at( i );
        if ( kpobject->isSelected() ) {
            if ( i == insertPos )
            {
                ++insertPos;
                continue;
            }
            createCmd = true;
            _new.take( i );
            if ( backward )
                _new.insert(qMax(i-1,0) ,  kpobject);
            else
                _new.insert( insertPos++, kpobject );
        }
    }
    if ( createCmd )
    {
        KPrLowerRaiseCmd *lrCmd = new KPrLowerRaiseCmd( i18n( "Lower Objects" ), m_objectList, _new, m_doc,this );
        lrCmd->execute();
        m_doc->addCommand( lrCmd );
    }

}

void KPrPage::raiseObjs(bool forward)
{
    KPrObject *kpobject = 0;

    Q3PtrList<KPrObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
        _new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );
    bool createCmd = false;
    int size = int( _new.count() );
    int insertPos = size - 1;
    for ( int i = size - 1; i >= 0; i-- )
    {
        kpobject = _new.at( i );
        if ( kpobject->isSelected() )
        {
            if ( i == insertPos )
            {
                --insertPos;
                continue;
            }
            createCmd = true;
            _new.take( i );
            if ( forward )
                _new.insert( qMin( i+1, size - 1),  kpobject);
            else
                _new.insert( insertPos--, kpobject );
        }
    }
    if ( createCmd )
    {
        KPrLowerRaiseCmd *lrCmd = new KPrLowerRaiseCmd( i18n( "Raise Objects" ), m_objectList, _new, m_doc,this );
        lrCmd->execute();
        m_doc->addCommand( lrCmd );
    }
}

KCommand *KPrPage::insertObject( const QString &name, KPrObject * object, const KoRect &r, bool addCommand )
{
    object->setOrig( r.x(), r.y() );
    object->setSize( r.width(), r.height() );
    object->setSelected( true );

    KPrInsertCmd *insertCmd = new KPrInsertCmd( name, object, m_doc, this );
    insertCmd->execute();
    if ( addCommand )
    {
        m_doc->addCommand( insertCmd );
        return 0L;
    }
    return insertCmd;
}

void KPrPage::insertLine( const KoRect &r, const KoPen & pen, LineEnd lb, LineEnd le, LineType lt )
{
    KPrLineObject *kplineobject = new KPrLineObject( pen, lb, le, lt );
    insertObject( i18n( "Insert Line" ), kplineobject, r );
}

void KPrPage::insertRectangle( const KoRect &r, const KoPen & pen, const QBrush &brush, FillType ft,
                               const QColor &g1, const QColor &g2,
                               BCType gt, int rndX, int rndY, bool unbalanced, int xfactor, int yfactor )
{
    KPrRectObject *kprectobject = new KPrRectObject( pen, brush, ft, g1, g2, gt, rndX, rndY,
                                                   unbalanced, xfactor, yfactor );
    insertObject( i18n( "Insert Rectangle" ), kprectobject, r );
}

void KPrPage::insertCircleOrEllipse( const KoRect &r, const KoPen &pen, const QBrush & brush, FillType ft,
                                     const QColor &g1, const QColor &g2,
                                     BCType gt, bool unbalanced, int xfactor, int yfactor )
{
    KPrEllipseObject *kpellipseobject = new KPrEllipseObject( pen, brush, ft, g1, g2, gt,
                                                            unbalanced, xfactor, yfactor );
    insertObject( i18n( "Insert Ellipse" ), kpellipseobject, r );
}

void KPrPage::insertPie( const KoRect &r, const KoPen &pen, const QBrush &brush, FillType ft,
                         const QColor &g1, const QColor &g2,
                         BCType gt, PieType pt, int _angle, int _len, LineEnd lb, LineEnd le,
                         bool unbalanced, int xfactor, int yfactor )
{
    KPrPieObject *kppieobject = new KPrPieObject( pen, brush, ft, g1, g2, gt, pt, _angle,
                                                _len, lb, le, unbalanced, xfactor, yfactor );
    insertObject( i18n( "Insert Pie/Arc/Chord" ), kppieobject, r );
}

KPrTextObject* KPrPage::insertTextObject( const KoRect& r, const QString&  text , KPrView * _view )
{
    KPrTextObject *kptextobject = new KPrTextObject( m_doc );
    insertObject( i18n( "Insert Textbox" ), kptextobject, r );
    //TODO create macro command : now insertObject can return KCommand
    //TODO fix repaint object
    if (  !text.isEmpty() )
    {
        KPrTextView *view =  kptextobject->createKPTextView( _view ? _view->getCanvas() : 0L );
        view->insertText( text );
        view->terminate();
        delete view;
        m_doc->repaint( kptextobject );
    }
    return kptextobject;
}

void KPrPage::insertAutoform( const KoRect &r, const KoPen &pen, const QBrush &brush, LineEnd lb, LineEnd le, FillType ft,
                              const QColor &g1, const QColor &g2, BCType gt, const QString &fileName, bool unbalanced,
                              int xfactor, int yfactor ){
    KPrAutoformObject *kpautoformobject = new KPrAutoformObject( pen, brush, fileName, lb, le, ft,
                                                               g1, g2, gt, unbalanced, xfactor, yfactor );
    insertObject( i18n( "Insert Autoform" ), kpautoformobject, r );
}

void KPrPage::insertFreehand( const KoPointArray &points, const KoRect &r, const KoPen &pen,
                              LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );
    KPrFreehandObject *kpfreehandobject = new KPrFreehandObject( points, size, pen, lb, le );
    insertObject( i18n( "Insert Freehand" ), kpfreehandobject, r );
}

void KPrPage::insertPolyline( const KoPointArray &points, const KoRect &r, const KoPen &pen,
                              LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );
    KPrPolylineObject *kppolylineobject = new KPrPolylineObject( points, size, pen, lb, le );
    insertObject( i18n( "Insert Polyline" ), kppolylineobject, r );
}

void KPrPage::insertQuadricBezierCurve( const KoPointArray &points, const KoPointArray &allPoints,
                                        const KoRect &r, const KoPen &pen,
                                        LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );

    KPrQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPrQuadricBezierCurveObject(
        points, allPoints, size, pen, lb, le );
    insertObject( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject, r );
}

void KPrPage::insertCubicBezierCurve( const KoPointArray &points, const KoPointArray &allPoints,
                                      const KoRect &r, const KoPen &pen,
                                      LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );

    KPrCubicBezierCurveObject *kpCubicBezierCurveObject = new KPrCubicBezierCurveObject( points, allPoints, size, pen, lb, le );
    insertObject( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject, r );
}

void KPrPage::insertPolygon( const KoPointArray &points, const KoRect &r, const KoPen &pen, const QBrush &brush, FillType ft,
                             const QColor &g1, const QColor &g2, BCType gt, bool unbalanced, int xfactor, int yfactor,
                             bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
{
    KoSize size( r.width(), r.height() );

    KPrPolygonObject *kpPolygonObject = new KPrPolygonObject( points, size, pen, brush, ft,
                                                            g1, g2, gt, unbalanced, xfactor, yfactor,
                                                            _checkConcavePolygon, _cornersValue, _sharpnessValue );
    insertObject( i18n( "Insert Polygon" ), kpPolygonObject, r );
}

void KPrPage::insertClosedLine( const KoPointArray &points, const KoRect &r, const KoPen &pen, const QBrush &brush,
                                FillType ft,const QColor &g1, const QColor &g2,
                                BCType gt, bool unbalanced, int xfactor, int yfactor, ToolEditMode _mode )
{
    QString _type;
    QString _name;
    if ( _mode == INS_CLOSED_FREEHAND ) {
        _type = i18n( "Closed Freehand" );
        _name = i18n( "Insert Closed Freehand" );
    }
    else if ( _mode == INS_CLOSED_POLYLINE ) {
        _type = i18n( "Closed Polyline" );
        _name = i18n( "Insert Closed Polyline" );
    }
    else if ( _mode == INS_CLOSED_QUADRICBEZIERCURVE ) {
        _type = i18n( "Closed Quadric Bezier Curve" );
        _name = i18n( "Insert Closed Quadric Bezier Curve" );
    }
    else if ( _mode == INS_CLOSED_CUBICBEZIERCURVE ) {
        _type = i18n( "Closed Cubic Bezier Curve" );
        _name = i18n( "Insert Closed Cubic Bezier Curve" );
    }

    KoSize size( r.width(), r.height() );

    KPrClosedLineObject *kpClosedLineObject = new KPrClosedLineObject( points, size, pen, brush, ft,
                                                                     g1, g2, gt, unbalanced, xfactor, yfactor, _type );
    insertObject( _name, kpClosedLineObject, r );
}

KPrPartObject* KPrPage::insertObject( const KoRect& _rect, KoDocumentEntry& _e )
{
    QString errorMsg;
    KoDocument* doc = _e.createDoc( &errorMsg, m_doc );
    if ( !doc )
        return NULL;

    if ( !doc->showEmbedInitDialog( 0 ) ) //TODO should really have parent
        return NULL;

    QRect r = QRect( (int)_rect.left(), (int)_rect.top(),
                     (int)_rect.width(), (int)_rect.height() );
    KPrChild* ch = new KPrChild( m_doc, doc, r );
    m_doc->insertObject( ch );

    KPrPartObject *kppartobject = new KPrPartObject( ch );
    insertObject( i18n( "Embed Object" ), kppartobject, _rect );

    QWidget::connect(ch, SIGNAL(changed(KoChild *)), kppartobject, SLOT(slot_changed(KoChild *)) );

    //emit sig_insertObject( ch, kppartobject );
    m_doc->repaint( false );

    return kppartobject;
}

KCommand* KPrPage::setPen( const KoPen &pen, LineEnd lb, LineEnd le, int flags )
{
    KoPenCmd * cmd = NULL;

    KoPenCmd::Pen _newPen( pen, lb, le);

    Q3PtrList<KPrObject> _objects;
    _objects.setAutoDelete( false );

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() )
        {
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() && flags ) {
        cmd = new KoPenCmd( i18n( "Apply Styles" ), _objects, _newPen, m_doc, this, flags );
        cmd->execute();
    }

    return cmd;
}

KCommand * KPrPage::setBrush( const QBrush &brush, FillType ft, const QColor &g1, const QColor &g2,
                              BCType gt, bool unbalanced, int xfactor, int yfactor, int flags )
{
    KPrBrushCmd * cmd = NULL;

    KPrBrushCmd::Brush _newBrush;

    _newBrush.brush = QBrush( brush );
    _newBrush.fillType = ft;
    _newBrush.gColor1 = g1;
    _newBrush.gColor2 = g2;
    _newBrush.gType = gt;
    _newBrush.unbalanced = unbalanced;
    _newBrush.xfactor = xfactor;
    _newBrush.yfactor = yfactor;

    Q3PtrList<KPrObject> _objects;
    _objects.setAutoDelete( false );

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() )
        {
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() && flags ) {
        cmd = new KPrBrushCmd( i18n( "Apply Styles" ), _objects, _newBrush, m_doc, this, flags );
        cmd->execute();
    }

    return cmd;
}

void KPrPage::slotRepaintVariable()
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_TEXT )
            m_doc->repaint( it.current() );
    }
}

void KPrPage::recalcPageNum()
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_TEXT ) {
            ( (KPrTextObject*)it.current() )->recalcPageNum( this );
            m_doc->repaint( it.current() ); // TODO only if something changed
        }
    }
}

void KPrPage::changePicture( const KUrl & url, QWidget *parent )
{
    // filename has been chosen in KPrView with a filedialog,
    // so we know it exists
    KoPicture image = m_doc->pictureCollection()->downloadPicture( url, parent );

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_PICTURE)
        {
            KPrPixmapObject* obj=dynamic_cast<KPrPixmapObject*>( it.current() );
            if( obj)
            {
                KPrPixmapObject *pix = new KPrPixmapObject( m_doc->pictureCollection(), image.getKey() );
                KPrChgPixCmd *chgPixCmd = new KPrChgPixCmd( i18n( "Change Pixmap" ),obj,pix, m_doc,this );
                chgPixCmd->execute();
                m_doc->addCommand( chgPixCmd );
            }
            break;
        }
    }
}

void KPrPage::insertPicture( const QString &filename, const KoPoint &pos )
{
    KoPictureKey key = m_doc->pictureCollection()->loadPicture( filename ).getKey();
    KPrPixmapObject *kppixmapobject = new KPrPixmapObject(m_doc->pictureCollection() , key );

    kppixmapobject->setOrig( pos );
    kppixmapobject->setSelected( true );

    kppixmapobject->setSize( m_doc->zoomHandler()->pixelXToPt( kppixmapobject->originalSize().width() ),
                             m_doc->zoomHandler()->pixelYToPt( kppixmapobject->originalSize().height() ) );
    kppixmapobject->setKeepRatio( true ); // pics have keep-aspect-ratio on by default.

    KPrInsertCmd *insertCmd = new KPrInsertCmd( i18n( "Insert Picture" ), kppixmapobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );

    KoRect s = getPageRect();
    float fakt = 1;
    if ( kppixmapobject->getSize().width() > s.width() )
        fakt = (float)s.width() / (float)kppixmapobject->getSize().width();
    if ( kppixmapobject->getSize().height() > s.height() )
        fakt = qMin( fakt, (float)s.height() / (float)kppixmapobject->getSize().height() );

    //kDebug(33001) << k_funcinfo << "Fakt: " << fakt << endl;

    if ( fakt < 1 ) {
        int w = (int)( fakt * (float)kppixmapobject->getSize().width() );
        int h = (int)( fakt * (float)kppixmapobject->getSize().height() );
        //kDebug(33001) << k_funcinfo << "Size: " << w << ", " << h << endl;
        kppixmapobject->setOrig(0,0);
        kppixmapobject->setSize(w, h);
        m_doc->repaint( false );
    }
}

void KPrPage::insertPicture( const QString &_file, const KoRect &_rect )
{
    KoPictureKey key = m_doc->pictureCollection()->loadPicture( _file ).getKey();
    KPrPixmapObject *kppixmapobject = new KPrPixmapObject( m_doc->pictureCollection(), key );
    kppixmapobject->setKeepRatio( true ); // pics have keep-aspect-ratio on by default.
    insertObject( i18n( "Insert Picture" ), kppixmapobject, _rect );
}

void KPrPage::enableEmbeddedParts( bool f )
{
    KPrPartObject *obj=0L;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_PART)
        {
            obj=dynamic_cast<KPrPartObject*>( it.current() );
            if(obj)
                obj->enableDrawing( f );
        }
    }
}


void KPrPage::setBackColor(const  QColor &backColor1, const QColor &backColor2, BCType bcType,
                           bool unbalanced, int xfactor, int yfactor )
{
    m_kpbackground->setBackColor1( backColor1 );
    m_kpbackground->setBackColor2( backColor2 );
    m_kpbackground->setBackColorType( bcType );
    m_kpbackground->setBackUnbalanced( unbalanced );
    m_kpbackground->setBackXFactor( xfactor );
    m_kpbackground->setBackYFactor( yfactor );
}

void KPrPage::setBackPicture( const KoPictureKey & key )
{
    m_kpbackground->setBackPicture( key );
}

bool KPrPage::getBackUnbalanced() const
{
    return m_kpbackground->getBackUnbalanced();
}

void KPrPage::setBackView( BackView backView )
{
    m_kpbackground->setBackView( backView );
}

void KPrPage::setBackType( BackType backType )
{
    m_kpbackground->setBackType( backType );
}

void KPrPage::setPageEffectSpeed( EffectSpeed pageEffectSpeed )
{
    m_pageEffectSpeed = pageEffectSpeed;
}

void KPrPage::setPageEffect( PageEffect pageEffect )
{
    m_pageEffect = pageEffect;
}

void KPrPage::setPageTimer( int pageTimer )
{
    m_pageTimer = pageTimer;
}

void KPrPage::setPageSoundEffect( bool soundEffect )
{
    m_soundEffect = soundEffect;
}

void KPrPage::setPageSoundFileName( const QString &fileName )
{
    m_soundFileName = fileName;
}

BackType KPrPage::getBackType() const
{
    return m_kpbackground->getBackType();
}

BackView KPrPage::getBackView() const
{
    return m_kpbackground->getBackView();
}

KoPictureKey KPrPage::getBackPictureKey() const
{
    return m_kpbackground->getBackPictureKey();
}

KoPicture KPrPage::getBackPicture() const
{
    return m_kpbackground->getBackPicture();
}

QColor KPrPage::getBackColor1() const
{
    return m_kpbackground->getBackColor1();
}

QColor KPrPage::getBackColor2() const
{
    return m_kpbackground->getBackColor2();
}

int KPrPage::getBackXFactor() const
{
    return m_kpbackground->getBackXFactor();
}

int KPrPage::getBackYFactor() const
{
    return m_kpbackground->getBackYFactor();
}

BCType KPrPage::getBackColorType() const
{
    return m_kpbackground->getBackColorType();
}

EffectSpeed KPrPage::getPageEffectSpeed() const
{
    return m_pageEffectSpeed;
}

PageEffect KPrPage::getPageEffect() const
{
    return m_pageEffect;
}

int KPrPage::getPageTimer() const
{
    return m_pageTimer;
}

bool KPrPage::getPageSoundEffect() const
{
    return m_soundEffect;
}

QString KPrPage::getPageSoundFileName() const
{
    return m_soundFileName;
}

KoRect KPrPage::getPageRect() const
{
    const KoPageLayout& p = m_doc->pageLayout();
    return KoRect( p.ptLeft, p.ptTop, p.ptWidth - p.ptRight - p.ptLeft, p.ptHeight - p.ptTop - p.ptBottom );
}

QRect KPrPage::getZoomPageRect() const
{
    return m_doc->zoomHandler()->zoomRect(getPageRect());
}

void KPrPage::completeLoading( bool _clean, int lastObj )
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        // Pictures and cliparts have been loaded from the store, we can now
        // get the picture from the collection, and set it in the image/clipart object
        if ( ( it.current()->getType() == OT_PICTURE )
             || ( it.current()->getType() == OT_CLIPART ) ) {
            if ( _clean || m_objectList.findRef( it.current() ) > lastObj )
            {
                KPrPixmapObject* obj=dynamic_cast<KPrPixmapObject*>( it.current());
                if(obj)
                    obj->reload();
            }
        }
        else if ( it.current()->getType() == OT_TEXT )
        {
            KPrTextObject*obj=dynamic_cast<KPrTextObject*>( it.current() );
            if(obj)
                obj->recalcPageNum( this );
        }
        else if ( it.current()->getType() == OT_GROUP )
            completeLoadingForGroupObject( it.current() );
    }
    m_kpbackground->reload();
}

void KPrPage::completeLoadingForGroupObject( KPrObject *_obj )
{
    KPrGroupObject *_groupObj = static_cast<KPrGroupObject*>( _obj );

    if ( _groupObj ) {
        Q3PtrListIterator<KPrObject> it( _groupObj->objectList() );
        for ( ; it.current(); ++it ) {
            if ( ( it.current()->getType() == OT_PICTURE )
                 || ( it.current()->getType() == OT_CLIPART ) ) {
                KPrPixmapObject *_pixObj = dynamic_cast<KPrPixmapObject*>( it.current() );
                if ( _pixObj )
                    _pixObj->reload();
            }
            else if ( it.current()->getType() == OT_TEXT ) {
                KPrTextObject *_textObj=  dynamic_cast<KPrTextObject*>( it.current() );
                if ( _textObj )
                    _textObj->recalcPageNum(  this );
            }
            else if ( it.current()->getType() == OT_GROUP )
                completeLoadingForGroupObject( it.current() ); // recursion
        }
    }
}

KCommand * KPrPage::replaceObjs( bool createUndoRedo, double _orastX,double _orastY,const QColor & _txtBackCol,
                                 const QColor & _otxtBackCol )
{
    KPrObject *kpobject = 0;
    double ox=0, oy=0;
    Q3PtrList<KPrObject> _objects;
    Q3ValueList<KoPoint> _diffs;
    _objects.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
        kpobject = m_objectList.at( i );
        ox = kpobject->getOrig().x();
        oy = kpobject->getOrig().y();
        ox = static_cast<int>(ox / m_doc->getGridX() * m_doc->getGridX());
        oy = static_cast<int>(oy / m_doc->getGridY() * m_doc->getGridY());

        _diffs.append( KoPoint( ox - kpobject->getOrig().x(), oy - kpobject->getOrig().y() ) );
        _objects.append( kpobject );
    }

    KPrSetOptionsCmd *setOptionsCmd = new KPrSetOptionsCmd( i18n( "Set New Options" ), _diffs, _objects, m_doc->getGridX(),
                                                      m_doc->getGridY(),_orastX, _orastY, _txtBackCol, _otxtBackCol, m_doc );
    if ( createUndoRedo )
        return setOptionsCmd;

    delete setOptionsCmd;
    return 0L;
}

QString KPrPage::manualTitle() const
{
    return m_manualTitle;
}

void KPrPage::insertManualTitle(const QString & title)
{
    m_manualTitle=title;
}

QString KPrPage::pageTitle( const QString &_title ) const
{
    // If a user sets a title with manual, return it.
    if ( !m_manualTitle.isEmpty() )
        return m_manualTitle;

    Q3PtrList<KPrTextObject> objs;

    // Create list of text objects in this page

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
            objs.append( static_cast<KPrTextObject*>( it.current() ) );
    }

    QString title;
    if ( _title.isNull() )
    {
        // MASTERPAGE
        if ( m_masterPage )
            title = i18n( "Slide %1", m_doc->pageList().findRef( this ) + 1 );
        else
            title = i18n( "Slide Master" );
    }
    else
    {
        title = _title;
    }

    if ( objs.isEmpty() )
        return title;

    // Find object most on top
    KPrTextObject *tmp = objs.first();
    KPrTextObject *textobject=tmp;
    for ( tmp = objs.next(); tmp; tmp = objs.next() )
        if ( tmp->getOrig().y() < textobject->getOrig().y() )
            textobject = tmp;

    // this can't happen, but you never know :- )
    if ( !textobject )
        return QString( title );

    QString txt;
    if ( textobject->textDocument()->firstParag() )
        txt = textobject->textDocument()->firstParag()->toString();
    if ( txt.trimmed().isEmpty() || txt=="\n" )
        return title;
    return txt;
}

void KPrPage::setNoteText( const QString &_text )
{
    m_noteText=_text;
    m_doc->setModified(true);
}

QString KPrPage::noteText() const
{
    return m_noteText;
}

void KPrPage::makeUsedPixmapList()
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->getType()==OT_PICTURE || it.current()->getType()==OT_CLIPART)
            m_doc->insertPixmapKey(static_cast<KPrPixmapObject*>( it.current() )->getKey() );
        else if ( it.current()->getType() == OT_GROUP )
            makeUsedPixmapListForGroupObject( it.current() );
    }

    if( m_kpbackground->getBackType()==BT_PICTURE || m_kpbackground->getBackType()==BT_CLIPART)
        m_doc->insertPixmapKey(m_kpbackground->getBackPictureKey());
}

void KPrPage::makeUsedPixmapListForGroupObject( KPrObject *_obj )
{
    KPrGroupObject *_groupObj = static_cast<KPrGroupObject*>( _obj );

    if ( _groupObj ) {
        Q3PtrListIterator<KPrObject> it( _groupObj->objectList() );
        for ( ; it.current(); ++it ) {
            if ( ( it.current()->getType() == OT_PICTURE )
                 || ( it.current()->getType() == OT_CLIPART ) )
                m_doc->insertPixmapKey(static_cast<KPrPixmapObject*>( it.current() )->getKey() );
            else if ( it.current()->getType() == OT_GROUP )
                makeUsedPixmapListForGroupObject( it.current() );  // recursion
        }
    }
}


Q3ValueList<int> KPrPage::getEffectSteps() const
{
    QMap<int,bool> stepmap;
    stepmap[0] = true;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current(); ++it )
    {
        stepmap[it.current()->getAppearStep()] = true;

        if ( it.current()->getDisappear() )
        {
            stepmap[it.current()->getDisappearStep()] = true;
        }
    }

    return stepmap.keys();
}


void KPrPage::deSelectAllObj()
{
    Q3PtrListIterator<KPrObject> sIt( m_objectList );
    for ( ; sIt.current() ; ++sIt )
    {
        if(sIt.current()->isSelected())
            deSelectObj(sIt.current() );
    }
}

void KPrPage::deSelectObj( KPrObject *kpobject )
{
    kpobject->setSelected( false );
    m_doc->repaint( kpobject );
}

QDomElement KPrPage::saveObjects( QDomDocument &doc, QDomElement &objects, double yoffset,
                                  int saveOnlyPage ) const
{
    Q3PtrListIterator<KPrObject> oIt(m_objectList);
    for (; oIt.current(); ++oIt )
    {
        //don't store header/footer (store in header/footer section)
        if ( oIt.current()==m_doc->header()
             || oIt.current()==m_doc->footer()
             ||  oIt.current()->getType() == OT_PART)
            continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( oIt.current()->getType() ));
        bool _sticky = ( this->m_masterPage == 0 );
        if (_sticky)
            object.setAttribute("sticky", static_cast<int>(_sticky));
        if ( saveOnlyPage != -1 )
            yoffset=0;
        //add yoffset to compatibility with koffice 1.1
        object.appendChild(oIt.current()->save( doc, yoffset ));

        objects.appendChild(object);
    }
    return objects;
}

bool KPrPage::oneObjectTextExist(bool forceAllTextObject)
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst,forceAllTextObject );
    Q3PtrListIterator<KPrObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if ( (it.current() == m_doc->header() && !hasHeader())
             || (it.current() == m_doc->footer() && !hasFooter()) )
            continue;
        if (it.current()->getType()==OT_TEXT)
            return true;
    }
    return false;
}

bool KPrPage::oneObjectTextSelected()
{
    return oneObjectTextExist(false);
}


bool KPrPage::isOneObjectSelected()
{
    return 0 != getSelectedObj();
}

bool KPrPage::haveASelectedPartObj()
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst );
    Q3PtrListIterator<KPrObject> it( lst );
    for ( ; it.current(); ++it ) {
        if (it.current()->getType() == OT_PART )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedGroupObj()
{
    Q3PtrList<KPrObject> objects( getSelectedObjects( true ) );
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_GROUP )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedPixmapObj()
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst );
    Q3PtrListIterator<KPrObject> it( lst );
    for ( ; it.current() ; ++it ) {
        if (( it.current()->getType() == OT_PICTURE )
            || ( it.current()->getType() == OT_CLIPART ) )
            return true;
    }
    return false;
}

KoRect KPrPage::getRealRect( bool all ) const
{
    KoRect rect;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if ( all || ( it.current()->isSelected() && ! it.current()->isProtect() ) )
            rect |= it.current()->getRealRect();
    }

    return rect;
}

//return true if we change picture
bool KPrPage::chPic( KPrView *_view)
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            KPrPixmapObject* obj=dynamic_cast<KPrPixmapObject*>( it.current() );
            if( obj)
            {
                _view->changePicture( obj->getFileName() );
                return true;
            }
        }
    }
    return false;
}

bool KPrPage::savePicture( KPrView *_view ) const
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            KPrPixmapObject* obj=dynamic_cast<KPrPixmapObject*>( it.current() );
            if( obj)
            {
                _view->savePicture( obj );
                return true;
            }
        }
    }
    return false;
}

// move object for releasemouseevent
KCommand *KPrPage::moveObject(KPrView *_view, double diffx, double diffy)
{
    bool createCommand=false;
    KPrMoveByCmd *moveByCmd=0L;
    Q3PtrList<KPrObject> _objects;
    _objects.setAutoDelete( false );
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current() == m_doc->header() || it.current() == m_doc->footer())
            continue;
        if ( it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
            QRect br = _view->zoomHandler()->zoomRect( it.current()->getRepaintRect() );
            br.moveBy( _view->zoomHandler()->zoomItX( diffx ), _view->zoomHandler()->zoomItY( diffy ) );
            m_doc->repaint( br ); // Previous position
            m_doc->repaint( it.current() ); // New position
            createCommand=true;
        }
    }
    if(createCommand) {
        moveByCmd = new KPrMoveByCmd( i18n( "Move Objects" ), KoPoint( diffx, diffy ),
                                   _objects, m_doc,this );

        m_doc->updateSideBarItem( this );
    }
    return moveByCmd;
}

KCommand *KPrPage::moveObject(KPrView *m_view,const KoPoint &_move,bool key)
{
    Q3PtrList<KPrObject> _objects;
    _objects.setAutoDelete( false );
    KPrMoveByCmd *moveByCmd=0L;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if ( it.current()->isSelected() && !it.current()->isProtect()) {

            QRect oldBoundingRect = m_view->zoomHandler()->zoomRect( it.current()->getRepaintRect() );

            it.current()->moveBy( _move );
            _objects.append( it.current() );
            m_doc->repaint( oldBoundingRect );
            m_doc->repaint( it.current() );
        }
    }

    if ( key && !_objects.isEmpty())
        moveByCmd = new KPrMoveByCmd( i18n( "Move Objects" ),
                                   KoPoint( _move ),
                                   _objects, m_doc,this );

    return moveByCmd;
}

void KPrPage::repaintObj()
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            m_doc->repaint(it.current() );
    }
}

KCommand *KPrPage::rotateSelectedObjects( float _newAngle, bool addAngle )
{
    KPrRotateCmd * cmd = NULL;

    Q3PtrList<KPrObject> _objects;
    _objects.setAutoDelete( false );

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer() )
            continue;
        if( it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() ) {
        cmd = new KPrRotateCmd( i18n( "Change Rotation" ), _newAngle, _objects, m_doc, addAngle );
        cmd->execute();
    }

    return cmd;
}


KCommand *KPrPage::shadowObj(ShadowDirection dir,int dist, const QColor &col)
{
    KPrShadowCmd *shadowCmd=0L;
    bool newShadow=false;
    Q3PtrList<KPrObject> _objects;
    Q3PtrList<KPrShadowCmd::ShadowValues> _oldShadow;
    KPrShadowCmd::ShadowValues _newShadow, *tmp;

    _objects.setAutoDelete( false );
    _oldShadow.setAutoDelete( false );

    _newShadow.shadowDirection = dir;
    _newShadow.shadowDistance = dist;
    _newShadow.shadowColor = col;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't shadow a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if ( it.current()->isSelected() ) {
            tmp = new KPrShadowCmd::ShadowValues;
            tmp->shadowDirection = it.current()->getShadowDirection();
            tmp->shadowDistance =it.current()->getShadowDistance();
            tmp->shadowColor = it.current()->getShadowColor();

            if(!newShadow &&( tmp->shadowDirection!=_newShadow.shadowDirection
                              || tmp->shadowDistance!=_newShadow.shadowDistance
                              || tmp->shadowColor!=_newShadow.shadowColor))
                newShadow=true;

            _oldShadow.append( tmp );
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() && newShadow ) {
        shadowCmd = new KPrShadowCmd( i18n( "Change Shadow" ),
                                   _oldShadow, _newShadow, _objects, m_doc );
        shadowCmd->execute();
    }
    else
    {
        _oldShadow.setAutoDelete( true );
        _oldShadow.clear();
    }
    return shadowCmd;
}

Q3PtrList<KoTextObject> KPrPage::allTextObjects() const
{
    Q3PtrList<KoTextObject> lst;
    addTextObjects( lst );
    return lst;
}

void KPrPage::addTextObjects(Q3PtrList<KoTextObject>& lst) const
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
        it.current()->addTextObjects( lst );
}

KPrObject * KPrPage::getCursor( const QPoint &pos )
{
    return getCursor( m_doc->zoomHandler()->unzoomPoint( pos ) );
}

KPrObject * KPrPage::getCursor(const KoPoint &pos )
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    KPrObject *kpobject = it.toLast();
    while ( kpobject ) {
        if ( kpobject->contains( pos ) && kpobject->isSelected() )
            return kpobject;
        kpobject = --it;
    }
    return 0L;
}

KPrObject* KPrPage::getObjectAt( const KoPoint &pos, bool withoutProtected ) const
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    KPrObject *o = 0;
    //tz Check first if there is a selected object at this pos!
    for ( int i = 0; i < 2; ++i )
    {
        o = it.toLast();
        while ( o ) {
            if ( o != m_doc->footer() || 
                 o != m_doc->header() || 
                 ( m_bHasFooter && o == m_doc->footer() ) || 
                 ( m_bHasHeader && o == m_doc->header() ) )
            {
                if ( ( o->isSelected() || i > 0 ) && 
                       o->contains( pos ) && !( o->isProtect() && withoutProtected ) )
                    return o;
            }
            o = --it;
        }
    }
    return 0L;
}

KPrPixmapObject * KPrPage::picViewOrigHelper() const
{
    KPrPixmapObject *obj=0L;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            obj=(KPrPixmapObject*)it.current();
            break;
        }
    }
    return obj;
}

void KPrPage::applyStyleChange( KoStyleChangeDefMap changed )
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    Q3PtrListIterator<KPrObject> it( lst );

    for ( ; it.current() ; ++it )
    {
        KPrTextObject *obj=dynamic_cast<KPrTextObject*>(it.current());
        if( obj)
            obj->applyStyleChange( changed );
    }
}

void KPrPage::reactivateBgSpellChecking(bool refreshTextObj)
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    Q3PtrListIterator<KPrObject> oIt( lst );

    for ( ; oIt.current() ; ++oIt )
    {
        if(oIt.current()->getType()==OT_TEXT)
        {
            static_cast<KPrTextObject*>( oIt.current() )->textObject()->setNeedSpellCheck(true);
            if(refreshTextObj)
                m_doc->repaint( oIt.current() );
        }
    }
}

bool KPrPage::canMoveOneObject() const
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected()&& !it.current()->isProtect())
            return true;
    }
    return false;
}

KCommand *KPrPage::alignVertical( VerticalAlignmentType _type )
{
    KMacroCommand *macro = 0L;
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst );
    Q3PtrListIterator<KPrObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            KPrTextObject *obj = dynamic_cast<KPrTextObject *>(it.current());
            if ( obj  && !obj->isProtectContent())
            {
                KPrChangeVerticalAlignmentCommand * cmd = new KPrChangeVerticalAlignmentCommand( i18n("Change Vertical Alignment"),
                                                                                                 obj, obj->verticalAlignment(),_type, m_doc);
                if ( !macro )
                    macro = new KMacroCommand( i18n("Change Vertical Alignment"));
                macro->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    return macro;
}

void KPrPage::changeTabStopValue ( double _tabStop )
{
    Q3PtrList<KPrObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    Q3PtrListIterator<KPrObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            KPrTextObject *obj = dynamic_cast<KPrTextObject *>(it.current());
            if ( obj )
            {
                obj->textDocument()->setTabStops( m_doc->zoomHandler()->ptToLayoutUnitPixX( _tabStop ));
                obj->layout();
                m_doc->repaint( obj );
            }
        }
    }
}

KPrObject *KPrPage::nextTextObject(KPrTextObject *obj)
{
    if ( m_objectList.count()==0 )
        return 0L;
    int pos = -1;
    if ( obj )
        pos = m_objectList.findNextRef(obj);

    if (pos != -1 )
    {
        KPrObject *frm=0L;
        for ( frm=m_objectList.at(pos); frm != 0; frm=m_objectList.next() )
        {
            KPrTextObject *newObj = frm->nextTextObject();
            if(newObj && newObj->nextTextObject()->textObject()->needSpellCheck())
                return frm;
        }
    }
    else
    {
        KPrObject *frm=0L;
        for ( frm=m_objectList.at(0); frm != 0; frm=m_objectList.next() )
        {
            KPrTextObject *newObj = frm->nextTextObject();
            if(newObj && newObj->nextTextObject()->textObject()->needSpellCheck())
                return frm;
        }
    }
    return 0L;
}

bool KPrPage::findTextObject( KPrObject *obj )
{
    return (m_objectList.findRef( obj )>=0 );
}

void KPrPage::getAllObjectSelectedList(Q3PtrList<KPrObject> &lst, bool force )
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
        it.current()->getAllObjectSelectedList( lst,force );
}

void KPrPage::getAllEmbeddedObjectSelected(Q3PtrList<KoDocumentChild> &embeddedObjects )
{
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PART )
            embeddedObjects.append( static_cast<KPrPartObject *>(it.current())->getChild() );
    }
}

KPrPixmapObject* KPrPage::getSelectedImage() const
{
    KPrPixmapObject *obj=0L;
    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            obj=dynamic_cast<KPrPixmapObject*>(it.current());
            break;
        }
    }
    return obj;
}

KCommand * KPrPage::setImageEffect(ImageEffect eff, QVariant param1, QVariant param2, QVariant param3)
{
    bool changed = false;
    KPrImageEffectCmd *imageEffectCmd = 0L;
    Q3PtrList<KPrObject> _objects;
    Q3PtrList<KPrImageEffectCmd::ImageEffectSettings> _oldSettings;
    KPrImageEffectCmd::ImageEffectSettings _newSettings, *tmp;

    _objects.setAutoDelete( false );
    _oldSettings.setAutoDelete( false );

    _newSettings.effect = eff;
    _newSettings.param1 = param1;
    _newSettings.param2 = param2;
    _newSettings.param3 = param3;

    Q3PtrListIterator<KPrObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_PICTURE && it.current()->isSelected()) {
            tmp = new KPrImageEffectCmd::ImageEffectSettings;
            tmp->effect = static_cast<KPrPixmapObject*>(it.current())->getImageEffect();
            tmp->param1 = static_cast<KPrPixmapObject*>(it.current())->getIEParam1();
            tmp->param2 = static_cast<KPrPixmapObject*>(it.current())->getIEParam2();
            tmp->param3 = static_cast<KPrPixmapObject*>(it.current())->getIEParam3();
            _oldSettings.append( tmp );
            _objects.append( it.current() );

            if( !changed && ( tmp->effect != _newSettings.effect
                              || tmp->param1 != _newSettings.param1
                              || tmp->param2 != _newSettings.param2
                              || tmp->param3 != _newSettings.param3 ) )
                changed = true;
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        imageEffectCmd = new KPrImageEffectCmd( i18n( "Change Image Effect" ), _oldSettings, _newSettings,
                                             _objects, m_doc );
        imageEffectCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return imageEffectCmd;
}

void KPrPage::setHeader( bool b, bool _updateDoc )
{
    m_bHasHeader = b;
    if ( _updateDoc )
        m_doc->setHeader(b);
}

void KPrPage::setFooter( bool b, bool _updateDoc )
{
    m_bHasFooter = b;
    if ( _updateDoc )
        m_doc->setFooter( b );
}

void KPrPage::setUseMasterBackground( bool useMasterBackground )
{
    m_useMasterBackground = useMasterBackground;
}

bool KPrPage::useMasterBackground() const
{
    return m_useMasterBackground;
}

bool KPrPage::isMasterPage() const
{
    return ( m_masterPage==0 );
}

void KPrPage::setDisplayObjectFromMasterPage( bool _b )
{
    m_displayObjectFromMasterPage = _b;
    m_doc->setDisplayObjectMasterPage( _b );
}

void KPrPage::setDisplayBackground( bool _b )
{
    m_displayBackground = _b;
    m_doc->setDisplayBackground( _b );
}


