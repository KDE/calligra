//#include <qapplication.h>
//#include <main/keximainwindowimpl.h>
#include <KoApplication.h>
#include <kcmdlineargs.h>

#include "KexiAboutData.h"

static const KCmdLineOptions options[]=
{
    {"+[file]", I18N_NOOP("File to open"), 0},
    KCmdLineLastOption
};

extern "C" KDE_EXPORT int kdemain(int argc, char *argv[])
{
    KCmdLineArgs::init( argc, argv, newAboutData() );
    KCmdLineArgs::addCmdLineOptions( options );

    KoApplication app;
    if(! app.start() )
        return 1;
    app.exec();
    return 0;

    /*
    int result = KexiMainWindowImpl::create(argc, argv);
    if (!qApp)
        return result;

    result = qApp->exec();
    delete qApp;
    return result;
    */
}
