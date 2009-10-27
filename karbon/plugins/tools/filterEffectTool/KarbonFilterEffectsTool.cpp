/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonFilterEffectsTool.h"

#include "KoGenericRegistryModel.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoFilterEffectFactory.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffectConfigWidgetBase.h"
#include "KoCanvasBase.h"
#include "KoCanvasResourceProvider.h"
#include "KoShapeManager.h"
#include "KoSelection.h"
#include "FilterEffectEditWidget.h"
#include "FilterEffectResource.h"
#include "FilterResourceServerProvider.h"
#include "FilterStackSetCommand.h"
#include "KoResourceServerAdapter.h"
#include "KoResourceSelector.h"

#include <KComboBox>
#include <KLocale>
#include <KIcon>
#include <KDialog>

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QToolButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>

class KarbonFilterEffectsTool::Private
{
public:
    Private()
    : filterSelector(0), configSelector(0)
    , configStack(0)
    , currentEffect(0), currentPanel(0), currentShape(0)
    {
    }
    
    void fillConfigSelector(KoShape *shape)
    {
        if (!configSelector)
            return;
        
        configSelector->clear();
        
        if (!shape || !shape->filterEffectStack())
            return;
        
        int index = 0;
        foreach(KoFilterEffect *effect, shape->filterEffectStack()->filterEffects()) {
            configSelector->addItem(QString("%1 - ").arg(index) + effect->name());
            index++;
        }
    }
    
    void addWidgetForEffect(KoFilterEffect * filterEffect, KarbonFilterEffectsTool * tool)
    {
        // remove current widget if new effect is zero or effect type has changed
        if( !filterEffect || (currentEffect && filterEffect->id() != currentEffect->id())) {
            while( configStack->count() )
                configStack->removeWidget( configStack->widget( 0 ) );
        }
        
        if( ! filterEffect ) {
            currentEffect = 0;
            currentPanel = 0;
            return;
        }  else if (!currentEffect || currentEffect->id() != filterEffect->id()) {
            // when a shape is set and is differs from the previous one
            // get the config widget and insert it into the option widget
            currentEffect = filterEffect;
            
            KoFilterEffectRegistry * registry = KoFilterEffectRegistry::instance();
            KoFilterEffectFactory * factory = registry->value(currentEffect->id());
            if (!factory)
                return;
            
            currentPanel = factory->createConfigWidget();
            if( ! currentPanel )
                return;
            
            currentPanel->layout()->setContentsMargins(0,0,0,0);
            configStack->insertWidget( 0, currentPanel );
            configStack->layout()->setContentsMargins(0,0,0,0);
            connect( currentPanel, SIGNAL(filterChanged()), tool, SLOT(filterChanged()));
        }
        
        if( currentPanel )
            currentPanel->editFilterEffect(filterEffect);
    }
    
    KoResourceSelector * filterSelector;
    KComboBox * configSelector;
    QStackedWidget * configStack;
    KoFilterEffect * currentEffect;
    KoFilterEffectConfigWidgetBase * currentPanel;
    KoShape * currentShape;
};

KarbonFilterEffectsTool::KarbonFilterEffectsTool(KoCanvasBase *canvas)
: KoTool(canvas), d(new Private())
{
    connect(canvas->shapeManager(), SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));
}

KarbonFilterEffectsTool::~KarbonFilterEffectsTool()
{
    delete d;
}

void KarbonFilterEffectsTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (d->currentShape && d->currentShape->filterEffectStack()) {
        painter.save();
        // apply the shape transformation
        QMatrix transform = d->currentShape->absoluteTransformation(&converter);
        painter.setMatrix(transform, true);
        // apply the zoom transformation
        KoShape::applyConversion(painter, converter);
        // get the size rect of the shape
        QRectF sizeRect(QPointF(), d->currentShape->size());
        // get the clipping rect of the filter stack
        KoFilterEffectStack * filterStack = d->currentShape->filterEffectStack();
        QRectF clipRect = filterStack->clipRectForBoundingRect(sizeRect);
        // finally paint the clipping rect
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::blue);
        painter.drawRect(clipRect);
        
        if (d->currentEffect) {
            QRectF filterRect = d->currentEffect->filterRectForBoundingRect(sizeRect);
            // paint the filter subregion rect
            painter.setBrush(Qt::NoBrush);
            painter.setPen(Qt::red);
            painter.drawRect(filterRect);
        }
        
        painter.restore();
    }
}

void KarbonFilterEffectsTool::repaintDecorations()
{
    if (d->currentShape && d->currentShape->filterEffectStack()) {
        QRectF bb = d->currentShape->boundingRect();
        int radius = m_canvas->resourceProvider()->handleRadius();
        m_canvas->updateCanvas(bb.adjusted(-radius, -radius, radius, radius));
    }
}

