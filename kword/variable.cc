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

#include "variable.h"
#include "parag.h"

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/

/*================================================================*/
void KWVariablePgNumFormat::setFormat(QString _format)
{
  KWVariableFormat::setFormat(_format);
}

/*================================================================*/
QString KWVariablePgNumFormat::convert(KWVariable *_var)
{
  if (_var->getType() != VT_PGNUM)
    {
      warning("Can't convert variable of type %d to a page num!!!",_var->getType());
      return QString();
    }

  QString str;
  str.setNum(dynamic_cast<KWPgNumVariable*>(_var)->getPgNum());
  str.prepend(pre);
  str.append(post);
  return QString(str);
}

/******************************************************************/
/* Class: KWVariableDateFormat                                    */
/******************************************************************/

/*================================================================*/
void KWVariableDateFormat::setFormat(QString _format)
{
  KWVariableFormat::setFormat(_format);
}

/*================================================================*/
QString KWVariableDateFormat::convert(KWVariable *_var)
{
  if (_var->getType() != VT_DATE_FIX && _var->getType() != VT_DATE_VAR)
    {
      warning("Can't convert variable of type %d to a date!!!",_var->getType());
      return QString();
    }

  // for now...
  return dynamic_cast<KWDateVariable*>(_var)->getDate().toString();
}

/******************************************************************/
/* Class: KWVariableTimeFormat                                    */
/******************************************************************/

/*================================================================*/
void KWVariableTimeFormat::setFormat(QString _format)
{
  KWVariableFormat::setFormat(_format);
}

/*================================================================*/
QString KWVariableTimeFormat::convert(KWVariable *_var)
{
  if (_var->getType() != VT_TIME_FIX && _var->getType() != VT_TIME_VAR)
    {
      warning("Can't convert variable of type %d to a time!!!",_var->getType());
      return QString();
    }

  // for now...
  return dynamic_cast<KWTimeVariable*>(_var)->getTime().toString();
}

/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/

/*================================================================*/
void KWVariable::save(ostream &out)
{
  out << indent << "<TYPE type=\"" << static_cast<int>(getType()) << "\"/>" << endl;
  out << indent << "<POS frameSet=\"" << frameSetNum << "\" frame=\"" << frameNum 
      << "\" pageNum=\"" << pageNum << "\"/>" << endl;
}

/*================================================================*/
void KWVariable::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

/*================================================================*/
void KWPgNumVariable::save(ostream &out)
{
  KWVariable::save(out);
  out << indent << "<PGNUM value=\"" << pgNum << "\"/>" << endl;
}

/*================================================================*/
void KWPgNumVariable::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
}

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

/*================================================================*/
KWDateVariable::KWDateVariable(KWordDocument *_doc,bool _fix,QDate _date)
  : KWVariable(_doc), fix(_fix)
{
  if (!fix)
    date = QDate::currentDate();
  else
    date = _date;

  recalc();
}

/*================================================================*/
void KWDateVariable::recalc()
{
}

/*================================================================*/
void KWDateVariable::save(ostream &out)
{
  KWVariable::save(out);
  out << indent << "<DATE year=\"" << date.year() << "\" month=\"" << date.month()
      << "\" day=\"" << date.day() << "\" fix=\"" << static_cast<int>(fix) << "\"/>" << endl;
}

/*================================================================*/
void KWDateVariable::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
}

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/

/*================================================================*/
KWTimeVariable::KWTimeVariable(KWordDocument *_doc,bool _fix,QTime _time)
  : KWVariable(_doc), fix(_fix)
{
  if (!fix)
    time = QTime::currentTime();
  else
    time = _time;

  recalc();
}

/*================================================================*/
void KWTimeVariable::recalc()
{
}

/*================================================================*/
void KWTimeVariable::save(ostream &out)
{
  KWVariable::save(out);
  out << indent << "<TIME hour=\"" << time.hour() << "\" minute=\"" << time.minute()
      << "\" second=\"" << time.second() << "\" msecond=\"" << time.msec()
      << "\" fix=\"" << static_cast<int>(fix) << "\"/>" << endl;
}

/*================================================================*/
void KWTimeVariable::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
}


