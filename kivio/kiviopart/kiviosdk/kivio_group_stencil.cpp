/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2004 theKompany.com & Dave Marotti
 *                         Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_group_stencil.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_layer.h"

#include <kdebug.h>

#include <KoGlobal.h>

KivioGroupStencil::KivioGroupStencil()
    : KivioStencil(),
    m_pGroupList(NULL)
{
    m_pGroupList = new QPtrList<KivioStencil>;
    m_pGroupList->setAutoDelete(true);

    m_x = m_y = 1000000000000.0f;
    m_w = m_h = -10000000000.0f;
    setType(kstGroup);
}

KivioGroupStencil::~KivioGroupStencil()
{
    if( m_pGroupList )
    {
        delete m_pGroupList;
        m_pGroupList = NULL;
    }
}


void KivioGroupStencil::paint( KivioIntraStencilData *pData )
{
    // Draw the group
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->paint(pData);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::paintOutline( KivioIntraStencilData *pData )
{
    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->paintOutline(pData);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::paintConnectorTargets( KivioIntraStencilData *pData )
{
    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->paintConnectorTargets(pData);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::setFGColor( QColor c )
{
    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setFGColor(c);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::setBGColor( QColor c )
{
    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setBGColor(c);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::setLineWidth( double f )
{
    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setLineWidth(f);

        pStencil = m_pGroupList->next();
    }
}

KivioCollisionType KivioGroupStencil::checkForCollision( KoPoint *p, double threshold )
{
    KivioCollisionType colType;

    // Draw the group in outline mode
    KivioStencil *pStencil = m_pGroupList->last();
    while( pStencil )
    {
        colType = pStencil->checkForCollision( p, threshold );
        if( colType != kctNone && pStencil->type()!=kstConnector) {
            return colType;
        }

        pStencil = m_pGroupList->prev();
    }

    return kctNone;
}

void KivioGroupStencil::addToGroup( KivioStencil *pStencil )
{
    double left, right, top, bottom;


    m_pGroupList->append(pStencil);

    // Special case it
    if( m_pGroupList->count() == 1 )
    {
        m_x = pStencil->x();
        m_y = pStencil->y();
        m_w = pStencil->w();
        m_h = pStencil->h();
    }
    else
    {
       right = pStencil->x() + pStencil->w();
       left = pStencil->x();
       top = pStencil->y();
       bottom = pStencil->y() + pStencil->h();

       // Adjust the borders to new limits
       if( left < m_x )
       {
	  m_w = m_w + (m_x - left);
	  m_x = left;
       }
       if( right > m_x + m_w )
       {
	  m_w = right - m_x;
       }
       if( top < m_y )
       {
	  m_h = m_h + (m_y - top);
	  m_y = top;
       }
       if( bottom > m_y + m_h )
       {
	  m_h = bottom - m_y;
       }
    }
}

KivioStencil *KivioGroupStencil::duplicate()
{
    KivioGroupStencil *pGroup;
    KivioStencil *pStencil;

    pGroup = new KivioGroupStencil();

    pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pGroup->addToGroup( pStencil->duplicate() );

        pStencil = m_pGroupList->next();
    }

    *(pGroup->protection()) = *m_pProtection;

    return pGroup;
}

bool KivioGroupStencil::loadXML( const QDomElement &e, KivioLayer *pLayer )
{
    QDomNode node;
    KivioStencil *pStencil;

    node = e.firstChild();
    while( !node.isNull() )
    {
        QString name = node.nodeName();

        if( name == "KivioGroupStencil" )
        {
            pStencil = pLayer->loadGroupStencil( node.toElement() );
            if( pStencil )
            {
                addToGroup( pStencil );
            }
            else
            {
	       kDebug(43000) << "KivioGroupStencil::loadXML() - Error loading group stencil" << endl;
            }
        }
        else if( name == "KivioSMLStencil" || name == "KivioPyStencil" )
        {
            pStencil = pLayer->loadSMLStencil( node.toElement() );
            if( pStencil )
            {
                addToGroup( pStencil );
            }
            else
            {
	       kDebug(43000) << "KivioGroupStencil::loadXML() - Error loading group stencil" << endl;
            }
        }
        else if( name == "KivioPluginStencil" )
        {
            KivioStencil *pStencil = pLayer->loadPluginStencil( node.toElement() );
            if( pStencil )
            {
                addToGroup( pStencil );
            }
            else
            {
	       kDebug(43000) << "KivioGroupStencil::loadXML() - Error loading group stencil" << endl;
            }
        }

        node = node.nextSibling();
    }

    return true;
}

QDomElement KivioGroupStencil::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioGroupStencil");

    QDomElement stencilE;
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        stencilE = pStencil->saveXML( doc );

        e.appendChild( stencilE );

        pStencil = m_pGroupList->next();
    }

    return e;
}

void KivioGroupStencil::setX( double newX )
{
  double dx = newX - m_x;

  m_x = newX;
  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if( pStencil->protection()->at(kpX)==false )
    {
      pStencil->setX( pStencil->x() + dx );
    }

    pStencil = m_pGroupList->next();
  }

}

