#ifndef QPRO_RECORD_H
#define QPRO_RECORD_H

#include <qpro/tablenames.h>
#include <qpro/stream.h>

enum QpRecType
{
   QpBof               = 0,
   QpEof               = 1,
   QpRecalcMode        = 2,
   QpRecalcOrder       = 3,
   QpEmptyCell         = 12,
   QpIntegerCell       = 13,
   QpFloatingPointCell = 14,
   QpLabelCell         = 15,
   QpFormulaCell       = 16,
   QpPassword          = 75,
   QpBop               = 202,
   QpPageName          = 204,
   QpUnknown           = -1
};

// -----------------------------------------------------------------------

class QpRec
{
public:
   QpRec(QpRecType pType);
   ~QpRec();

   QP_INT16 type();

protected:
   QP_INT16 cType;
};

// -----------------------------------------------------------------------

//class QP_CELL_REF
//{
//public:
//   QP_CELL_REF(QpIStream& pIn);
//   ~QP_CELL_REF();
// 
//   QP_UINT8 Column();
//   QP_INT16 Row();
// 
//protected:
//   QP_UINT8 cColumn;
//   QP_INT16 cNoteBook;
//   QP_INT8  cPage;
//   QP_INT16 cRow;
//};

// -----------------------------------------------------------------------

class QpRecCell : public QpRec
{
public:
   QpRecCell(QpRecType pType);
   ~QpRecCell();

   void        attributes(QP_INT16 pAttributes);
   QP_INT16    attributes();
 
//   const char* cellRef();
   void        cellRef(char* pText, QpTableNames& pTable, QP_INT16 pNoteBook, QP_UINT8 pPage, QP_UINT8 pColumn, QP_INT16 pRow);
   void        cellRef(char* pText, QpTableNames& pTable, QpIStream& pFormulaRef);
 
   void        column(QP_UINT8 pColumn);
   QP_UINT8    column();
 
   void        row(QP_INT16 pRow);
   QP_INT16    row();

protected:
   int         loadCellInfo(QpIStream& pIn);

   QP_INT16 cAttributes;
   QP_UINT8 cColumn;
   QP_UINT8 cPage;
   QP_INT16 cRow;
   char*    cCellRef;
};

// -----------------------------------------------------------------------

class QpRecBof : public QpRec
{
public:
   QpRecBof(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecBof();

   void     fileFormat(QP_INT16 pFileFormat);
   QP_INT16 fileFormat();

protected:
   QP_INT16 cFileFormat;
};

// -----------------------------------------------------------------------

class QpRecEof : public QpRec
{
public:
   QpRecEof(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecEof();
};


// -----------------------------------------------------------------------

class QpRecRecalcMode : public QpRec
{
public:
   enum MODE{Manual=0, Background=1, Automatic=255};

   QpRecRecalcMode(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecRecalcMode();

   void mode(MODE pMode);
   MODE mode();

protected:
   MODE cMode;
};


// -----------------------------------------------------------------------

class QpRecRecalcOrder : public QpRec
{
public:
   enum ORDER { Natural=0, Column=1, Row=255 };

   QpRecRecalcOrder(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecRecalcOrder();

   void  order(ORDER pOrder);
   ORDER order();

protected:
   ORDER cOrder;
};


// -----------------------------------------------------------------------

// QpRecDimension
// -----------------------------------------------------------------------

// QpRecName

// -----------------------------------------------------------------------

class QpRecEmptyCell : public QpRecCell
{
public:
   QpRecEmptyCell(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecEmptyCell();
};


// -----------------------------------------------------------------------

class QpRecIntegerCell : public QpRecCell
{
public:
   QpRecIntegerCell(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecIntegerCell();


   QP_INT16 integer();
protected:
   QP_INT16 cInt;
};

// -----------------------------------------------------------------------

class QpRecFloatingPointCell : public QpRecCell
{
public:
   QpRecFloatingPointCell(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecFloatingPointCell();
 
   QP_INT64 value();
protected:
   QP_INT64 cValue;
};

// -----------------------------------------------------------------------

class QpRecFormulaCell : public QpRecCell
{
public:
   QpRecFormulaCell(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecFormulaCell();
 
   const char* formula();
   QP_INT16    formulaReferences();
   QP_INT16    formulaLen();

protected:
   QP_INT16    cCellRef;
   char*       cFormula;
   QP_INT64    cLastValue;
   QP_INT16    cLen;
   QP_INT16    cState;
};

// -----------------------------------------------------------------------

class QpRecLabelCell : public QpRecCell
{
public:
   QpRecLabelCell(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecLabelCell();
 
   char        labelPrefix();
   const char* label();

protected:
   char  cLabelPrefix;
   char* cLabel;
};                                                                                                                                 

// -----------------------------------------------------------------------

class QpRecUnknown : public QpRec
{
public:
   QpRecUnknown(QP_INT16 pType, QP_INT16 pLen, QpIStream& pIn);
   ~QpRecUnknown();
};

// -----------------------------------------------------------------------

class QpRecBop : public QpRec
{
public:
    QpRecBop(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecBop();

   QP_UINT8 pageIndex();

protected:
   QP_UINT8 cPageIndex;
};

// -----------------------------------------------------------------------
 
class QpRecPageName : public QpRec
{
public:
    QpRecPageName(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecPageName();
 
   const char* pageName();
 
protected:
   char* cPageName;
};

// -----------------------------------------------------------------------
 
class QpRecPassword : public QpRec
{
public:
   QpRecPassword(QP_INT16 pLen, QpIStream& pIn);
   ~QpRecPassword();

   const QP_UINT8* password();

protected:
   QP_UINT8* cPassword;
};

#endif // QPRO_RECORD_H

