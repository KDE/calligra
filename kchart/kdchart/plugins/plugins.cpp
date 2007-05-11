#include "plugins.h"
#include <KDChartWidget>
#include <KDChartChart>
#include <KDChartLineDiagram>
//#include <KDChartCartesianAxis>
#include "kdchartdesignerextension.h"
#include "kdchartextrainfoextension.h"


#include <QDesignerCustomWidgetInterface>
#include <QDesignerFormEditorInterface>
#include <QExtensionManager>
#include <QtGui>

#include <QString>
#include <QIcon>
#include <qplugin.h>


template <typename T_Widget>
QWidget * create( QWidget * parent ) {
  return new T_Widget( parent );
}

QWidget* createKDChartWidget( QWidget * parent )
{
     KDChart::Widget *w = new KDChart::Widget( parent );
     QVector<double> col0, col1;
     col0 << 3.0 << 0.0 << 2.0 << 1.0 << 3.0 << 0.0;
     col1 << 2.0 << 1.0 << 3.0 << 4.0 << 3.0 << 2.0;
     w->setDataset( 0, col0, "One" );
     w->setDataset( 1, col1, "Two" );
     return w;
}

QWidget* createKDChartChart( QWidget * parent )
{
    QStandardItemModel *m_model = new QStandardItemModel;
    KDChart::Chart *w = new KDChart::Chart(  parent );

    m_model->insertRows( 0, 2, QModelIndex() );
    m_model->insertColumns(  0,  3,  QModelIndex() );
    for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                QModelIndex index = m_model->index(row, column, QModelIndex());
                m_model->setData(index, QVariant(row+1 * column) );
            }
    }

    KDChart::LineDiagram* diagram = new KDChart::LineDiagram;
    diagram->setModel(m_model);

    w->coordinatePlane()->replaceDiagram(diagram);

    return w;/*new KDChart::Chart( parent )*/;
}

void initKDChartChart( QDesignerCustomWidgetInterface *, QDesignerFormEditorInterface * core )
{
    Q_ASSERT( core );
    QExtensionManager * extMgr = core->extensionManager();
    KDChartChartTaskMenuFactory * menuFactory = new KDChartChartTaskMenuFactory( extMgr );
    extMgr->registerExtensions( menuFactory, Q_TYPEID( QDesignerTaskMenuExtension ) );
    extMgr->registerExtensions( new KDChartExtraInfoExtensionFactory(core, extMgr), Q_TYPEID(QDesignerExtraInfoExtension));
}

void initKDChartWidget( QDesignerCustomWidgetInterface *, QDesignerFormEditorInterface * core )
{
  Q_ASSERT( core );
  QExtensionManager * extMgr = core->extensionManager();
  KDChartWidgetTaskMenuFactory * menuFactory = new KDChartWidgetTaskMenuFactory( extMgr );
  extMgr->registerExtensions( menuFactory, Q_TYPEID( QDesignerTaskMenuExtension ) );
  extMgr->registerExtensions( new KDChartExtraInfoExtensionFactory(core, extMgr), Q_TYPEID(QDesignerExtraInfoExtension));
}

typedef QWidget * (*widget_create_func)( QWidget * );
typedef void ( *plugin_init_func )( QDesignerCustomWidgetInterface *, QDesignerFormEditorInterface * );

///\brief WidgetInfo struct, used for the plugin widgets.
///See Qt Designer plugin documentation for details.
static const struct WidgetInfo {
  const char * name;
  const char * group;
  const char * icon;
  const char * includeFile;
  const char * toolTip;
  const char * whatsThis;
  bool isContainer;
  widget_create_func create;
  plugin_init_func init;
} widgetInfos[] = {
  {
      "KDChart::Widget", QT_TR_NOOP("KDChart Widgets"),
    "", "include/KDChartWidget",
    QT_TR_NOOP("A charting widget"),
    QT_TR_NOOP("A charting widget"),
    false, &createKDChartWidget, &initKDChartWidget
  },
  {
      "KDChart::Chart", QT_TR_NOOP("KDChart Widgets"),
      "", "include/KDChartChart",
      QT_TR_NOOP("A charting view"),
      QT_TR_NOOP("A charting view"),
      false, &createKDChartChart, &initKDChartChart
  },
};

static const unsigned int numWidgetInfos = sizeof widgetInfos / sizeof *widgetInfos ;

class KDChartSingleWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
  Q_OBJECT
  Q_INTERFACES( QDesignerCustomWidgetInterface )
public:
  KDChartSingleWidgetPlugin( unsigned int idx_ ) : idx( idx_ ), inited( false ) {}

  QString name() const
    { return QString::fromLatin1( widgetInfos[idx].name ); }
  QString group() const
    { return QString::fromLatin1( widgetInfos[idx].group ); }
  QString toolTip() const
    { return QString::fromLatin1( widgetInfos[idx].toolTip ); }
  QString whatsThis() const
    { return QString::fromLatin1( widgetInfos[idx].whatsThis ); }
  QString includeFile() const
    { return QString::fromLatin1( widgetInfos[idx].includeFile ); }
  QIcon icon() const
    { return QIcon( QString::fromLatin1( widgetInfos[idx].icon ) ); }
  bool isContainer() const
    { return widgetInfos[idx].isContainer; }
  QWidget * createWidget( QWidget * p )
    { return widgetInfos[idx].create( p ); }
  bool isInitialized() const
    { return inited; }
  void initialize( QDesignerFormEditorInterface * core )
    {
      if ( inited ) return;
      if ( plugin_init_func f = widgetInfos[idx].init ) f( this, core );
      inited = true;
    }
  QString codeTemplate() const
    { return QString::null; }
  QString domXml() const
  { return QString::fromLatin1( "<widget class=\"%1\" name=\"%2\">\n"
                                "</widget>\n" ).arg( name(), name().toLower() ); }
protected:
  unsigned int idx;
  bool inited;
};


KDChartWidgetPlugin::KDChartWidgetPlugin( QObject * p )
  : QObject( p ),
    QDesignerCustomWidgetCollectionInterface(),
    mCustomWidgets()
{
  for ( unsigned int i = 0 ; i < numWidgetInfos ; ++i )
    mCustomWidgets.push_back( new KDChartSingleWidgetPlugin( i ) );
}


#include "plugins.moc"

Q_EXPORT_PLUGIN( KDChartWidgetPlugin )
