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
class KWParag;

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
/* Class: KWVariableTimeFormat                                    */
/******************************************************************/

class KWVariableTimeFormat : public KWVariableFormat
{
public:
  KWVariableTimeFormat() : KWVariableFormat() {}

  virtual VariableFormatType getType()
  { return VFT_TIME; }

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

  virtual void setFormat(QString _format);

  virtual QString convert(KWVariable *_var);

};

/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/

class KWVariable
{
public:
  KWVariable(KWordDocument *_doc) : text() { varFormat = 0L; doc = _doc; }
  virtual ~KWVariable() {}

  virtual KWVariable *copy() {
    KWVariable *v = new KWVariable(doc);
    v->setVariableFormat(varFormat);
    v->setInfo(frameSetNum,frameNum,pageNum,parag);
    return v;
  }

  virtual VariableType getType()
  { return VT_NONE; }

  void setVariableFormat(KWVariableFormat *_varFormat,bool _deleteOld = false)
  { if (_deleteOld && varFormat) delete varFormat; varFormat = _varFormat; }
  KWVariableFormat *getVariableFormat()
  { return varFormat; }

  QString getText()
  { return varFormat->convert(this); }

  virtual void setInfo(int _frameSetNum,int _frameNum,int _pageNum,KWParag *_parag)
  { frameSetNum = _frameSetNum; frameNum = _frameNum; pageNum = _pageNum; parag = _parag; }

  virtual void recalc() {}

protected:
  KWordDocument *doc;
  KWVariableFormat *varFormat;
  QString text;
  int frameSetNum,frameNum,pageNum;
  KWParag *parag;

};

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

class KWPgNumVariable : public KWVariable
{
public:
  KWPgNumVariable(KWordDocument *_doc) : KWVariable(_doc) { pgNum = 0; }

  virtual KWVariable *copy() {
    KWPgNumVariable *var = new KWPgNumVariable(doc);
    var->setVariableFormat(varFormat);
    var->setInfo(frameSetNum,frameNum,pageNum,parag);
    return var;
  }

  virtual VariableType getType()
  { return VT_PGNUM; }

  virtual void recalc() { pgNum = pageNum; }
  long unsigned int getPgNum() { return pgNum; }


protected:
  long unsigned int pgNum;

};

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

class KWDateVariable : public KWVariable
{
public:
  KWDateVariable(KWordDocument *_doc,bool _fix,QDate _date);

  virtual KWVariable *copy() {
    KWDateVariable *var = new KWDateVariable(doc,fix,date);
    var->setVariableFormat(varFormat);
    var->setInfo(frameSetNum,frameNum,pageNum,parag);
    return var;
  }

  virtual VariableType getType()
  { return fix ? VT_DATE_FIX : VT_DATE_VAR; }

  virtual void recalc();

  QDate getDate() { return date; }
  void setDate(QDate _date) { date = _date; }

protected:
  QDate date;
  bool fix;

};

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/

class KWTimeVariable : public KWVariable
{
public:
  KWTimeVariable(KWordDocument *_doc,bool _fix,QTime _time);

  virtual KWVariable *copy() {
    KWTimeVariable *var = new KWTimeVariable(doc,fix,time);
    var->setVariableFormat(varFormat);
    var->setInfo(frameSetNum,frameNum,pageNum,parag);
    return var;
  }

  virtual VariableType getType()
  { return fix ? VT_TIME_FIX : VT_TIME_VAR; }

  virtual void recalc();

  QTime getTime() { return time; }
  void setTime(QTime _time) { time = _time; }

protected:
  QTime time;
  bool fix;

};

#endif

