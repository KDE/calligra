#include "kdchartdesignerextension.h"

#include "kdchartwidgetdesignercustomeditor.h"
#include "kdchartchartdesignercustomeditor.h"

#include <KDChartChart>
#include <KDChartWidget>

#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>

#include <QAction>


KDChartBaseTaskMenu::~KDChartBaseTaskMenu()
{
}

QAction * KDChartBaseTaskMenu::preferredEditAction() const
{
  return mEditPropertiesAction;
}

QList<QAction*> KDChartBaseTaskMenu::taskActions() const
{
  QList<QAction*> actions;
  actions.append( mEditPropertiesAction );

  return actions;
}

void KDChartBaseTaskMenu::editChartProperties()
{
    doEditChartProperties();
}

KDChartBaseTaskMenu::KDChartBaseTaskMenu( QObject * parentW )
    : QObject( parentW ),
    mEditPropertiesAction( 0 )
{
    mEditPropertiesAction = new QAction( tr( "Edit chart properties..." ), this );
    connect( mEditPropertiesAction, SIGNAL( triggered() ), this, SLOT( editChartProperties() ) );
}

KDChartWidgetTaskMenu::KDChartWidgetTaskMenu( KDChart::Widget * chartWidget, QObject * parentW )
    : KDChartBaseTaskMenu( parentW ),
      mChart( chartWidget )
{
}

void KDChartWidgetTaskMenu::doEditChartProperties()
{
    KDChartWidgetDesignerCustomEditor dlg( mChart );
    dlg.exec();
}

KDChartChartTaskMenu::KDChartChartTaskMenu( KDChart::Chart * chart, QObject * parentW )
    : KDChartBaseTaskMenu( parentW ),
    mChart( chart )
{
}

void KDChartChartTaskMenu::doEditChartProperties()
{
    KDChartChartDesignerCustomEditor dlg( mChart );
    dlg.exec();
}

KDChartWidgetTaskMenuFactory::KDChartWidgetTaskMenuFactory( QExtensionManager * extMgr ):
    QExtensionFactory( extMgr )
{
}

QObject * KDChartWidgetTaskMenuFactory::createExtension( QObject * object, const QString &iid, QObject * parentW) const
{
    if ( KDChart::Widget * kdchart = qobject_cast<KDChart::Widget*>( object ) ) {
        if ( iid == Q_TYPEID( QDesignerTaskMenuExtension ) ) {
            return new KDChartWidgetTaskMenu( kdchart, parentW );
        }
    }

    return 0;
}


KDChartChartTaskMenuFactory::KDChartChartTaskMenuFactory( QExtensionManager * extMgr ):
        QExtensionFactory( extMgr )
{
}

QObject * KDChartChartTaskMenuFactory::createExtension( QObject * object, const QString &iid, QObject * parentW) const
{
    if ( KDChart::Chart * chart = qobject_cast<KDChart::Chart*>( object ) ) {
        if ( iid == Q_TYPEID( QDesignerTaskMenuExtension ) ) {
            return new KDChartChartTaskMenu( chart, parentW );
        }
    }

    return 0;
}
