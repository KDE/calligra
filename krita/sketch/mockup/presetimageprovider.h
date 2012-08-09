#ifndef PRESETIMAGEPROVIDER_H
#define PRESETIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class PresetImageProvider : public QDeclarativeImageProvider
{
public:
    explicit PresetImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    
private:
    class Private;
    Private* d;
};

#endif // PRESETIMAGEPROVIDER_H
