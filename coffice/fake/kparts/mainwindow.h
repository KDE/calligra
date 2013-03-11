#ifndef FAKE_KPARTSMAINWINDOW_H
#define FAKE_KPARTSMAINWINDOW_H

#include <kxmlguiwindow.h>
#include <kparts/part.h>

namespace KParts {

    class MainWindow : public KXmlGuiWindow, virtual public PartBase
    {
        Q_OBJECT
    public:
        MainWindow( QWidget* parent = 0 ) : KXmlGuiWindow(parent) {}
    public Q_SLOTS:
        virtual void configureToolbars() {}
    protected Q_SLOTS:
        void createGUI( KParts::Part * part ) {}
    virtual void slotSetStatusBarText( const QString & ) {}
        void saveNewToolbarConfig() {}
    protected:
        virtual void createShellGUI( bool create = true ) {}
    };

}

#endif
