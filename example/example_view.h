#ifndef EXAMPLE_VIEW
#define EXAMPLE_VIEW

#include <koView.h>

class KAction;
class QPaintEvent;

class ExamplePart;

class ExampleView : public KoView
{
    Q_OBJECT
public:
    ExampleView( ExamplePart* part, QWidget* parent = 0, const char* name = 0 );

protected slots:
    void cut();

protected:
    void paintEvent( QPaintEvent* );

    virtual void updateReadWrite( bool readwrite );

private:
    KAction* m_cut;
};

#endif
