#ifndef QPRO_FORMULA_H
#define QPRO_FORMULA_H

#include <qpro/tablenames.h>
#include <qpro/stream.h>
#include <qpro/record.h>

class QpFormula;

// --------------------------------------------------------------------

class QpFormulaStack
{
public:
   QpFormulaStack();
   ~QpFormulaStack();
 
   void        bracket(const char* pBefore="(", const char* pAfter=")");
   void        push(const char* pString);
   void        pop(int pCnt=1);
   void        join(int pCnt, const char* pSeparator=",");
   const char* top();
 
   const char* operator [] (int pIdx);
 
protected:
   int    cIdx;
   int    cMax;
 
   char** cStack;
};


// --------------------------------------------------------------------

struct QpFormulaConv
{
   QP_INT8       cOperand;
   void        (*cFunc)(QpFormula& pThis, const char* pArg);
   const char*   cArg;
};

// --------------------------------------------------------------------

class QpFormula
{
public:
   QpFormula(QpRecFormulaCell& pCell, QpTableNames& pTable);
   ~QpFormula();

   void  argSeparator(const char* pArg);

   char* formula();


   static inline void binaryOperand(QpFormula& pThis, const char* pOper)
                      {pThis.binaryOperandReal(pOper);}

   static inline void floatFunc(QpFormula& pThis, const char* pFunc)
                      {pThis.floatFuncReal(pFunc);}

   void               formulaStart(const char* pFirstChar);

   static inline void func0(QpFormula& pThis, const char* pFunc)
                      {pThis.func0Real(pFunc);}

   static inline void func1(QpFormula& pThis, const char* pFunc)
                      {pThis.func1Real(pFunc);}

   static inline void func2(QpFormula& pThis, const char* pFunc)
                      {pThis.func2Real(pFunc);}

   static inline void funcV(QpFormula& pThis, const char* pFunc)
                      {pThis.funcVReal(pFunc);}

   static inline void intFunc(QpFormula& pThis, const char* pFunc)
                      {pThis.intFuncReal(pFunc);}

   void               dropLeadingAt(int pBool=-1);

   static inline void ref(QpFormula& pThis, const char* pFunc)
                      {pThis.refReal(pFunc);}

   void               replaceFunc(QpFormulaConv* pFuncEntry);

   static inline void stringFunc(QpFormula& pThis, const char* pFunc)
                      {pThis.stringFuncReal(pFunc);}

   static inline void unaryOperand(QpFormula& pThis, const char* pOper)
                      {pThis.unaryOperandReal(pOper);}

protected:
   char*              cArgSeparator;
   QpRecFormulaCell&  cCell;
   int                cDropLeadingAt;
   QpIStream          cFormula;
   QpIStream          cFormulaRefs;
   char*              cFormulaStart;
   int                cIdx;
   QpFormulaConv*     cReplaceFunc;
   QpFormulaStack     cStack;
   QpTableNames&      cTable;

   void binaryOperandReal(const char* pOper);
   void floatFuncReal(const char* pFunc);
   void func0Real(const char* pFunc);
   void func1Real(const char* pFunc);
   void func2Real(const char* pFunc);
   void funcVReal(const char* pFunc);
   void intFuncReal(const char* pFunc);
   void refReal(const char* pFunc);
   void stringFuncReal(const char* pFunc);
   void unaryOperandReal(const char* pOper);
};

#endif // QPRO_FORMULA_H
