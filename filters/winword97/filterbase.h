#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <qobject.h>

class FilterBase : public QObject {

    Q_OBJECT

public:
    FilterBase() : QObject() {};
    virtual ~FilterBase() {};

    virtual const bool filter() { return false; };
    virtual const QString part();
    virtual const QString extension() { return ".kwd"; };

signals:
    virtual void signalSavePic(const char *data, const char *nameOUT);
    virtual void signalPart(const char *nameIN, const char *nameOUT);

protected slots:
    virtual void slotSavePic(const char *data, const char *nameOUT);
    virtual void slotPart(const char *nameIN, const char *nameOUT);

private:
    FilterBase(const FilterBase &);
    const FilterBase &operator=(const FilterBase &);
};
#endif // FILTERBASE_H
