#include "container_frame.h"
#include "container_frame.moc"
#include "widgetcontainer.h"

#include <qlayout.h>

namespace KFormEditor {

	container_Frame::container_Frame(QWidget *parent, const char* name):QFrame(parent,name),containerIface() {
		setFrameShape(QFrame::Panel);
		QVBoxLayout *l=new QVBoxLayout(this);
		l->setMargin(frameWidth());
		l->setAutoAdd(true);
		m_container=new WidgetContainer(this);		
	}

	void container_Frame::registerContainers(WidgetContainer *par) {
		par->registerSubContainer(m_container);
	}

	container_Frame::~container_Frame(){
	}

}
