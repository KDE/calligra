#ifndef QPROFORMULA_H
#define QPROFORMULA_H

#include <qpro/formula.h>

class KSpreadFormula : public QpFormula
{
public:
   KSpreadFormula(QpRecFormulaCell& pCell, QpTableNames& pTable);
   ~KSpreadFormula();
};

#endif // QPROFORMULA_H
