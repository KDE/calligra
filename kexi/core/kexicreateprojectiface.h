#ifndef _KEXI_CREATE_PROJECT_IFACE_H_
#define _KEXI_CREATE_PROJECT_IFACE_H_
class KexiCreateProjectIface {
	public:
	KexiCreateProjectIface(){;}
	virtual ~KexiCreateProjectIface(){;}
	virtual int execute()=0;
};

#endif
