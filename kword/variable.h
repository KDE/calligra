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

class KWordDocument;

enum VariableType {VT_DATE = 0,VT_TIME = 1,VT_PGNUM = 2,VT_NUMPAGES = 3,VT_NONE};
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

};

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/

class KWVariablePgNumFormat : public KWVariableFormat
{
public:
  KWVariablePgNumFormat();

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

  void setVariableFormat(KWVariableFormat *_varFormat,bool _deleteOld = true)
    { if (_deleteOld && varFormat) delete varFormat; varFormat = _varFormat; }
  KWVariableFormat *getVariableFormat()
    { return varFormat; }

  QString getText()
    { return text; }

  virtual void recalc() {}

protected:
  KWordDocument *doc;
  KWVariableFormat *varFormat;
  QString text;

};

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

class KWPgNumVariable : public KWVariable
{
public:
  KWPgNumVariable(KWordDocument *_doc);

  virtual VariableType getType()
    { return VT_PGNUM; }
  virtual void recalc();

protected:
  long unsigned int pgNum;

};

#endif
