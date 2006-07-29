#ifndef QPRO_RECORD_FACTORY_H
#define QPRO_RECORD_FACTORY_H

#include <qpro/stream.h>
#include <qpro/record.h>

class QpRecFactory
{
public:
   QpRecFactory(QpIStream& pIn);
   ~QpRecFactory();

   QpRec* nextRecord();
protected:
   QpIStream& cIn;
};

#endif // QPRO_RECORD_FACTORY_H

