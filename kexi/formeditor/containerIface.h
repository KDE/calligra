#ifndef _KFE_CONTAINER_IFACE_H_
#define _KFE_CONTAINER_IFACE_H_

class QObject;
class QEvent;

namespace KFormEditor
{

	class WidgetContainer;

	class KFORMEDITOR_EXPORT containerIface
	{
	public:
	        enum Action{None=0,Activate=1,Move=2,EatEvent=4,MoveEatEvent=6,AllEat=7};

		containerIface();
		virtual ~containerIface();
		virtual void registerContainers(WidgetContainer*)=0;
		virtual Action allowMousePress(QObject*,QEvent*){return AllEat;}
	};
	
}



#endif
