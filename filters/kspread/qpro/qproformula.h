#ifndef QPROFORMULA_H
#define QPROFORMULA_H

#include <qpro/formula.h>

namespace KSpread
{
namespace QuattroPro
{

class Formula : public QpFormula
{
public:
   Formula(QpRecFormulaCell& pCell, QpTableNames& pTable);
   ~Formula();
};

}
}

#endif // QPROFORMULA_H
