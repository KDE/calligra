#/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: gradient collection                                    */
/******************************************************************/

#include "kpgradientcollection.h"
#include "kpgradientcollection.moc"

/******************************************************************/
/* Class: KPGradientCollection                                    */
/******************************************************************/

/*======================= get Gradient ===========================*/
QPixmap* KPGradientCollection::getGradient(QColor _color1,QColor _color2,BCType _bcType,KSize _size,bool addref = true)
{
  int num = inGradientList(_color1,_color2,_bcType,_size);

  if (num == -1)
    {
      KPGradient *kpgradient = new KPGradient(_color1,_color2,_bcType,_size);
      gradientList.append(kpgradient);

#ifdef SHOW_INFO
      debug("-------------");
      debug("add Gradient");
      debug("GradientCollection count: %d",gradientList.count());
      debug("%d: ",gradientList.count() - 1);
#endif

      if (addref)
	kpgradient->addRef();

#ifdef SHOW_INFO
      debug("-------------");
#endif

      return kpgradient->getGradient();
    }
  else 
    {

#ifdef SHOW_INFO
      debug("-------------");
      debug("%d: ",num);
#endif

      if (addref)
	gradientList.at(num)->addRef();

#ifdef SHOW_INFO
      debug("-------------");
#endif

      return gradientList.at(num)->getGradient();
    }
}

/*====================== remove ref =============================*/
void KPGradientCollection::removeRef(QColor _color1,QColor _color2,BCType _bcType,KSize _size)
{
  int num = inGradientList(_color1,_color2,_bcType,_size);

  if (num != -1)
    {
      if (gradientList.at(num)->removeRef())
	{
	  gradientList.remove(num);

#ifdef SHOW_INFO
	  debug("remove Gradient");
	  debug("GradientCollection count: %d\n",gradientList.count());
#endif

	}
    }
}

/*========================== in gradient list? ====================*/
int KPGradientCollection::inGradientList(QColor _color1,QColor _color2,BCType _bcType,KSize _size)
{
  if (!gradientList.isEmpty())
    {
      KPGradient *kpgradient = 0;
      for (int i = 0;i < static_cast<int>(gradientList.count());i++)
	{
	  kpgradient = gradientList.at(i);
	  if (kpgradient->getColor1() == _color1 && kpgradient->getColor2() == _color2 &&
	      kpgradient->getBackColorType() == _bcType && kpgradient->getSize() == _size) 
	    return i;
	}
      return -1;
    }
  else return -1;
}

