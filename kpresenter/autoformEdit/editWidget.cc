/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: edit widget                                            */
/******************************************************************/

#include "editWidget.h"
#include "editWidget.moc"

/*================================================================*/
/* Class: EditWidget                                              */
/*================================================================*/

/*==================== constructor ===============================*/
EditWidget::EditWidget( QWidget *parent, const char *name )
    : QWidget(parent,name)
{
    QString pixdir;
    QPixmap pixmap;

    pixdir = KApplication::kde_toolbardir();

    ok = new KButton(this);
    pixmap.load(pixdir+"/tick.xpm");
    ok->setPixmap(pixmap);
    QToolTip::add(ok,"Take the changes");

    cancel = new KButton(this);
    pixmap.load(pixdir+"/exit.xpm");
    cancel->setPixmap(pixmap);
    QToolTip::add(cancel,"Don't take the changes");

    lineEdit = new QLineEdit(this);
    treeList = new KTreeList(this);

    connect(treeList,SIGNAL(selected(int)),this,SLOT(itemSelected(int)));
    connect(treeList,SIGNAL(highlighted(int)),this,SLOT(itemSelected(int)));
    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(itemChanged()));
    connect(ok,SIGNAL(clicked()),this,SLOT(itemChanged()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(cancelInput()));

    show();
    setMinimumSize(100,100);
}

/*=================== destrcutor =================================*/
EditWidget::~EditWidget()
{
    delete ok;
    delete cancel;
    delete lineEdit;
    delete treeList;
}

/*======================= delete point ===========================*/
void EditWidget::deletePoint()
{
    if (treeList->getCurrentItem())
    {
        KTreeListItem *item = treeList->getCurrentItem();
        QString str = item->getText();
        if (str.left(5) == "Point")
        {
            QString str = item->getText();
            int pnt = str.right(2).toInt() - 1;
            changed = true;
            emit delPnt(pnt);
        }
    }
}

/*====================== resize event ============================*/
void EditWidget::resizeEvent(QResizeEvent *e)
{
    int h;

    QWidget::resizeEvent(e);

    lineEdit->resize(lineEdit->sizeHint());
    h = lineEdit->height();

    ok->resize(h,h);
    cancel->resize(h,h);

    cancel->move(5,5);
    ok->move(cancel->x()+cancel->width()+5,cancel->y());
    lineEdit->move(ok->x()+ok->width()+10,ok->y());

    lineEdit->resize(width()-5-lineEdit->x(),lineEdit->height());

    treeList->move(0,ok->y()+ok->height()+5);
    treeList->resize(width(),height()-treeList->y());
}

/*================== parse the source ============================*/
void EditWidget::parseSource()
{
    KTreeListItem *point;
    KTreeListItem *crd,*attrib;
    KTreeListItem *a,*b,*c,*d,*e,*f,*result;
    KTreeListItem *isVariable,*pwDiv;
    QString tmp = "",tmp1 = "";
    unsigned int i = 1, j = 0;
    ATFInterpreter::CoordStruct coord;

    treeList->clear();
    if (!pointList.isEmpty())
    {
        for (pntPtr = pointList.first();pntPtr != 0;pntPtr = pointList.next(),i++)
        {
            tmp = "Point ";
            tmp1.setNum(i);
            tmp += tmp1;
            point = new KTreeListItem(tmp,0);
            for (j = 1;j <= 2;j++)
            {
                if (j == 1)
                {
                    coord = pntPtr->x;
                    crd = new KTreeListItem("X",0);
                }
                else
                {
                    coord = pntPtr->y;
                    crd = new KTreeListItem("Y",0);
                }
                if (!coord.a.isEmpty())
                    a = new KTreeListItem(qstrdup(stretch(coord.a)),0);
                else
                    a = new KTreeListItem("a = 0",0);
                if (!coord.b.isEmpty())
                    b = new KTreeListItem(qstrdup(stretch(coord.b)),0);
                else
                    b = new KTreeListItem("b = 0",0);
                if (!coord.c.isEmpty())
                    c = new KTreeListItem(qstrdup(stretch(coord.c)),0);
                else
                    c = new KTreeListItem("c = 0",0);
                if (!coord.d.isEmpty())
                    d = new KTreeListItem(qstrdup(stretch(coord.d)),0);
                else
                    d = new KTreeListItem("d = 0",0);
                if (!coord.e.isEmpty())
                    e = new KTreeListItem(qstrdup(stretch(coord.e)),0);
                else
                    e = new KTreeListItem("e = 0",0);
                if (!coord.f.isEmpty())
                    f = new KTreeListItem(qstrdup(stretch(coord.f)),0);
                else
                    f = new KTreeListItem("f = 0",0);
                if (!coord.result.isEmpty())
                    result = new KTreeListItem(qstrdup(stretch(coord.result)),0);
                else
                    if (j == 1)
                        result = new KTreeListItem("x = a",0);
                    else
                        result = new KTreeListItem("y = a",0);
                crd->insertChild(-1,a);
                crd->insertChild(-1,b);
                crd->insertChild(-1,c);
                crd->insertChild(-1,d);
                crd->insertChild(-1,e);
                crd->insertChild(-1,f);
                crd->insertChild(-1,result);
                point->insertChild(-1,crd);
            }
            attrib = new KTreeListItem("Attributes",0);
            if (!pntPtr->attrib.isVariable.isEmpty())
                isVariable = new KTreeListItem(qstrdup(stretch(pntPtr->attrib.isVariable)),0);
            else
                isVariable= new KTreeListItem("v = 0",0);
            if (!pntPtr->attrib.pwDiv.isEmpty())
                pwDiv = new KTreeListItem(qstrdup(stretch(pntPtr->attrib.pwDiv)),0);
            else
                pwDiv = new KTreeListItem("p = 1",0);
            attrib->insertChild(-1,isVariable);
            attrib->insertChild(-1,pwDiv);
            point->insertChild(-1,attrib);
            treeList->insertItem(point,-1);
        }
    }
}

