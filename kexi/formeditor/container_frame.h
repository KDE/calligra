#ifndef _CONTAINER_FRAME_H_
#define _CONTAINER_FRAME_H_

#include <qframe.h>
#include <formeditor/containerIface.h>

namespace KFormEditor {


	class WidgetContainer;

	class KFORMEDITOR_EXPORT container_Frame: public QFrame, public KFormEditor::containerIface
	{
	Q_OBJECT

	public:
		container_Frame(QWidget *parent, const char* name);
		virtual ~container_Frame();
		virtual void registerContainers(WidgetContainer *par);
	private:
		WidgetContainer *m_container;
	};

}

#endif
