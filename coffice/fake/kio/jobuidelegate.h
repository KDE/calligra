#ifndef FAKE_KIOJOBUIDELEGATE_H
#define FAKE_KIOJOBUIDELEGATE_H

namespace KIO {

class JobUiDelegate : public QWidget
{
public:
    void setWindow(QWidget*) {}
};

}

#endif
