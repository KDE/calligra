/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_common.h"
#include "kivio_connector_target.h"
#include "kivio_shape.h"
#include "kivio_shape_data.h"
#include "kivio_sml_stencil.h"
#include "kivio_sml_stencil_spawner.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"

#include <iostream.h>
#include <qdom.h>
#include <qfile.h>
#include <qiodevice.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qfileinfo.h>

#include <kdebug.h>

KivioSMLStencilSpawner::KivioSMLStencilSpawner( KivioStencilSpawnerSet *p )
    : KivioStencilSpawner( p ),
      m_pStencil(NULL)
{
    m_pStencil = new KivioSMLStencil();

    m_pTargets = new QList<KivioConnectorTarget>;
    m_pTargets->setAutoDelete(true);
}

KivioSMLStencilSpawner::~KivioSMLStencilSpawner()
{
    if( m_pStencil )
    {
        delete m_pStencil;
        m_pStencil = NULL;
    }

    if( m_pTargets )
    {
        delete m_pTargets;
        m_pTargets = NULL;
    }

    kdDebug() << "* SMLStencilSpawner %s "<< m_pInfo->title() << " deleted" << endl;
}

QDomElement KivioSMLStencilSpawner::saveXML( QDomDocument &doc )
{
    QDomElement spawnE = doc.createElement("KivioSMLStencilSpawner");

    XmlWriteString( spawnE, "filename", m_filename );

    return spawnE;
}

bool KivioSMLStencilSpawner::load( const QString &file )
{
    KivioConnectorTarget *pTarget;
    QDomDocument d("test");

    m_filename = QString(file);
    QFile f(file);

    if( f.open( IO_ReadOnly )==false )
    {
       kdDebug() << "KivioSMLStencilSpawner::load() - Error opening stencil: " << file << endl;
        return false;
    }

    d.setContent(&f);

    QDomElement root = d.documentElement();
    QDomElement e;
    QDomNode node = root.firstChild();
    QString nodeName;
    
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        if( nodeName.compare("KivioSMLStencilSpawnerInfo")==0 )
        {
            m_pInfo->loadXML( (const QDomElement)node.toElement() );
        }
        else if( nodeName.compare("KivioShape")==0 )
        {
            loadShape( node );
        }
        else if( nodeName.compare("Dimensions")==0 )
        {
            e = node.toElement();
            
            m_defWidth = XmlReadFloat( e, "w", 72.0f );
            m_defHeight = XmlReadFloat( e, "h", 72.0f );
        }
        else if( nodeName.compare("KivioConnectorTarget")==0 )
        {
            pTarget = new KivioConnectorTarget();
            pTarget->loadXML( (const QDomElement)node.toElement() );

            m_pStencil->m_pConnectorTargets->append( pTarget );
            m_pTargets->append(pTarget->duplicate());
//            m_pStencil->m_pOriginalConnectorTargets->append( pTarget->duplicate() );
        }
        else
        {
	   kdDebug() << "KivioSMLStencilSpawner::load() - Unknown node " << nodeName << endl;
        }
        
        node = node.nextSibling();    
    }

    // Now load the xpm
    QFileInfo finfo(file);
    QString xpmFile = finfo.dirPath(true) + "/" + finfo.baseName() + ".xpm";

    m_icon.load( xpmFile );

    f.close();
   
    return true;
}


/**
 * Loads a shape from an XML node.
 */
void KivioSMLStencilSpawner::loadShape( QDomNode &shapeNode )
{
    KivioShapeData::KivioShapeType t;
    KivioShape *pShape = NULL;
    QDomElement shapeElement = shapeNode.toElement();
    
    t = KivioShapeData::shapeTypeFromString( XmlReadString( shapeElement, "type", "None" ) );
    
    switch( t )
    {
        case KivioShapeData::kstNone:
            break;
            
        case KivioShapeData::kstArc:
            pShape = KivioShape::loadShapeArc( shapeElement );
            break;
        
        case KivioShapeData::kstPie:
            pShape = KivioShape::loadShapePie( shapeElement );
            break;
        
        case KivioShapeData::kstLineArray:
            pShape = KivioShape::loadShapeLineArray( shapeElement );
            break;
        
        case KivioShapeData::kstPolyline:
            pShape = KivioShape::loadShapePolyline( shapeElement );
            break;
        
        case KivioShapeData::kstPolygon:
            pShape = KivioShape::loadShapePolygon( shapeElement );
            break;
        
        case KivioShapeData::kstBezier:
            pShape = KivioShape::loadShapeBezier( shapeElement );
            break;
        
        case KivioShapeData::kstRectangle:
            pShape = KivioShape::loadShapeRectangle( shapeElement );
            break;
        
        case KivioShapeData::kstRoundRectangle:
            pShape = KivioShape::loadShapeRoundRectangle( shapeElement );
            break;
        
        case KivioShapeData::kstEllipse:
            pShape = KivioShape::loadShapeEllipse( shapeElement );
            break;
        
        case KivioShapeData::kstOpenPath:
            pShape = KivioShape::loadShapeOpenPath( shapeElement );
            break;
        
        case KivioShapeData::kstClosedPath:
            pShape = KivioShape::loadShapeClosedPath( shapeElement );
            break;
        
        case KivioShapeData::kstTextBox:
            pShape = KivioShape::loadShapeTextBox( shapeElement );
            break;

        default:
            break;        
    }

    if( pShape )
    {
        m_pStencil->m_pShapeList->append( pShape );
    }


}


/**
 * Returns a new stencil, with default width/height of the spawner settings.
*/
KivioStencil *KivioSMLStencilSpawner::newStencil()
{
    KivioStencil *pNewStencil = m_pStencil->duplicate();
    
    pNewStencil->setSpawner(this);
    
    pNewStencil->setDimensions( m_defWidth, m_defHeight );

    return pNewStencil;
}
