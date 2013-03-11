#ifndef FAKE_CALLIGRABIGS_KOASPECTBUTTON_H
#define FAKE_CALLIGRABIGS_KOASPECTBUTTON_H

#include <QPushButton>

class KoAspectButton : public QPushButton
{
public:
    KoAspectButton(QWidget *parent = 0) : QPushButton(parent) {}
    bool keepAspectRatio() const { return true; }
    void setKeepAspectRatio(bool) {}
};

#endif
