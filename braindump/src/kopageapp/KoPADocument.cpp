/* This file is part of the KDE project
   Copyright (C) 2006-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Thomas Zander <zander@kde.org>

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

#include "KoPADocument.h"

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoOdfStylesReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoOdfLoadingContext.h>
#include <KoOasisSettings.h>
#include <KoStoreDevice.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoXmlNS.h>
#include <KoDataCenter.h>

#include "Section.h"

#include "KoPACanvas.h"
#include "KoPAView.h"
#include "commands/KoPAPageDeleteCommand.h"

#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>

#include <typeinfo>

class KoPADocument::Private
{
public:
    bool rulersVisible;
    QMap<QString, KoDataCenter *>  dataCenterMap;
};

KoPADocument::KoPADocument( QWidget* parentWidget, QObject* parent, bool singleViewMode )
: KoDocument( parentWidget, parent, singleViewMode ), SectionGroup(0),
    d(new Private())
{

    // Ask every shapefactory to populate the dataCenterMap
    foreach(const QString & id, KoShapeRegistry::instance()->keys())
    {
        KoShapeFactory *shapeFactory = KoShapeRegistry::instance()->value(id);
        shapeFactory->populateDataCenterMap(d->dataCenterMap);
    }

    loadConfig();
}

KoPADocument::~KoPADocument()
{
    saveConfig();
    qDeleteAll( d->dataCenterMap );
    delete d;
}

void KoPADocument::paintContent( QPainter &painter, const QRect &rect)
{
  if(sections().isEmpty()) return;
  Section* page = sections()[0];
  Q_ASSERT( page );
// TODO what ?
//   QPixmap thumbnail = page->thumbnail( rect.size() );
//   painter.drawPixmap( rect, thumbnail );
}

bool KoPADocument::loadXML( const KoXmlDocument & doc, KoStore * )
{
    Q_UNUSED( doc );

    //Perhaps not necessary if we use filter import/export for old file format
    //only needed as it is in the base class will be removed.
    return true;
}

bool KoPADocument::loadOdf( KoOdfReadStore & odfStore )
{
  qFatal("Unimplemented");
  return false;
}

bool KoPADocument::completeLoading( KoStore* store )
{
    bool ok=true;
    foreach(KoDataCenter *dataCenter, d->dataCenterMap)
    {
        ok = ok && dataCenter->completeLoading(store);
    }
    return ok;
}

bool KoPADocument::saveOdf( SavingContext & documentContext )
{
  qFatal("Unimplemented");
  return false;
}

#if 0
KoPAPageBase* KoPADocument::pageByNavigation( KoPAPageBase * currentPage, KoPageApp::PageNavigation pageNavigation ) const
{
    const QList<KoPAPageBase*>& pages = dynamic_cast<KoPAMasterPage *>( currentPage ) ? d->masterPages : d->pages;

    Q_ASSERT( ! pages.isEmpty() );

    KoPAPageBase * newPage = currentPage;

    switch ( pageNavigation )
    {
        case KoPageApp::PageFirst:
            newPage = pages.first();
            break;
        case KoPageApp::PageLast:
            newPage = pages.last();
            break;
        case KoPageApp::PagePrevious:
        {
            int index = pages.indexOf( currentPage ) - 1;
            if ( index >= 0 )
            {
                newPage = pages.at( index );
            }
        }   break;
        case KoPageApp::PageNext:
            // fall through
        default:
        {
            int index = pages.indexOf( currentPage ) + 1;
            if ( index < pages.size() )
            {
                newPage = pages.at( index );
            }
            break;
        }
    }

    return newPage;
}
#endif

void KoPADocument::addShape( KoShape * shape )
{
#if 0
  if(!shape)
      return;

  // the KoShapeController sets the active layer as parent
  KoPAPageBase * page( pageByShape( shape ) );

  foreach( KoView *view, views() )
  {
      KoPAView * kopaView = static_cast<KoPAView*>( view );
      kopaView->viewMode()->addShape( shape );
  }

  emit shapeAdded( shape );

  // it can happen in kpresenter notes view that there is no page
  if ( page ) {
      page->shapeAdded( shape );
      postAddShape( page, shape );
  }
#endif
}

void KoPADocument::removeShape( KoShape *shape )
{
#if 0
    if(!shape)
        return;

    KoPAPageBase * page( pageByShape( shape ) );

    foreach( KoView *view, views() )
    {
        KoPAView * kopaView = static_cast<KoPAView*>( view );
        kopaView->viewMode()->removeShape( shape );
    }

    emit shapeRemoved( shape );

    page->shapeRemoved( shape );
    postRemoveShape( page, shape );
#endif
}

QMap<QString, KoDataCenter *> KoPADocument::dataCenterMap() const
{
    return d->dataCenterMap;
}

void KoPADocument::loadConfig()
{
    KSharedConfigPtr config = componentData().config();

    if( config->hasGroup( "Grid" ) )
    {
        KoGridData defGrid;
        KConfigGroup configGroup = config->group( "Grid" );
        bool showGrid = configGroup.readEntry<bool>( "ShowGrid", defGrid.showGrid() );
        gridData().setShowGrid(showGrid);
        bool snapToGrid = configGroup.readEntry<bool>( "SnapToGrid", defGrid.snapToGrid() );
        gridData().setSnapToGrid(snapToGrid);
        qreal spacingX = configGroup.readEntry<qreal>( "SpacingX", defGrid.gridX() );
        qreal spacingY = configGroup.readEntry<qreal>( "SpacingY", defGrid.gridY() );
        gridData().setGrid( spacingX, spacingY );
        QColor color = configGroup.readEntry( "Color", defGrid.gridColor() );
        gridData().setGridColor( color );
    }

    if( config->hasGroup( "Interface" ) )
    {
        KConfigGroup configGroup = config->group( "Interface" );
        bool showRulers = configGroup.readEntry<bool>( "ShowRulers", true);
        setRulersVisible(showRulers);
    }
}

void KoPADocument::saveConfig()
{
    KSharedConfigPtr config = componentData().config();
    KConfigGroup configGroup = config->group( "Grid" );
    KoGridData defGrid;

    bool showGrid = gridData().showGrid();
    if ((showGrid == defGrid.showGrid()) && !configGroup.hasDefault("ShowGrid"))
        configGroup.revertToDefault("ShowGrid");
    else
        configGroup.writeEntry("ShowGrid", showGrid);

    bool snapToGrid = gridData().snapToGrid();
    if ((snapToGrid == defGrid.snapToGrid()) && !configGroup.hasDefault("SnapToGrid"))
        configGroup.revertToDefault("SnapToGrid");
    else
        configGroup.writeEntry("SnapToGrid", snapToGrid);

    qreal spacingX = gridData().gridX();
    if ((spacingX == defGrid.gridX()) && !configGroup.hasDefault("SpacingX"))
        configGroup.revertToDefault("SpacingX");
    else
        configGroup.writeEntry("SpacingX", spacingX);

    qreal spacingY = gridData().gridY();
    if ((spacingY == defGrid.gridY()) && !configGroup.hasDefault("SpacingY"))
        configGroup.revertToDefault("SpacingY");
    else
        configGroup.writeEntry("SpacingY", spacingY);

    QColor color = gridData().gridColor();
    if ((color == defGrid.gridColor()) && !configGroup.hasDefault("Color"))
        configGroup.revertToDefault("Color");
    else
        configGroup.writeEntry("Color", color);

    configGroup = config->group( "Interface" );

    bool showRulers = rulersVisible();
    if ((showRulers == true) && !configGroup.hasDefault("ShowRulers"))
        configGroup.revertToDefault("ShowRulers");
    else
        configGroup.writeEntry("ShowRulers", showRulers);
}

void KoPADocument::setRulersVisible(bool visible)
{
    d->rulersVisible = visible;
}

bool KoPADocument::rulersVisible() const
{
    return d->rulersVisible;
}

void KoPADocument::insertIntoDataCenterMap(QString key, KoDataCenter *dc)
{
    d->dataCenterMap[key] = dc;
}

void KoPADocument::sectionAdded(Section* page)
{
  emit(sigSectionAdded(page));
}

void KoPADocument::sectionRemoved(Section* page)
{
  emit(sigSectionRemoved(page));
}

#include "KoPADocument.moc"
