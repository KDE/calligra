#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include <qobject.h>

class Paragraph : public QObject {

    Q_OBJECT

public:
    Paragraph();
    ~Paragraph();

private:
    Paragraph(const Paragraph &);
    const Paragraph &operator=(const Paragraph &);
};
#endif // PARAGRAPH_H
