#ifndef KEXIDOCUMENT_H
#define KEXIDOCUMENT_H

#include <QList>
#include <QMap>
#include <QRect>
#include <QString>

//#include <kglobalsettings.h>

#include <KoDocument.h>
//#include <KoXmlReader.h>
#include <KoZoomHandler.h>

//#include "Global.h"

//class QDomDocument;
//class QObject;
//class QPainter;

// class DCOPObject;
//class KCommand;
//class KCommandHistory;
//class KCompletion;
//class K3SpellConfig;
//class KoGenStyles;
//class KoOasisSettings;
//class KoPicture;
//class KoPictureCollection;
//class KoPictureKey;
class KoStore;
class KoXmlWriter;

class KDE_EXPORT KexiDocument : public KoDocument, public KoZoomHandler
{
        Q_OBJECT
    public:

        /**
        * Creates a new document.
        * @param parentWidget the parent widget
        * @param parent the parent object
        * @param singleViewMode enables single view mode, if @c true
        */
        KexiDocument(QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false);

        /**
        * Destroys the document.
        */
        ~KexiDocument();

        /**
        * Main painting method.
        * Implement abstract \a KoZoomHandler::paintContent method.
        */
        virtual void paintContent(QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0);

        /**
        * Main loading method.
        * Implement abstract \a KoDocument::loadXML method.
        */
        virtual bool loadXML(QIODevice*, const KoXmlDocument& doc);

        /**
        * Main loading method.
        * Implement abstract \a KoDocument::loadOasis method.
        */
        virtual bool loadOasis(const KoXmlDocument& doc, KoOasisStyles& oasisStyles, const KoXmlDocument& settings, KoStore*);

        /**
        * Main saving method.
        * Implement abstract \a KoDocument::saveOasis method.
        */
        virtual bool saveOasis(KoStore* store, KoXmlWriter* manifestWriter);

    protected:

        /**
        * Create and return a new \a KexiView instance.
        * Implement abstract \a KoDocument::createViewInstance method.
        */
        virtual KoView* createViewInstance(QWidget* parent);

    private:
        class Private;
        Private* const d;
};

#endif