/*================== stretch a line =============================*/
QString EditWidget::stretch(QString s)
{
    QString res = "";
    unsigned int i;

    if (!s.isEmpty())
    {
        for (i=0;i < s.length()-1;i++)
        {
            res += s.at(i);
            if ((isNum((char)QChar(s.at(i))) && !isNum((char)QChar(s.at(i+1)))) || (!isNum((char)QChar(s.at(i)))))
                res += " ";
        }
        res += s.at(s.length()-1);
    }
    return res;
}

/*====================== simplyfy a string =======================*/
QString EditWidget::simplify(QString s)
{
    QString res;
    QString str = s.stripWhiteSpace();

    for (unsigned int i=0;i < str.length();i++)
        if (str.at(i) != ' ') res.insert(res.length(),str.at(i));
    return res;
}

/*====================== is a number ============================*/
bool EditWidget::isNum(char c)
{
    if (c >= 48 && c <= 57) return true;
    else return false;
}

/*====================== is a variable ==========================*/
bool EditWidget::isVar(char c)
{
    if (c >= 97 && c <= 102) return true;
    if (c == 'w' || c == 'h') return true;
    else return false;
}

/*====================== is a operator ==========================*/
bool EditWidget::isOperator(char c)
{
    if (c == '+' || c == '-' || c == '*' || c == '/') return true;
    else return false;
}

/*================== test if the input is ok ====================*/
bool EditWidget::isInputOk(QString str,int structur,int var)
{
    str = simplify(str);
    if (!str.isEmpty())
    {
        unsigned int i;
        int current;

        const int varNum = 0;
        const int op     = 1;
        const int numOp  = 2;

        if (structur < 2)
        {
            switch (var)
            {
            case 0: if (str.at(0) != 'a') return false; break;
            case 1: if (str.at(0) != 'b') return false; break;
            case 2: if (str.at(0) != 'c') return false; break;
            case 3: if (str.at(0) != 'd') return false; break;
            case 4: if (str.at(0) != 'e') return false; break;
            case 5: if (str.at(0) != 'f') return false; break;
            case 6:
            {
                if (structur == 0)
                    if (str.at(0) != 'x') return false;
                if (structur == 1)
                    if (str.at(0) != 'y') return false;
            } break;
            }
            if (str.at(1) != '=') return false;
            current = varNum;
            for (i=2;i < str.length();i++)
            {
                switch (current)
                {
                case op:
                {
                    if (!isOperator((char)QChar(str.at(i)))) return false;
                    current = varNum;
                } break;
                case varNum:
                {
                    if (!(isVar((char)QChar(str.at(i))) || isNum((char)QChar(str.at(i))))) return false;
                    current = numOp;
                } break;
                case numOp:
                {
                    if (!(isNum((char)QChar(str.at(i))) || isOperator((char)QChar(str.at(i))))) return false;
                    if (isNum((char)QChar(str.at(i)))) current = op;
                    if (isOperator((char)QChar(str.at(i)))) current = varNum;		
                } break;
                }
            }
            if (current == varNum) return false;
        }
        else
        {
            switch (var)
            {
            case 0:
            {
                if (str.at(0) != 'v') return false;
                if (str.at(1) != '=') return false;
                if (!(str.at(2) == '0' || str.at(2) == '1')) return false;
            } break;
            case 1:
            {
                if (str.at(0) != 'p') return false;
                if (str.at(1) != '=') return false;
                if (!isNum((char)QChar(str.at(2)))) return false;
            } break;
            }
        }
        return true;
    } else return false;
}

/*====================== item selected ==========================*/
void EditWidget::itemSelected(int index)
{
    if (!treeList->itemAt(index)->hasChild())
        lineEdit->setText(treeList->itemAt(index)->getText());
    else
        lineEdit->setText("");
}

/*====================== item changed ===========================*/
void EditWidget::itemChanged()
{
    QString str = stretch(simplify(lineEdit->text()));

    if (treeList->getCurrentItem() && !str.isEmpty())
    {
        KTreeListItem *item = treeList->getCurrentItem();
        if (!item->hasChild())
        {
            KTreeListItem *parent1 = item->getParent();
            KTreeListItem *parent2 = parent1->getParent();
            QString istr = item->getText();
            QString p1str = parent1->getText();
            QString p2str = parent2->getText();
            int pnt = p2str.right(2).simplifyWhiteSpace().toInt() - 1;
            int structur = 2;
            if (p1str == "X") structur = 0;
            else if (p1str == "Y") structur = 1;
            int var = 6;
            if (istr.left(1) == "a" || istr.left(1) == "v") var = 0;
            else if (istr.left(1) == "b" || istr.left(1) == "p") var = 1;
            else if (istr.left(1) == "c") var = 2;
            else if (istr.left(1) == "d") var = 3;
            else if (istr.left(1) == "e") var = 4;
            else if (istr.left(1) == "f") var = 5;
            if (isInputOk(str,structur,var))
            {
                treeList->changeItem(str,0,treeList->currentItem());
                lineEdit->setText(str);
                emit changeVar(pnt,structur,var,str);
                changed = true;
            }
            else
                QMessageBox::warning(this,"Error","Invalid input! Please correct it!");
        }
    }
}

/*====================== cancel input ===========================*/
void EditWidget::cancelInput()
{
    lineEdit->setText("");
    if (treeList->currentItem() != -1) itemSelected(treeList->currentItem());
}