void KivioGroupStencil::setY( double newY )
{
  double dy = newY - m_y;

  m_y = newY;
  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if( pStencil->protection()->at(kpY)==false )
    {
      pStencil->setY( pStencil->y() + dy );
    }

    pStencil = m_pGroupList->next();
  }

}

void KivioGroupStencil::setPosition( double newX, double newY )
{
  double dx = newX - m_x;
  double dy = newY - m_y;

  double newX2, newY2;

  m_x = newX;
  m_y = newY;

  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if(((pStencil->type() == kstConnector) && !pStencil->connected()) ||
      pStencil->type() != kstConnector)
    {
      if( pStencil->protection()->at(kpX)==false ) {
        newX2 = pStencil->x() + dx;
      } else {
        newX2 = pStencil->x();
      }

      if( pStencil->protection()->at(kpY)==false ) {
        newY2 = pStencil->y() + dy;
      } else {
        newY2 = pStencil->y();
      }

      pStencil->setPosition( newX2, newY2 );
    }

    pStencil = m_pGroupList->next();
  }

}

void KivioGroupStencil::setW( double newW )
{
  double percInc = newW / m_w;

  if( newW > 0.0f ) {
    m_w = newW;
  }

  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if(((pStencil->type() == kstConnector) && !pStencil->connected()) ||
      pStencil->type() != kstConnector)
    {
      if( pStencil->protection()->at(kpX)==false ) {
        pStencil->setX( ((pStencil->x() - m_x) * percInc) + m_x );
      }

      if( pStencil->protection()->at(kpWidth)==false ) {
        pStencil->setW( pStencil->w() * percInc );
      }
    }

    pStencil = m_pGroupList->next();
  }
}

void KivioGroupStencil::setH( double newH )
{
  double percInc = newH / m_h;

  if( newH > 0.0f ) {
    m_h = newH;
  }

  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if(((pStencil->type() == kstConnector) && !pStencil->connected()) ||
      pStencil->type() != kstConnector)
    {
      if( pStencil->protection()->at(kpY)==false ) {
        pStencil->setY( ((pStencil->y() - m_y) * percInc) + m_y );
      }

      if( pStencil->protection()->at(kpHeight)==false ) {
        pStencil->setH( pStencil->h() * percInc );
      }
    }

    pStencil = m_pGroupList->next();
  }
}

void KivioGroupStencil::setDimensions( double newW, double newH )
{
  double percIncX = newW / m_w;
  double percIncY = newH / m_h;

  if( newW > 0.0f ) {
    m_w = newW;
  }
  if( newH > 0.0f ) {
    m_h = newH;
  }

  KivioStencil *pStencil = m_pGroupList->first();
  while( pStencil )
  {
    if(((pStencil->type() == kstConnector) && !pStencil->connected()) ||
      pStencil->type() != kstConnector)
    {
      if( newW > 0.0f ) {
        if( pStencil->protection()->at(kpX)==false ) {
          pStencil->setX( ((pStencil->x() - m_x) * percIncX) + m_x );
        }
        if( pStencil->protection()->at(kpWidth)==false ) {
          pStencil->setW( pStencil->w() * percIncX );
        }
      }

      if( newH > 0.0f ) {
        if( pStencil->protection()->at(kpY)==false ) {
          pStencil->setY( ((pStencil->y() - m_y) * percIncY) + m_y );
        }
        if( pStencil->protection()->at(kpHeight)==false ) {
          pStencil->setH( pStencil->h() * percIncY );
        }
      }
    }

    pStencil = m_pGroupList->next();
  }

}

int KivioGroupStencil::generateIds( int next )
{
    KivioStencil *pStencil = m_pGroupList->first();

    while( pStencil )
    {
        next = pStencil->generateIds( next );

        pStencil = m_pGroupList->next();
    }

    return next;
}


KivioConnectorTarget *KivioGroupStencil::connectToTarget( KivioConnectorPoint *p, double thresh)
{
    KivioConnectorTarget *pTarget;

    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pTarget = pStencil->connectToTarget( p, thresh );
        if( pTarget )
            return pTarget;

        pStencil = m_pGroupList->next();
    }

    return NULL;
}

KivioConnectorTarget *KivioGroupStencil::connectToTarget( KivioConnectorPoint *p, int id )
{
    KivioConnectorTarget *pTarget;

    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pTarget = pStencil->connectToTarget( p, id );
        if( pTarget )
            return pTarget;

        pStencil = m_pGroupList->next();
    }

    return NULL;
}

