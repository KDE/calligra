/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Base class for objects                                 */
/******************************************************************/

#include "kpobject.h"
#include "kpobject.moc"

/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

/*======================== constructor ===========================*/
KPObject::KPObject()
  : QObject(), orig(), ext(), shadowColor(gray)
{
  presNum = 0;
  effect = EF_NONE;
  effect2 = EF2_NONE;
  angle = 0.0;
  shadowDirection = SD_RIGHT_BOTTOM;
  shadowDistance = 0;
  dSelection = true;
  selected = false;
  presFakt = 0.0;
  zoomed = false;
  ownClipping = true;
  subPresStep = 0;
  specEffects = false;
  onlyCurrStep = true;
  inObjList = true;
  cmds = 0;
  move = false;
}

/*======================= get bounding rect ======================*/
QRect KPObject::getBoundingRect(int _diffx,int _diffy)
{
  QRect r(orig.x() - _diffx,orig.y() - _diffy,
	  ext.width(),ext.height());

  if (shadowDistance > 0)
    {
      int sx = r.x(),sy = r.y();
      getShadowCoords(sx,sy,shadowDirection,shadowDistance);
      QRect r2(sx,sy,r.width(),r.height());
      r = r.unite(r2);
    }

  if (angle == 0.0)
    return r;
  else
    {
      QWMatrix mtx;
      mtx.rotate(angle);
      QRect rr = mtx.map(r);
      
      int diffw = abs(rr.width() - r.width());
      int diffh = abs(rr.height() - r.height());
      
      return QRect(r.x() - diffw,r.y() - diffh,
		   r.width() + diffw * 2,r.height() + diffh * 2);
    }

  return r;
}

/*======================== contain point ? =======================*/
bool KPObject::contains(QPoint _point,int _diffx,int _diffy)
{
  QRect r(orig.x() - _diffx,orig.y() - _diffy,
	  ext.width(),ext.height());

  return r.contains(_point);
}

/*======================== get cursor ============================*/
QCursor KPObject::getCursor(QPoint _point,int _diffx,int _diffy,ModifyType &_modType)
{
  int px = _point.x();
  int py = _point.y();

  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();

  QRect r(ox,oy,ow,oh);

  if (!r.contains(_point))
    return arrowCursor;

  if (px >= ox && py >= oy && px <= ox + 6 && py <= oy + 6)
    {
      _modType = MT_RESIZE_LU;
      return sizeFDiagCursor;
    }

  if (px >= ox && py >= oy + oh / 2 - 3 && px <= ox + 6 && py <= oy + oh / 2 + 3)
    {
      _modType = MT_RESIZE_LF;
      return sizeHorCursor;
    }

  if (px >= ox && py >= oy + oh - 6 && px <= ox + 6 && py <= oy + oh)
    {
      _modType = MT_RESIZE_LD;
      return sizeBDiagCursor;
    }

  if (px >= ox + ow / 2 - 3 && py >= oy && px <= ox + ow / 2 + 3 && py <= oy + 6)
    {
      _modType = MT_RESIZE_UP;
      return sizeVerCursor;
    }

  if (px >= ox + ow / 2 - 3 && py >= oy + oh - 6 && px <= ox + ow / 2 + 3 && py <= oy + oh)
    {
      _modType = MT_RESIZE_DN;
      return sizeVerCursor;
    }

  if (px >= ox + ow - 6 && py >= oy && px <= ox + ow && py <= oy + 6)
    {
      _modType = MT_RESIZE_RU;
      return sizeBDiagCursor;
    }

  if (px >= ox + ow - 6 && py >= oy + oh / 2 - 3 && px <= ox + ow && py <= oy + oh / 2 + 3)
    {
      _modType = MT_RESIZE_RT;
      return sizeHorCursor;
    }

  if (px >= ox + ow - 6 && py >= oy + oh - 6 && px <= ox + ow && py <= oy + oh)
    {
      _modType = MT_RESIZE_RD;
      return sizeFDiagCursor;
    }

  _modType = MT_MOVE;
  return sizeAllCursor;
}

/*========================= zoom =================================*/
void KPObject::zoom(float _fakt)
{
  presFakt = _fakt;

  zoomed = true;

  oldOrig = orig;
  oldExt = ext;

  orig.setX(static_cast<int>(static_cast<float>(orig.x()) * presFakt));
  orig.setY(static_cast<int>(static_cast<float>(orig.y()) * presFakt));
  ext.setWidth(static_cast<int>(static_cast<float>(ext.width()) * presFakt));
  ext.setHeight(static_cast<int>(static_cast<float>(ext.height()) * presFakt));

  setSize(ext);
  setOrig(orig);
}

/*==================== zoom orig =================================*/
void KPObject::zoomOrig()
{
  orig = oldOrig;
  ext = oldExt;

  setSize(ext);
  setOrig(orig);
}

/*======================== draw ==================================*/
void KPObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  if (dSelection && selected)
    {
      _painter->save();
      QRect r = _painter->viewport();
      
      _painter->setViewport(orig.x() - _diffx,orig.y() - _diffy,r.width(),r.height());
      paintSelection(_painter);

      _painter->setViewport(r);
      _painter->restore();
    }
}

/*====================== get shadow coordinates ==================*/
void KPObject::getShadowCoords(int& _x,int& _y,ShadowDirection _direction,int _distance)
{
  int sx = 0,sy = 0;

  switch (shadowDirection)
    {
    case SD_LEFT_UP:
      {
	sx = _x - shadowDistance;
	sy = _y - shadowDistance;
      } break;
    case SD_UP:
      {
	sx = _x;
	sy = _y - shadowDistance;
      } break;
    case SD_RIGHT_UP:
      {
	sx = _x + shadowDistance;
	sy = _y - shadowDistance;
      } break;
    case SD_RIGHT:
      {
	sx = _x + shadowDistance;
	sy = _y;
      } break;
    case SD_RIGHT_BOTTOM:
      {
	sx = _x + shadowDistance;
	sy = _y + shadowDistance;
      } break;
    case SD_BOTTOM:
      {
	sx = _x;
	sy = _y + shadowDistance;
      } break;
    case SD_LEFT_BOTTOM:
      {
	sx = _x - shadowDistance;
	sy = _y + shadowDistance;
      } break;
    case SD_LEFT:
      {
	sx = _x - shadowDistance;
	sy = _y;
      } break;
    }

  _x = sx; _y = sy;
}

/*======================== paint selection =======================*/
void KPObject::paintSelection(QPainter *_painter)
{
  _painter->save();
  RasterOp rop = _painter->rasterOp();

  _painter->setRasterOp(NotROP);
  
  _painter->fillRect(0,0,6,6,black);
  _painter->fillRect(0,ext.height() / 2 - 3,6,6,black);
  _painter->fillRect(0,ext.height() - 6,6,6,black);
  _painter->fillRect(ext.width() - 6,0,6,6,black);
  _painter->fillRect(ext.width() - 6,ext.height() / 2 - 3,6,6,black);
  _painter->fillRect(ext.width() - 6,ext.height() - 6,6,6,black);
  _painter->fillRect(ext.width() / 2 - 3,0,6,6,black);
  _painter->fillRect(ext.width() / 2 - 3,ext.height() - 6,6,6,black);

  _painter->setRasterOp(rop);
  _painter->restore();
}

/*======================== do delete =============================*/
void KPObject::doDelete()
{
  if (cmds == 0 && !inObjList) delete this;
}
