#ifndef KSpreadPaperHeadings_included
#define KSpreadPaperHeadings_included

#include <qwidget.h>
#include <qlined.h>
#include <qlabel.h>

class KSpreadPaperHeadingsData
{
public:
    KSpreadPaperHeadingsData( QWidget* parent );

    QLineEdit* m_headLeft;
    QLineEdit* m_headMid;
    QLineEdit* m_headRight;
    QLineEdit* footLeft;
    QLineEdit* footMid;
    QLineEdit* footRight;
};

class KSpreadPaperHeadings : public QWidget, public KSpreadPaperHeadingsData
{
    Q_OBJECT
public:
    KSpreadPaperHeadings( QWidget* parent = NULL, const char* name = NULL );

    virtual ~KSpreadPaperHeadings();
};

#endif
