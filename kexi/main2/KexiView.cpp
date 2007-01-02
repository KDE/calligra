#include "KexiView.h"
#include "KexiDocument.h"
#include "KexiFactory.h"

/*
#include <kprinter.h> // has to be first
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QCursor>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QRegExp>
#include <QResizeEvent>
#include <QTimer>
#include <QToolButton>
#include <QSqlDatabase>
#include <QSizePolicy>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kfontaction.h>
#include <kfontsizeaction.h>
#include <kfontdialog.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kpassworddialog.h>
#include <kprocio.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <ktoolbarpopupaction.h>
#include <kparts/partmanager.h>
#include <kpushbutton.h>
#include <kxmlguifactory.h>
#include <kicon.h>
#include <KoCharSelectDia.h>
#include <KoMainWindow.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStore.h>
#include <KoOasisStyles.h>
#include <KoPartSelectAction.h>
#include <KoTabBar.h>
#include <Toolbox.h>
#include <KoTemplateCreateDia.h>
#include <KoZoomAction.h>
#include <ktoolinvocation.h>
*/

class KexiView::Private
{
    public:
        KexiDocument* const doc;
        Private(KexiDocument* const d) : doc(d) {}
};

KexiView::KexiView(QWidget* parent, KexiDocument* document)
    : KoView(document, parent)
    , d( new Private(document) )
{
    setInstance( KexiFactory::global() );
    if ( doc()->isReadWrite() )
        setXMLFile( "kexi.rc" );
    else
        setXMLFile( "kexi_readonly.rc" );
}

KexiView::~KexiView()
{
    delete d;
}

KexiDocument* KexiView::doc() const
{
    return d->doc;
}

void KexiView::updateReadWrite(bool readwrite)
{
    //TODO
}

#include "KexiView.moc"
