#ifndef _KFE_CONTAINER_IFACE_H_
#define _KFE_CONTAINER_IFACE_H_

namespace KFormEditor
{

	class WidgetContainer;

	class containerIface
	{
	public:
		containerIface();
		virtual ~containerIface();
		virtual void registerContainers(WidgetContainer*)=0;
	};
	
}



#endif
