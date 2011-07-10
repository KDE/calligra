#include "PresentationViewPortShapePlugin.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <kpluginfactory.h>
#include "PresentationViewPortShape.h"
#include "PresentationViewPortShapeFactory.h"

K_PLUGIN_FACTORY(PresentationViewPortShapePluginFactory, registerPlugin<PresentationViewPortShapePlugin>();)
K_EXPORT_PLUGIN(PresentationViewPortShapePluginFactory("PresentationViewPortShape"))

PresentationViewPortShapePlugin::PresentationViewPortShapePlugin( QObject * parent, const QVariantList & )
    : QObject(parent)
{
    KoShapeRegistry::instance()->add( new PresentationViewPortShapeFactory() );
    }

PresentationViewPortShapePlugin::~PresentationViewPortShapePlugin()
{
}

#include <PresentationViewPortShapePlugin.moc>
