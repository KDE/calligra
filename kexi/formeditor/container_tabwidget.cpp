#include "container_tabwidget.h"
#include "container_tabwidget.moc"
#include "widgetcontainer.h"

#include <qlayout.h>
#include <qtabbar.h>
namespace KFormEditor {

	container_TabWidget::container_TabWidget(QWidget *parent, const char* name):QTabWidget(parent,name),containerIface() {
		m_container1=new WidgetContainer(this);
		m_container2=new WidgetContainer(this);
		addTab(m_container1,"Tab1");
		addTab(m_container2,"Tab2");
	}

	void container_TabWidget::registerContainers(WidgetContainer *par) {
		par->registerSubContainer(m_container1);
		par->registerSubContainer(m_container2);
	}

	container_TabWidget::~container_TabWidget(){
	}

        containerIface::Action container_TabWidget::allowMousePress(QObject* o,QEvent*)
	{
		if (o==tabBar()) return (containerIface::Action)(Activate | Move);
		else return (AllEat);
	}

}
