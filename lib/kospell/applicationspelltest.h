
#ifndef APPLICATIONSPELL_H
#define APPLICATIONSPELL_H

#include <qmainwindow.h>
#include <kdialogbase.h>
class QMultiLineEdit;
class KOSpell;
class QWidget;
class KOSpellConfig;
class ApplicationWindowSpell;

class spellConfig : public KDialogBase
{
    Q_OBJECT
public:
   spellConfig( QWidget* parent,ApplicationWindowSpell *_spell );
    KOSpellConfig * configSpellCheck() const { return config; }
public slots:
    void slotApply();
private:
   KOSpellConfig *config;
    ApplicationWindowSpell *m_spell;
};

class ApplicationWindowSpell: public QMainWindow
{
    Q_OBJECT

public:
    ApplicationWindowSpell();
    ~ApplicationWindowSpell();
    void changeConfig( KOSpellConfig _kspell);
protected:
    void closeEvent( QCloseEvent* );

private slots:
    void slotSpellText();
    void slotConfigSpellText();
    void spellCheckerFinished();
    void spellCheckerMisspelling( const QString &, const QStringList &, unsigned int );
    void spellCheckerCorrected( const QString &, const QString &, unsigned int );
    void spellCheckerDone( const QString & );
    void spellCheckerIgnoreAll( const QString & );

    void spellCheckerReplaceAll( const QString &, const QString & );

    void slotModalSpellText();

private:
    QMultiLineEdit *multi;
    KOSpell *m_spell;
    KOSpellConfig *m_spellConfig;
};


#endif
