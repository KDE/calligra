#ifndef KSpreadScripts_included
#define KSpreadScripts_included

#include <qdialog.h>
#include <qstrlist.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include "kmetaedit.h"

class KSpreadScriptsData
{
public:

    KSpreadScriptsData( QWidget* parent );

    QListBox* list;
    QPushButton* del;
    QPushButton* add;
    QLineEdit* add2;
    QPushButton* edit;
    QPushButton* rename;
    QLineEdit* rename2;
};

class KSpreadScripts : public QDialog, private KSpreadScriptsData
{
    Q_OBJECT
public:
    KSpreadScripts( QWidget* parent = NULL, const char* name = NULL );
    virtual ~KSpreadScripts();

public slots:
    void slotEdit();
    void slotDelete();
    void slotAdd();
    void slotRename();
    
    void slotHighlighted( int );
    void slotSelected( int );

protected:
    void updateList();

    QStrList nameList;
    
    KMetaEditor *editor;
};

#endif