void KivioGroupStencil::searchForConnections( KivioPage *p )
{
    KivioStencil *pCur = 0;
    KivioStencil *pStencil = m_pGroupList->first();

    while( pStencil )
    {
        // Backup the current list position
        pCur = pStencil;

        pStencil->searchForConnections( p );

        // Restore it
        m_pGroupList->find( pCur );

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::setTextColor( QColor c )
{
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setTextColor(c);

        pStencil = m_pGroupList->next();
    }
}

void KivioGroupStencil::setText( const QString &text )
{
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setText(text);

        pStencil = m_pGroupList->next();
    }
}

QString KivioGroupStencil::text()
{
    KivioStencil *pStencil = m_pGroupList->first();

    if( !pStencil )
        return QString("");

    return pStencil->text();
}

void KivioGroupStencil::setHTextAlign( int a )
{
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setHTextAlign(a);

        pStencil = m_pGroupList->next();
    }
}

int KivioGroupStencil::hTextAlign()
{
    KivioStencil *pStencil = m_pGroupList->first();

    if( !pStencil )
        return Qt::AlignHCenter;

    return pStencil->hTextAlign();
}

void KivioGroupStencil::setVTextAlign( int a )
{
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setVTextAlign(a);

        pStencil = m_pGroupList->next();
    }
}

int KivioGroupStencil::vTextAlign()
{
    KivioStencil *pStencil = m_pGroupList->first();

    if( !pStencil )
        return Qt::AlignVCenter;

    return pStencil->vTextAlign();
}

void KivioGroupStencil::setTextFont( const QFont &f )
{
    KivioStencil *pStencil = m_pGroupList->first();
    while( pStencil )
    {
        pStencil->setTextFont(f);

        pStencil = m_pGroupList->next();
    }
}

QFont KivioGroupStencil::textFont()
{
  KivioStencil *pStencil = m_pGroupList->first();

  if( !pStencil )
    return KoGlobal::defaultFont();

  return pStencil->textFont();
}

QColor KivioGroupStencil::textColor()
{
  KivioStencil *pStencil = m_pGroupList->first();

  if( !pStencil )
    return QColor(0, 0, 0);

  return pStencil->textColor();
}

int KivioGroupStencil::resizeHandlePositions()
{
    return KIVIO_RESIZE_HANDLE_POSITION_ALL;
}

QString KivioGroupStencil::getTextBoxName(const KoPoint& p)
{
  int id = checkForCollision(p);
  
  if(id < 0) {
    return QString::null;
  }
  
  KivioStencil* pStencil = m_pGroupList->at(id);
  QString name = QString::number(id) + "-" + pStencil->getTextBoxName(p);
  return name;
}

void KivioGroupStencil::setText(const QString& text, const QString& name)
{
  int id = name.section("-", 0, 0).toInt();
  QString n = name.section("-", 1);
  
  m_pGroupList->at(id)->setText(text, n);
}

QString KivioGroupStencil::text(const QString& name)
{
  int id = name.section("-", 0, 0).toInt();
  QString n = name.section("-", 1);
  
  return m_pGroupList->at(id)->text(n);
}

int KivioGroupStencil::checkForCollision(const KoPoint& p)
{
  KivioStencil *pStencil = m_pGroupList->first();
  KoPoint pos = p;
  int id = 0;
  
  while(pStencil)
  {
    if(pStencil->checkForCollision(&pos, 4.0) != kctNone) {
      return id;
    }
    
    pStencil = m_pGroupList->next();
    id++;
  }
  
  return -1;
}

bool KivioGroupStencil::hasTextBox() const
{
  KivioStencil *pStencil = m_pGroupList->first();

  while(pStencil)
  {
    if(pStencil->hasTextBox()) {
      return true;
    }

    pStencil = m_pGroupList->next();
  }

  return false;
}

QColor KivioGroupStencil::textColor(const QString& textBoxName)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  return m_pGroupList->at(id)->textColor(n);
}

void KivioGroupStencil::setTextColor(const QString& textBoxName, const QColor& color)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  m_pGroupList->at(id)->setTextColor(n, color);
}

QFont KivioGroupStencil::textFont(const QString& textBoxName)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  return m_pGroupList->at(id)->textFont(n);
}

void KivioGroupStencil::setTextFont(const QString& textBoxName, const QFont& font)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  m_pGroupList->at(id)->setTextFont(n, font);
}

int KivioGroupStencil::hTextAlign(const QString& textBoxName)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  return m_pGroupList->at(id)->hTextAlign(n);
}

int KivioGroupStencil::vTextAlign(const QString& textBoxName)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  return m_pGroupList->at(id)->vTextAlign(n);
}

void KivioGroupStencil::setHTextAlign(const QString& textBoxName, int align)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  m_pGroupList->at(id)->setHTextAlign(n, align);
}

void KivioGroupStencil::setVTextAlign(const QString& textBoxName, int align)
{
  int id = textBoxName.section("-", 0, 0).toInt();
  QString n = textBoxName.section("-", 1);

  m_pGroupList->at(id)->setVTextAlign(n, align);
}
