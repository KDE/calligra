/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
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
    QString pwDiv;
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
  void load(const char*);
  void newAutoform();
  void save(const char*);

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
  const int ST_WIDTH    = 1;
  const int ST_HEIGHT   = 2;
  const int ST_VARIABLE = 3;
  const int ST_NUMBER   = 4;
  const int ST_OPERATOR = 5;

  // operators
  const char OP_EQUAL   = '=';
  const char OP_PLUS    = '+';
  const char OP_MINUS   = '-';
  const char OP_MULT    = '*';
  const char OP_DIV     = '/';
  const char COMMENT    = '#';

  // block (structure) beginnings - ends
  const char PNT_BG[]   = "POINT{";
  const char X_BG[]     = "X{";
  const char Y_BG[]     = "Y{";
  const char ATTR_BG[]  = "ATTRIB{";
  const char END[]       = "}";

  // allowed variables
  const char VAR_1      = 'a';
  const char VAR_2      = 'b';
  const char VAR_3      = 'c';
  const char VAR_4      = 'd';
  const char VAR_5      = 'e';
  const char VAR_6      = 'f';
  const char VAR_X      = 'x';
  const char VAR_Y      = 'y';
  const char VAR_VARIA  = 'v';
  const char VAR_PW     = 'p';
  const char VAR_W      = 'w';
  const char VAR_H      = 'h';

  // level (depth) of the syntax
  const int LEVEL_NULL  = 0;
  const int LEVEL_POINT = 1;
  const int LEVEL_X     = 2;
  const int LEVEL_Y     = 3;
  const int LEVEL_ATTR  = 4;

  // numbers
  const char NUM_0      = '0';
  const char NUM_1      = '1';
  const char NUM_2      = '2';
  const char NUM_3      = '3';
  const char NUM_4      = '4';
  const char NUM_5      = '5';
  const char NUM_6      = '6';
  const char NUM_7      = '7';
  const char NUM_8      = '8';
  const char NUM_9      = '9';

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



