#ifndef PICTURE_H
#define PICTURE_H

#include <qobject.h>

class Picture : public QObject {

    Q_OBJECT

public:
    Picture();
    ~Picture();

private:
    Picture(const Picture &);
    const Picture &operator=(const Picture &);
};
#endif // PICTURE_H
