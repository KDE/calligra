#ifndef __ko_app_h__
#define __ko_app_h__

#include <opApplication.h>

class KoApplication : public OPApplication
{
    Q_OBJECT 
    
public:

    KoApplication( int &argc, char **argv, const QString& rAppName = 0);
    virtual ~KoApplication();
    
};

#endif
