#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <qobject.h>
#include <qstring.h>
#include <myfile.h>

class Picture;

// Attention: The nameOUT Strings are allocated with new[] in the
// slots!!! Therefore you have to delete [] them!
class FilterBase : public QObject {

    Q_OBJECT

public:
    FilterBase();
    virtual ~FilterBase() {}

    virtual const bool filter();
    virtual const QString part() { return QString(""); }

signals:
    void signalSavePic(Picture *pic);
    void signalPart(const char *nameIN, char **nameOUT);
    void signalGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void signalGetStream(const QString &name, myFile &stream);

protected slots:
    void slotSavePic(Picture *pic);
    void slotPart(const char *nameIN, char **nameOUT);
    void slotFilterError();

protected:
    bool success;      // ok, the filtering process was successful
    bool ready;        // filtering took place, you may fetch the file now

private:
    FilterBase(const FilterBase &);
    const FilterBase &operator=(const FilterBase &);
};
#endif // FILTERBASE_H
