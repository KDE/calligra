/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Autoform Interpreter (header)                          */
/******************************************************************/

#ifndef ATFINTERP_H
#define ATFINTERP_H

#include <qobject.h>
#include <qstack.h>
#include <qpntarry.h>
#include <qstring.h>
#include <qlist.h>
#include <qfile.h>
#include <qstrlist.h>
#include <qcstring.h>

/******************************************************************/
/* class ATFInterpreter                                           */
/******************************************************************/

class ATFInterpreter : public QObject
{
  Q_OBJECT

public:

  // structure of an attribute list
  struct AttribList
  {
    bool isVariable;
    int pwDiv;
  };

  // coordinate structure
  struct CoordStruct
  {
    QString a;
    QString b;
    QString c;
    QString d;
    QString e;
    QString f;
    QString result;
  };

  // attribute structure
  struct AttribStruct
  {
    QString isVariable;
    QCString pwDiv;
  };

  //point structure
  struct PointStruct
  {
    CoordStruct x;
    CoordStruct y;
    AttribStruct attrib;
  };

  // constructure - destructure
  ATFInterpreter(QObject* parent=0,const char* name=0);
  ~ATFInterpreter();

  // load/new/save autifirm
  void load(const QString &);
  void newAutoform();
  void save(const QString &);

  // get pointarray/attribute list/points
  QPointArray getPointArray(int,int);
  QList<AttribList> getAttribList();
  QList<PointStruct> getPoints() {return pointList;}

  // change a variable
  void changeVar(int,int,int,QString);

  // get nuber of points
  int getNumOfPoints();

  // insert - delete point
  void insertPoint(int,bool);
  void deletePoint(int);

protected:

  // fields
  static const int ST_WIDTH    = 1;
  static const int ST_HEIGHT   = 2;
  static const int ST_VARIABLE = 3;
  static const int ST_NUMBER   = 4;
  static const int ST_OPERATOR = 5;

  // operators
  static const char OP_EQUAL   = '=';
  static const char OP_PLUS    = '+';
  static const char OP_MINUS   = '-';
  static const char OP_MULT    = '*';
  static const char OP_DIV     = '/';
  static const char COMMENT    = '#';

  // block (structure) beginnings - ends
  static const char PNT_BG[];
  static const char X_BG[];
  static const char Y_BG[];
  static const char ATTR_BG[];
  static const char END[];

  // allowed variables
  static const char VAR_1      = 'a';
  static const char VAR_2      = 'b';
  static const char VAR_3      = 'c';
  static const char VAR_4      = 'd';
  static const char VAR_5      = 'e';
  static const char VAR_6      = 'f';
  static const char VAR_X      = 'x';
  static const char VAR_Y      = 'y';
  static const char VAR_VARIA  = 'v';
  static const char VAR_PW     = 'p';
  static const char VAR_W      = 'w';
  static const char VAR_H      = 'h';

  // level (depth) of the syntax
  static const int LEVEL_NULL  = 0;
  static const int LEVEL_POINT = 1;
  static const int LEVEL_X     = 2;
  static const int LEVEL_Y     = 3;
  static const int LEVEL_ATTR  = 4;

  // numbers
  static const char NUM_0      = '0';
  static const char NUM_1      = '1';
  static const char NUM_2      = '2';
  static const char NUM_3      = '3';
  static const char NUM_4      = '4';
  static const char NUM_5      = '5';
  static const char NUM_6      = '6';
  static const char NUM_7      = '7';
  static const char NUM_8      = '8';
  static const char NUM_9      = '9';

  // structure of signs
  struct Sign
  {
    char op;
    int num;
    char var;
    int type;
  };

  // structure of values
  struct Value
  {
    QList<Sign> var1;
    QList<Sign> var2;
    QList<Sign> var3;
    QList<Sign> var4;
    QList<Sign> var5;
    QList<Sign> var6;
    QList<Sign> result;
  };

  // structure of coordinates
  struct Coord
  {
    Value pntX;
    Value pntY;
    bool isVariable;
    int pwDiv;
  };

  // structure of an integer
  struct ls
  {
    int l;
  };

  // interpret the code
  void interpret();

  // simplify a line
  QString simplify(QString);

  // get varaible of a line
  QList<Sign> getVar(QString);

  // make lines from interpretded code
  void makeLines();

  // stretch a line
  QString stretch(QString);

  // if a char is a number
  bool isNum(char);

  // ********** variables **********

  // list of coordinates and pointers to coordinate/sign/value
  QList<Coord> coordList;
  Coord *coordPtr;
  Sign *signPtr;
  Value *valuePtr;

  // list of lines and points and pointer to a point
  QStrList lines;
  QList<PointStruct> pointList;
  PointStruct *pntPtr;

};

#endif //ATFINTERP_H



