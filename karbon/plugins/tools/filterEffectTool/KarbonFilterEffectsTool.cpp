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
            
            configStack->insertWidget( 0, currentPanel );
            connect( currentPanel, SIGNAL(filterChanged()), tool, SLOT(filterChanged()));
        }
        
        if( currentPanel )
            currentPanel->editFilterEffect(filterEffect);
    }
    
    KComboBox * filterSelector;
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
        // apply the zoom transformation
        KoShape::applyConversion(painter, converter);
        // apply the shape transformation
        QMatrix transform = d->currentShape->absoluteTransformation(&converter);
        painter.setMatrix(transform, true);
        // get the size rect of the shape
        QRectF sizeRect(QPointF(), d->currentShape->size());
        // get the clipping rect of the filter stack
        KoFilterEffectStack * filterStack = d->currentShape->filterEffectStack();
        QRectF clipRect = filterStack->clipRectForBoundingRect(sizeRect);
        // finally paint the clipping rect
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::blue);
        painter.drawRect(clipRect);
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

void KarbonFilterEffectsTool::addFilter()
{
    if( ! d->currentShape )
        return;
    
    KoFilterEffectRegistry * registry = KoFilterEffectRegistry::instance();
    KoFilterEffectFactory * factory = registry->values()[d->filterSelector->currentIndex()];
    if( ! factory )
        return;
    
    QRectF bbox(QPointF(), d->currentShape->size());
    
    KoFilterEffect * effect = factory->createFilterEffect();
    KoFilterEffectStack * effectStack = d->currentShape->filterEffectStack();
    if (!effectStack ) {
        effectStack = new KoFilterEffectStack();
        d->currentShape->setFilterEffectStack(effectStack);
    }
    d->currentShape->update();
    effectStack->appendFilterEffect(effect);
    d->currentShape->update();
    
    d->fillConfigSelector(d->currentShape);
}

void KarbonFilterEffectsTool::removeFilter()
{
    if( ! d->currentShape || !d->currentShape->filterEffectStack())
        return;
    
    QList<KoFilterEffect*> filterEffects = d->currentShape->filterEffectStack()->filterEffects();
    
    int index = d->configSelector->currentIndex();
    if (index >= 0 && index < filterEffects.count()) {
        KoFilterEffect * effect = filterEffects[index];
        
        d->currentShape->update();
        d->currentShape->filterEffectStack()->removeFilterEffect(index);
        d->currentShape->update();
        
        delete effect;

        d->fillConfigSelector(d->currentShape);
    }
}

void KarbonFilterEffectsTool::raiseFilter()
{
}

void KarbonFilterEffectsTool::lowerFilter()
{
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
}

void KarbonFilterEffectsTool::selectionChanged()
{
    d->currentShape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    d->fillConfigSelector(d->currentShape);
}

QMap<QString, QWidget *> KarbonFilterEffectsTool::createOptionWidgets()
{
    QMap<QString, QWidget*> widgets;
    
    KoGenericRegistryModel<KoFilterEffectFactory*> * filterEffectModel = new KoGenericRegistryModel<KoFilterEffectFactory*>(KoFilterEffectRegistry::instance());
    
    //---------------------------------------------------------------------
    
    QWidget * addFilterWidget = new QWidget();
    addFilterWidget->setObjectName("AddEffect");
    QGridLayout * addFilterLayout = new QGridLayout(addFilterWidget);
    
    d->filterSelector = new KComboBox(addFilterWidget);
    d->filterSelector->setModel(filterEffectModel);
    addFilterLayout->addWidget(d->filterSelector, 0, 0);
    
    QToolButton * addButton = new QToolButton(addFilterWidget);
    addButton->setIcon( KIcon("list-add") );
    addButton->setToolTip( i18n("Add filter effect to shape") );
    addFilterLayout->addWidget(addButton, 0, 1);
    connect(addButton, SIGNAL(clicked()), this, SLOT(addFilter()));
    
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
    
    QToolButton * removeButton = new QToolButton(configFilterWidget);
    removeButton->setIcon( KIcon("list-remove") );
    removeButton->setToolTip(i18n("Remove filter effect from shape"));
    configFilterLayout->addWidget(removeButton, 0, 1);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFilter()));

    QToolButton * raiseButton = new QToolButton(configFilterWidget);
    raiseButton->setIcon(KIcon("arrow-up"));
    raiseButton->setToolTip(i18n("Move filter effect up"));
    configFilterLayout->addWidget(raiseButton, 0, 2);
    connect(raiseButton, SIGNAL(clicked()), this, SLOT(raiseFilter()));

    QToolButton * lowerButton = new QToolButton(configFilterWidget);
    lowerButton->setIcon(KIcon("arrow-down"));
    lowerButton->setToolTip(i18n("Move filter effect up"));
    configFilterLayout->addWidget(lowerButton, 0, 3);
    connect(lowerButton, SIGNAL(clicked()), this, SLOT(lowerFilter()));
    
    d->configStack = new QStackedWidget(configFilterWidget);
    configFilterLayout->addWidget(d->configStack, 1, 0, 1, 4);
    
    widgets.insert(i18n("Effect Properties"), configFilterWidget);
    
    //---------------------------------------------------------------------
    
    d->fillConfigSelector(d->currentShape);
    
    return widgets;
}

#include "KarbonFilterEffectsTool.moc"
