
#include <koApplication.h>
#include <kiconloader.h>

KoApplication::KoApplication(int &argc, char **argv, 
			     const QString& rAppName)
    : OPApplication(argc, argv, rAppName)
{
    getLocale()->insertCatalogue("koffice");
    getIconLoader()->insertDirectory(0, kde_datadir() + 
				     "/koffice/toolbar");
    getIconLoader()->insertDirectory(3, kde_localedir() + 
				     "/share/apps/koffice/toolbar");
}

KoApplication::~KoApplication() {}
    
#include "koApplication.moc"
