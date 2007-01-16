#ifndef KPTPERTEDITOR_H
#define KPTPERTEDITOR_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QTreeWidget>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>

#include <kactionselector.h>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;

class PertEditor : public ViewBase
{
    Q_OBJECT
public:

    KActionSelector * assignList;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QTableWidget *tableTaskWidget;
    QVBoxLayout *vboxLayout1;
    QLabel *label1;

    PertEditor( Part *part, QWidget *parent );

    void setupGui();
    


};

}  //KPlato namespace

#endif
