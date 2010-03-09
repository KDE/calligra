#ifndef KPRESENTERSLIDELOADER_H
#define KPRESENTERSLIDELOADER_H

#include "slideloader.h"

class KoPADocument;

class KPresenterSlideLoader : public SlideLoader {
private:
    const KoPADocument* m_doc;
    int version;

    void closeDocument();
public:
    KPresenterSlideLoader(QObject* parent = 0);
    ~KPresenterSlideLoader();
    int numberOfSlides();
    QSize slideSize();
    int slideVersion(int /*position*/) {
        // version is independent of position for this loader
        return version;
    }
    QPixmap loadSlide(int number, const QSize& maxsize);
    void open(const QString& path);
};

#endif
