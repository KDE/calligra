#ifndef COLORIMAGEPROVIDER_H
#define COLORIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class ColorImageProvider : public QDeclarativeImageProvider
{
public:
    explicit ColorImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // COLORIMAGEPROVIDER_H
