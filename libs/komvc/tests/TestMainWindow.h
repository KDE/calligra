#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include <KoMainWindowBase.h>
class KoControllerBase;

class TestMainWindow : public KoMainWindowBase
{
    Q_OBJECT
public:
    explicit TestMainWindow(KoControllerBase *controller);
    virtual ~TestMainWindow();

protected:

    bool addView(KoViewBase *view);

private:

    friend class TestMainWindowFactory;

    void createMenu();

    class Private;
    Private *const d;
};

class TestMainWindowFactory : public KoMainWindowFactory
{
public:
    virtual KoMainWindowBase *create(KoControllerBase *controller);
};

#endif // TESTMAINWINDOW_H
