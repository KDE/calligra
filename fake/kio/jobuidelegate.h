#ifndef FAKE_KIOJOBUIDELEGATE_H
#define FAKE_KIOJOBUIDELEGATE_H

#include "kofake_export.h"

#include <QWidget>

namespace KIO {

class JobUiDelegate : public QWidget
{
public:
    void setWindow(QWidget*) {}
};

}

#endif
