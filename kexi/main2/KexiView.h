#ifndef KEXIVIEW_H
#define KEXIVIEW_H

#include <QWidget>
//#include <QList>
//#include <QPoint>
//#include <QString>
//#include <QStringList>

//#include <kprinter.h>
//#include <kdeprint/kprintdialogpage.h>
//#include <QPointF>
#include <KoView.h>
//#include <KoZoomMode.h>

class KexiDocument;

class KDE_EXPORT KexiView : public KoView
{
        Q_OBJECT
    public:

        /**
        * Creates a new view.
        */
        KexiView(QWidget* parent, KexiDocument* document);

        /**
        * Destroys the view.
        */
        ~KexiView();

        /**
        * Return the \a KexiDocument instance.
        */
        KexiDocument* doc() const;

        /**
        * Update the readwrite/readonly state.
        * Implement abstract \a KoView::updateReadWrite method.
        */
        virtual void updateReadWrite(bool readwrite);

    private:
        class Private;
        Private* const d;
};

#endif
