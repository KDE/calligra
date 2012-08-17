#include "colorimageprovider.h"

ColorImageProvider::ColorImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

QPixmap ColorImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 100;
    int height = 50;

    if( size )
        *size = QSize(width, height);
    QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                   requestedSize.height() > 0 ? requestedSize.height() : height);
    if(QColor::isValidColor(id))
    {
        pixmap.fill(QColor(id).rgba());
    }
    else
    {
        QList<QString> elements = id.split(",");
        if(elements.count() == 4)
            pixmap.fill(QColor(elements.at(0).toFloat() * 255, elements.at(1).toFloat() * 255, elements.at(2).toFloat() * 255, elements.at(3).toFloat() * 255));
        if(elements.count() == 3)
            pixmap.fill(QColor(elements.at(0).toFloat() * 255, elements.at(1).toFloat() * 255, elements.at(2).toFloat() * 255));
    }
    return pixmap;
}
