/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Variable                                               */
/******************************************************************/

#ifndef variable_h
#define variable_h

#include <qstring.h>
#include <qdatetime.h>

class KWordDocument;
class KWVariable;

enum VariableType {VT_DATE_FIX = 0,VT_DATE_VAR = 1,VT_TIME_FIX = 2,VT_TIME_VAR = 3,VT_PGNUM = 4,VT_NUMPAGES = 5,VT_NONE};
enum VariableFormatType {VFT_DATE = 0,VFT_TIME = 1,VFT_PGNUM = 2,VFT_NUMPAGES = 3};

/******************************************************************/
/* Class: KWVariableFormat                                        */
/******************************************************************/

class KWVariableFormat
{
public:
  KWVariableFormat() {}
  virtual ~KWVariableFormat() {}

  virtual VariableFormatType getType() = 0;

  virtual void setFormat(QString _format)
  { format = _format; }
  
  virtual QString convert(KWVariable *_var) = 0;
  
protected:
  QString format;

};

/******************************************************************/
/* Class: KWVariableDateFormat                                    */
/******************************************************************/

class KWVariableDateFormat : public KWVariableFormat
{
public:
  KWVariableDateFormat() : KWVariableFormat() {}

  virtual VariableFormatType getType()
  { return VFT_DATE; }

  virtual void setFormat(QString _format);
  
  virtual QString convert(KWVariable *_var);
  
};

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/

class KWVariablePgNumFormat : public KWVariableFormat
{
public:
  KWVariablePgNumFormat() {}

  virtual VariableFormatType getType()
  { return VFT_PGNUM; }

};

/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/

class KWVariable
{
public:
  KWVariable(KWordDocument *_doc) : text() { varFormat = 0L; doc = _doc; }
  virtual ~KWVariable() {}

  virtual VariableType getType()
  { return VT_NONE; }

  void setVariableFormat(KWVariableFormat *_varFormat,bool _deleteOld = false)
  { if (_deleteOld && varFormat) delete varFormat; varFormat = _varFormat; }
  KWVariableFormat *getVariableFormat()
  { return varFormat; }

  QString getText()
  { return varFormat->convert(this); }

  virtual void setInfo(int _frameSetNum,int _frameNum,int _pageNum,int _paragNum) 
  { frameSetNum = _frameSetNum; frameNum = _frameNum; pageNum = _pageNum; paragNum = _pageNum; }
  
  virtual void recalc() {}

protected:
  KWordDocument *doc;
  KWVariableFormat *varFormat;
  QString text;
  int frameSetNum,frameNum,paragNum,pageNum;
  
};

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

class KWPgNumVariable : public KWVariable
{
public:
  KWPgNumVariable(KWordDocument *_doc) : KWVariable(_doc) {}

  virtual VariableType getType()
  { return VT_PGNUM; }
  
  virtual void recalc() {}

protected:
  long unsigned int pgNum;

};

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

class KWDateVariable : public KWVariable
{
public:
  KWDateVariable(KWordDocument *_doc,bool _fixx,QDate _date);

  virtual VariableType getType()
  { return fix ? VT_DATE_FIX : VT_DATE_VAR; }

  virtual void recalc();

protected:
  QDate date;
  bool fix;
  
};

#endif
