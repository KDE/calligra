/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TodoShapeFactory.h"

#include <klocale.h>

#include <KoProperties.h>

#include "TodoShape.h"
#include "TodoShapeConfigWidget.h"

TodoShapeFactory::TodoShapeFactory(QObject* parent) 
   : KoShapeFactory( parent, TODOSHAPEID,
                     i18n("Todo Shape") )
{
  setToolTip( i18n("A todo shape") );
  setIcon( "todoshape" );
  setOdfElementNames( "http://kde.org/braindump", QStringList( "todo" ) );
}

KoShape* TodoShapeFactory::createDefaultShape() const
{
  TodoShape* fooShape = new TodoShape();
  fooShape->setShapeId(TODOSHAPEID);
  // set defaults
  return fooShape;
}

KoShape* TodoShapeFactory::createShape(
                            const KoProperties* params ) const
{
  Q_UNUSED(params);
  TodoShape* fooShape = new TodoShape();
  fooShape->setShapeId(TODOSHAPEID);
  // use the params
  return fooShape;
}

bool TodoShapeFactory::supports(const KoXmlElement & e) const
{
  return ( e.localName() == "todo" && e.namespaceURI() == "http://kde.org/braindump" );
}

QList<KoShapeConfigWidgetBase*> TodoShapeFactory::createShapeOptionPanels()
{
  QList<KoShapeConfigWidgetBase*> answer;
  answer.append( new TodoShapeConfigWidget() );
  return answer;
}