void KarbonFilterEffectsTool::activate( bool temporary )
{
    Q_UNUSED(temporary);
    if( ! m_canvas->shapeManager()->selection()->count() )
    {
        emit done();
        return;
    }
    
    d->currentShape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    d->fillConfigSelector(d->currentShape);
}

void KarbonFilterEffectsTool::mousePressEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KarbonFilterEffectsTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KarbonFilterEffectsTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KarbonFilterEffectsTool::presetSelected(KoResource *resource)
{
    if (!d->currentShape)
        return;
    
    FilterEffectResource * effectResource = dynamic_cast<FilterEffectResource*>(resource);
    if (!effectResource)
        return;
    
    KoFilterEffectStack * filterStack = effectResource->toFilterStack();
    if (!filterStack)
        return;
    
    m_canvas->addCommand(new FilterStackSetCommand(filterStack, d->currentShape));
    d->fillConfigSelector(d->currentShape);
}

void KarbonFilterEffectsTool::editFilter()
{
    QPointer<KDialog> dlg = new KDialog();
    dlg->setCaption(i18n("Filter Effect Editor"));
    dlg->setButtons(KDialog::Close);

    FilterEffectEditWidget * editor = new FilterEffectEditWidget(dlg);
    editor->editShape(d->currentShape, m_canvas);
    
    dlg->setMainWidget(editor);
    dlg->exec();
    delete dlg;
    
    d->fillConfigSelector(d->currentShape);
}

void KarbonFilterEffectsTool::filterChanged()
{
    if( ! d->currentShape )
        return;
    
    d->currentShape->update();
}

void KarbonFilterEffectsTool::filterSelected(int index)
{
    if( ! d->currentShape || ! d->currentShape->filterEffectStack())
        return;
    
    KoFilterEffect * effect = 0;
    QList<KoFilterEffect*> filterEffects = d->currentShape->filterEffectStack()->filterEffects();
    if (index >= 0 && index < filterEffects.count()) {
        effect = filterEffects[index];
    }
    
    d->addWidgetForEffect(effect, this);
    repaintDecorations();
}

void KarbonFilterEffectsTool::selectionChanged()
{
    d->currentShape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    d->fillConfigSelector(d->currentShape);
}

QMap<QString, QWidget *> KarbonFilterEffectsTool::createOptionWidgets()
{
    QMap<QString, QWidget*> widgets;
    
    FilterResourceServerProvider * serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> * server = serverProvider->filterEffectServer();
    KoAbstractResourceServerAdapter * adapter = new KoResourceServerAdapter<FilterEffectResource>(server, this);
    
    //---------------------------------------------------------------------
    
    QWidget * addFilterWidget = new QWidget();
    addFilterWidget->setObjectName("AddEffect");
    QGridLayout * addFilterLayout = new QGridLayout(addFilterWidget);
    
    d->filterSelector = new KoResourceSelector(addFilterWidget);
    d->filterSelector->setResourceAdapter(adapter);
    d->filterSelector->setDisplayMode(KoResourceSelector::TextMode);
    d->filterSelector->setColumnCount(1);
    addFilterLayout->addWidget(new QLabel(i18n("Effects"), addFilterWidget), 0, 0);
    addFilterLayout->addWidget(d->filterSelector, 0, 1);
    connect(d->filterSelector, SIGNAL(resourceSelected(KoResource*)),
            this, SLOT(presetSelected(KoResource*)));
            
    connect(d->filterSelector, SIGNAL(resourceApplied(KoResource*)),
            this, SLOT(presetSelected(KoResource*)));

    QToolButton * editButton = new QToolButton(addFilterWidget);
    editButton->setIcon(KIcon("view-filter"));
    editButton->setToolTip(i18n("View and edit filter"));
    addFilterLayout->addWidget(editButton, 0, 2);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editFilter()));
    
    widgets.insert(i18n("Add Filter"), addFilterWidget);
    
    //---------------------------------------------------------------------
    
    QWidget * configFilterWidget = new QWidget();
    configFilterWidget->setObjectName("ConfigEffect");
    QGridLayout * configFilterLayout = new QGridLayout(configFilterWidget);
    
    d->configSelector = new KComboBox(configFilterWidget);
    configFilterLayout->addWidget(d->configSelector, 0, 0);
    connect(d->configSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(filterSelected(int)));
    
    d->configStack = new QStackedWidget(configFilterWidget);
    configFilterLayout->addWidget(d->configStack, 1, 0);
    configFilterLayout->setContentsMargins(0,0,0,0);
    
    widgets.insert(i18n("Effect Properties"), configFilterWidget);
    
    //---------------------------------------------------------------------
    
    d->fillConfigSelector(d->currentShape);
    
    return widgets;
}

#include "KarbonFilterEffectsTool.moc"
