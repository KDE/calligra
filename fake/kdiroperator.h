#ifndef FAKE_KDIROPERATOR_H
#define FAKE_KDIROPERATOR_H

#include <QObject>
#include <kurl.h>

class KDirOperator
{
public:
    KUrl url() const { return KUrl(); }
};

#endif
 
