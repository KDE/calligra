/*
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
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

#include "ComicBoxesShapeFactory.h"

#include <klocale.h>

#include <KoProperties.h>

#include "ComicBoxesShape.h"

ComicBoxesShapeFactory::ComicBoxesShapeFactory() 
   : KoShapeFactoryBase( COMICBOXESSHAPEID,
                     i18n("Comic Boxes Shape") )
{
  setToolTip( i18n("A comic boxes shape") );
  setIcon( "comicboxes" );
  setOdfElementNames( "http://calligra-suite.org/", QStringList( "comicboxes" ) );
}

KoShape *ComicBoxesShapeFactory::createDefaultShape(KoResourceManager */*documentResources*/ ) const
{
  ComicBoxesShape* fooShape = new ComicBoxesShape();
  fooShape->setShapeId(COMICBOXESSHAPEID);
  // set defaults
  return fooShape;
}

KoShape *ComicBoxesShapeFactory::createShape(const KoProperties *params, KoResourceManager */*documentResources*/ ) const
{
  Q_UNUSED(params);
  ComicBoxesShape* fooShape = new ComicBoxesShape();
  fooShape->setShapeId(COMICBOXESSHAPEID);
  // use the params
  return fooShape;
}

bool ComicBoxesShapeFactory::supports(const KoXmlElement& e, KoShapeLoadingContext& /*context*/) const
{
  return ( e.localName() == "comicboxes" && e.namespaceURI() == "http://calligra-suite.org/" );
}

QList<KoShapeConfigWidgetBase*> ComicBoxesShapeFactory::createShapeOptionPanels()
{
  QList<KoShapeConfigWidgetBase*> answer;
  return answer;
}
