#ifndef EXAMPLE_VIEW
#define EXAMPLE_VIEW

#include <container.h>

class QAction;
class QPaintEvent;

class ExamplePart;

class ExampleView : public ContainerView
{
    Q_OBJECT
public:
    ExampleView( ExamplePart* part, QWidget* parent = 0, const char* name = 0 );
    
protected slots:
    void cut();
    
protected:
    void paintEvent( QPaintEvent* );
    
private:
    QAction* m_cut;
};

#endif
