#ifndef _CONTAINER_TABWIDGET_H_
#define _CONTAINER_TABWIDGET_H_

#include <qtabwidget.h>
#include <formeditor/containerIface.h>

namespace KFormEditor {


	class WidgetContainer;

	class KFORMEDITOR_EXPORT container_TabWidget: public QTabWidget, public KFormEditor::containerIface
	{
	Q_OBJECT

	public:
		container_TabWidget(QWidget *parent, const char* name);
		virtual ~container_TabWidget();
		virtual void registerContainers(WidgetContainer *par);
		virtual containerIface::Action allowMousePress(QObject*,QEvent*);

		
	private:
		WidgetContainer *m_container1;
		WidgetContainer *m_container2;
	};

}

#endif
