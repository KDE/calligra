#ifndef __koffice_filter_h__
#define __koffice_filter_h__

#include <qcstring.h>
#include <qobject.h>

/**
 * This is an abstract base class for filters.
 */
class KoFilter : public QObject {

    Q_OBJECT

public:
    virtual ~KoFilter() {}
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString &from, const QCString &to,
                              const QString &config=QString::null) = 0;
protected:
    KoFilter(KoFilter *parent, QString name);
};
#endif
