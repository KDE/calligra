#ifndef ITEMOPTS_H
#define ITEMOPTS_H

#include <map.h>
#include <pair.h>
#include "dlgoptions.h"

class QString;
class QStringList;

class dlgItemOptions: public dlgOptions{
    Q_OBJECT
public:
    dlgItemOptions(map<QString, pair<QString, QStringList> > *p,
		   QWidget *parent = 0, const char *name = 0, WFlags f = 0);
private:
    map<QString, pair<QString, QStringList> > *props;
public slots:
    void commitProps();
};

#endif
