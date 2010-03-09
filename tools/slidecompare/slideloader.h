#ifndef SLIDELOADER_H
#define SLIDELOADER_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

class SlideLoader : public QObject {
private:
Q_OBJECT
public:
    SlideLoader(QObject* parent = 0) :QObject(parent) {}
    virtual int numberOfSlides() = 0;
    virtual QSize slideSize() = 0;
    virtual int slideVersion(int position) = 0;
    virtual QPixmap loadSlide(int number, const QSize& maxsize) = 0;
signals:
    void slidesChanged();
};

#endif
