
#include "applicationspelltest.h"
#include "applicationspelltest.moc"

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qmultilineedit.h>
#include "koSpell.h"
#include <kdebug.h>
#include <qlayout.h>

ApplicationWindowSpell::ApplicationWindowSpell()
    : QMainWindow( 0, "Spell test", WDestructiveClose )
{
    QToolBar * fileTools = new QToolBar( this, "file operations" );
    fileTools->setLabel( "File Operations" );

    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );


    file->insertItem( "Spell text", this, SLOT(slotSpellText()));
    file->insertItem( "Config", this, SLOT(slotConfigSpellText()));

    multi = new QMultiLineEdit( this, "editor" );
    multi->setFocus();
    setCentralWidget( multi );
    resize( 450, 600 );
    m_spell = 0L;
    m_spellConfig = 0L;
    m_spellConfig=new KOSpellConfig();
}


ApplicationWindowSpell::~ApplicationWindowSpell()
{
    delete m_spell;
    delete m_spellConfig;
}


void ApplicationWindowSpell::slotSpellText()
{
    if ( m_spell )
        return;
    m_spell = new KOSpell(this, "test" ,m_spellConfig, true, true, true );

    QObject::connect( m_spell, SIGNAL( death() ),
                      this, SLOT( spellCheckerFinished() ) );
    QObject::connect( m_spell, SIGNAL( misspelling( const QString &, const QStringList &, unsigned int ) ),
                      this, SLOT( spellCheckerMisspelling( const QString &, const QStringList &, unsigned int ) ) );
    QObject::connect( m_spell, SIGNAL( corrected( const QString &, const QString &, unsigned int ) ),
                      this, SLOT( spellCheckerCorrected( const QString &, const QString &, unsigned int ) ) );
    QObject::connect( m_spell, SIGNAL( done( const QString & ) ),
                      this, SLOT( spellCheckerDone( const QString & ) ) );
    QObject::connect( m_spell, SIGNAL( ignoreall (const QString & ) ),
                      this, SLOT( spellCheckerIgnoreAll( const QString & ) ) );

    QObject::connect( m_spell, SIGNAL( replaceall( const QString &, const QString & )),
                      this, SLOT( spellCheckerReplaceAll( const QString &,const QString & )));
    bool result = m_spell->check( multi->text());
    if ( !result)
    {
        delete m_spell;
        m_spell=0L;
    }
}

void ApplicationWindowSpell::slotConfigSpellText()
{
    spellConfig *conf = new spellConfig( this,this );
    conf->exec();
    delete conf;
}


void ApplicationWindowSpell::spellCheckerFinished()
{
    kdDebug()<<" void ApplicationWindowSpell::spellCheckerFinished() \n";
    delete m_spell;
    m_spell= 0L;
}

void ApplicationWindowSpell::spellCheckerMisspelling( const QString &text, const QStringList &, unsigned int pos)
{
    kdDebug()<<" void ApplicationWindowSpell::spellCheckerMisspelling( const QString &text, const QStringList &, unsigned int ) :"<<text<<" pos :"<<pos<<endl;
}

void ApplicationWindowSpell::spellCheckerCorrected( const QString &orig , const QString & newWord , unsigned int pos )
{
    kdDebug()<<"void ApplicationWindowSpell::spellCheckerCorrected( const QString &, const QString &, unsigned int ) :"<<orig<<" new :"<<newWord<<" pos :"<<pos <<endl;
}

void ApplicationWindowSpell::spellCheckerDone( const QString & text)
{
    kdDebug()<<"void ApplicationWindowSpell::spellCheckerDone( const QString & ) :"<<text<<endl;
}

void ApplicationWindowSpell::spellCheckerIgnoreAll( const QString &text )
{
    kdDebug()<<"void ApplicationWindowSpell::spellCheckerIgnoreAll( const QString & ) :"<<text<<endl;
}

void ApplicationWindowSpell::spellCheckerReplaceAll( const QString &  text,  const QString & replace)
{
    kdDebug()<<" void ApplicationWindowSpell::spellCheckerReplaceAll( const QString &  ,  const QString & ) :"<<text<<" replace:"<<replace<< endl;
}

void ApplicationWindowSpell::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void ApplicationWindowSpell::changeConfig( KOSpellConfig _kspell)
{
    if(m_spellConfig==0)
        m_spellConfig=new KOSpellConfig();
  m_spellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_spellConfig->setRunTogether(_kspell.runTogether ());
  m_spellConfig->setDictionary(_kspell.dictionary ());
  m_spellConfig->setDictFromList(_kspell.dictFromList());
  m_spellConfig->setEncoding(_kspell.encoding());
}


spellConfig::spellConfig( QWidget* parent,ApplicationWindowSpell *_spell )
    : KDialogBase( parent, "urldialog", true, "config",
                   KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
    m_spell = _spell;
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    config = new KOSpellConfig( page );
    topLayout->addWidget( config );
    connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));
}

void spellConfig::slotApply()
{
    m_spell->changeConfig(*config);
}

