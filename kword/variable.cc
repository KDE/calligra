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
  if (_var->getType() != VT_DATE_FIX)
    {
      warning("Can't convert variable of type %d to a date!!!",_var->getType());
      return QString();
    }
  
  // for now...
  return dynamic_cast<KWDateVariable*>(_var)->getDate().toString();
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
