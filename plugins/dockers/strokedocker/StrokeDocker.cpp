/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <t.zachmann@zagge.de>

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

#include "StrokeDocker.h"

#include <KoStrokeConfigWidget.h>

#include <QLabel>
#include <QRadioButton>
#include <QWidget>
#include <QGridLayout>
#include <QButtonGroup>

#include <klocale.h>
#include <kiconloader.h>

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoResourceManager.h>
#include <KoDockFactoryBase.h>
//#include <KoUnitDoubleSpinBox.h>
//#include <KoLineStyleSelector.h>
#include <KoShapeManager.h>
#include <KoShapeBorderCommand.h>
#include <KoShapeBorderModel.h>
#include <KoSelection.h>
#include <KoLineBorder.h>
#include "KoMarkerSelector.h"


#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <QBuffer>
#include <KoMarker.h>

class StrokeDocker::Private
{
public:
    Private()
        : canvas(0)
        , mainWidget(0)
    {}

    KoLineBorder border;
    KoCanvasBase *canvas;
    KoStrokeConfigWidget *mainWidget;
#if 0
    KoMarkerSelector *markerSelector;
#endif
};


StrokeDocker::StrokeDocker()
    : d( new Private() )
{
    setWindowTitle( i18n( "Stroke Properties" ) );

#if 0
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;


    QBuffer xmldevice;
    xmldevice.open( QIODevice::WriteOnly );
    QTextStream xmlstream( &xmldevice );

    xmlstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xmlstream << "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:koffice=\"http://www.koffice.org/2005/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
    xmlstream << "<draw:marker draw:name=\"Arrow\" svg:viewBox=\"0 0 20 30\" svg:d=\"m10 0-10 30h20z\"/>";
    xmlstream << "</office:document-content>";
    xmldevice.close();
    doc.setContent(&xmldevice, true, &errorMsg, &errorLine, &errorColumn);
    qDebug() << __PRETTY_FUNCTION__ << errorMsg << errorLine << errorColumn;
    KoXmlElement content = doc.documentElement();
    KoXmlElement element(KoXml::namedItemNS(content, KoXmlNS::draw, "marker"));
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext(stylesReader, 0);
    KoShapeLoadingContext shapeContext(odfContext, 0);

    KoMarker *marker = new KoMarker();
    marker->loadOdf(element, shapeContext);
    QList<KoMarker*> markers;
    markers << marker;
    d->markerSelector->updateMarkers(markers);
#endif
    d->mainWidget = new KoStrokeConfigWidget( this );
    setWidget( d->mainWidget );

    connect( d->mainWidget, SIGNAL(currentIndexChanged()), this, SLOT(styleChanged()));
    connect( d->mainWidget, SIGNAL(widthChanged()),        this, SLOT(widthChanged()));
    connect( d->mainWidget, SIGNAL(capChanged(int)),       this, SLOT(slotCapChanged(int)));
    connect( d->mainWidget, SIGNAL(joinChanged(int)),      this, SLOT(slotJoinChanged(int)));
    connect( d->mainWidget, SIGNAL(miterLimitChanged()),   this, SLOT(miterLimitChanged()));

    d->mainWidget->updateControls(d->border);

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
}

StrokeDocker::~StrokeDocker()
{
    delete d;
}


// ----------------------------------------------------------------
//             Apply changes initiated from the UI


void StrokeDocker::applyChanges()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    canvasController->canvas()->resourceManager()->setActiveBorder( d->border );

    if (!selection || !selection->count())
        return;

    KoLineBorder *newBorder = new KoLineBorder(d->border);
    KoLineBorder *oldBorder = dynamic_cast<KoLineBorder*>( selection->firstSelectedShape()->border() );
    if (oldBorder) {
        newBorder->setColor(oldBorder->color());
        newBorder->setLineBrush(oldBorder->lineBrush());
    }

    KoShapeBorderCommand *cmd = new KoShapeBorderCommand(selection->selectedShapes(), newBorder);
    canvasController->canvas()->addCommand(cmd);
}


void StrokeDocker::styleChanged()
{
    d->border.setLineStyle( d->mainWidget->lineStyle(), d->mainWidget->lineDashes() );
    applyChanges();
}

void StrokeDocker::widthChanged()
{
    d->border.setLineWidth( d->mainWidget->lineWidth() );
    applyChanges();
}

void StrokeDocker::slotCapChanged(int ID)
{
    d->border.setCapStyle(static_cast<Qt::PenCapStyle>(ID));
    applyChanges();
}

void StrokeDocker::slotJoinChanged( int ID )
{
    d->border.setJoinStyle( static_cast<Qt::PenJoinStyle>( ID ) );
    applyChanges();
}

void StrokeDocker::miterLimitChanged()
{
    d->border.setMiterLimit( d->mainWidget->miterLimit() );
    applyChanges();
}

// ----------------------------------------------------------------


void StrokeDocker::setStroke( const KoShapeBorderModel *border )
{
    const KoLineBorder *lineBorder = dynamic_cast<const KoLineBorder*>( border );
    if (lineBorder) {
        d->border.setLineWidth( lineBorder->lineWidth() );
        d->border.setCapStyle( lineBorder->capStyle() );
        d->border.setJoinStyle( lineBorder->joinStyle() );
        d->border.setMiterLimit( lineBorder->miterLimit() );
        d->border.setLineStyle( lineBorder->lineStyle(), lineBorder->lineDashes() );
    }
    else {
        d->border.setLineWidth( 0.0 );
        d->border.setCapStyle( Qt::FlatCap );
        d->border.setJoinStyle( Qt::MiterJoin );
        d->border.setMiterLimit( 0.0 );
        d->border.setLineStyle( Qt::NoPen, QVector<qreal>() );
    }

    d->mainWidget->updateControls(d->border);
}

void StrokeDocker::setUnit(KoUnit unit)
{
    d->mainWidget->setUnit(unit);
}


void StrokeDocker::selectionChanged()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    KoShape * shape = selection->firstSelectedShape();
    if (shape)
        setStroke(shape->border());
}

void StrokeDocker::setCanvas( KoCanvasBase *canvas )
{
    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    if (canvas) {
        connect(canvas->shapeManager()->selection(), SIGNAL(selectionChanged()),
                this, SLOT(selectionChanged()));
        connect(canvas->resourceManager(), SIGNAL(resourceChanged(int, const QVariant&)),
                this, SLOT(resourceChanged(int, const QVariant&)));
        setUnit(canvas->unit());
    }

    d->canvas = canvas;
}

void StrokeDocker::unsetCanvas()
{
    d->canvas = 0;
}  

void StrokeDocker::resourceChanged(int key, const QVariant &value)
{
    switch (key) {
    case KoCanvasResource::Unit:
        setUnit(value.value<KoUnit>());
        break;
    }
}

void StrokeDocker::locationChanged(Qt::DockWidgetArea area)
{
    d->mainWidget->locationChanged(area);
}


#include <StrokeDocker.moc>

