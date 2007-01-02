#include "KexiDocument.h"
#include "KexiView.h"

#include <KoXmlWriter.h>

/*
#include <QApplication>
#include <QFileInfo>
#include <QFont>
#include <QPair>
#include <QTimer>
#include <kcompletion.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ktemporaryfile.h>
#include <KoApplication.h>
#include <KoDocumentInfo.h>
#include <KoDom.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOasisStyles.h>
#include <KoStoreDevice.h>
#include <KoVariable.h>
#include <KoXmlNS.h>
*/

class KexiDocument::Private
{
    public:
};

//QList<Doc*> Doc::Private::s_docs;
//int Doc::Private::s_docId = 0;

KexiDocument::KexiDocument(QWidget *parentWidget, QObject* parent, bool singleViewMode)
    : KoDocument(parentWidget, parent, singleViewMode)
    , d( new Private() )
{
}

KexiDocument::~KexiDocument()
{
  //if(isReadWrite()) saveConfig();
  delete d;
}

void KexiDocument::paintContent(QPainter& painter, const QRect& rect, bool transparent, double zoomX, double zoomY)
{
    //TODO
}

bool KexiDocument::loadXML(QIODevice*, const KoXmlDocument& doc)
{
    //TODO
    return true;
}

bool KexiDocument::loadOasis(const KoXmlDocument& doc, KoOasisStyles& oasisStyles, const KoXmlDocument& settings, KoStore*)
{
    //TODO
    return true;
}

bool KexiDocument::saveOasis(KoStore* store, KoXmlWriter* manifestWriter)
{
    //TODO
    return true;
}

KoView* KexiDocument::createViewInstance(QWidget* parent)
{
    return new KexiView(parent, this);
}

#include "KexiDocument.moc"

