#include "animator.h"

#include <stdlib.h>

#include <QTimer>

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kis_debug.h>
#include <kpluginfactory.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <KoDockFactoryBase.h>
#include <KoDockRegistry.h>


#include "kis_config.h"
#include "kis_cursor.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_view2.h"

#include "animator_dock.h"
#include "animator_light_table_dock.h"

K_PLUGIN_FACTORY(AnimatorPluginFactory, registerPlugin<AnimatorPlugin>();)
K_EXPORT_PLUGIN(AnimatorPluginFactory("krita"))

class AnimatorDockFactory : public KoDockFactoryBase {
public:
    AnimatorDockFactory()
    {
    }

    virtual QString id() const
    {
        return QString( "Animator" );
    }

    virtual Qt::DockWidgetArea defaultDockWidgetArea() const
    {
        return Qt::BottomDockWidgetArea;
    }

    virtual QDockWidget* createDockWidget()
    {
        AnimatorDock * dockWidget = new AnimatorDock();
        
        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const
    {
        return DockMinimized;
    }
private:
};

class LightTableDockFactory : public KoDockFactoryBase {
public:
    LightTableDockFactory()
    {
    }
    
    virtual QString id() const
    {
        return QString( "Light table" );
    }
    
    virtual Qt::DockWidgetArea defaultDockWidgetArea() const
    {
        return Qt::RightDockWidgetArea;
    }
    
    virtual QDockWidget* createDockWidget()
    {
        AnimatorLightTableDock* dockWidget = new AnimatorLightTableDock();
        
        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const
    {
        return DockMinimized;
    }
};


AnimatorPlugin::AnimatorPlugin(QObject *parent, const QVariantList &)
    : KParts::Plugin(parent)
{
    KoDockRegistry::instance()->add(new AnimatorDockFactory());
    KoDockRegistry::instance()->add(new LightTableDockFactory());
}

AnimatorPlugin::~AnimatorPlugin()
{
}

#include "animator.moc"
