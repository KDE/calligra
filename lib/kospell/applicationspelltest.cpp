
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

    file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );


    file->insertItem( "Spell text", this, SLOT(slotSpellText()));
    m_modalSpellCheckMenuIndex = file->insertItem( "Modal Spell text", this, SLOT(slotModalSpellText()));

    file->insertItem( "Config", this, SLOT(slotConfigSpellText()));

    file->insertItem( "Quit", this, SLOT(close()));

    multi = new QMultiLineEdit( this, "editor" );
    connect( multi, SIGNAL( selectionChanged ()),this, SLOT( slotSelectionChanged()));
    multi->setFocus();
    setCentralWidget( multi );
    resize( 450, 600 );
    m_spell = 0L;
    m_spellConfig=new KOSpellConfig();
    file->setItemEnabled( m_modalSpellCheckMenuIndex, false );
}


ApplicationWindowSpell::~ApplicationWindowSpell()
{
    delete m_spell;
    delete m_spellConfig;
}

void ApplicationWindowSpell::slotSelectionChanged()
{
    file->setItemEnabled( m_modalSpellCheckMenuIndex, multi->hasSelectedText () );
}

void ApplicationWindowSpell::slotModalSpellText()
{
    QString text = multi->selectedText();
    if ( !text.isEmpty() )
    {
        kdDebug()<<" text :"<<text<<endl;
        KOSpell::modalCheck( text, 0L );
        kdDebug()<<" after : text :"<<text<<endl;
    }
}

void ApplicationWindowSpell::slotSpellText()
{
    if ( m_spell )
        return;
    m_spell =KOSpell::createKoSpell( this, "test" ,0L,0L,m_spellConfig, true, true );
//new KOSpell(this, "test" ,m_spellConfig, true, true);

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
//todo fix me parag id !!!!
    kdDebug()<<" pos :"<<pos<<" text :"<<text<<" text.length() :"<<text.length()<<endl;
    unsigned int l = 0;
    unsigned int cnt = 0;
    posToRowCol (pos, l, cnt);
    multi->setSelection(l, cnt, l, cnt+text.length());
}

void ApplicationWindowSpell::spellCheckerCorrected( const QString &orig , const QString & newWord , unsigned int pos )
{
    if( orig != newWord )
    {
        unsigned int l = 0;
        unsigned int cnt = 0;
        kdDebug()<<"void ApplicationWindowSpell::spellCheckerCorrected( const QString &, const QString &, unsigned int ) :"<<orig<<" new :"<<newWord<<" pos :"<<pos <<endl;
        posToRowCol (pos, l, cnt);
        multi->setSelection(l, cnt, l, cnt+orig.length());
        multi->removeSelectedText();
        multi->insert(newWord);
    }
}

void  ApplicationWindowSpell::posToRowCol(unsigned int pos, unsigned int &line, unsigned int &col)
{
  for (line = 0; line < static_cast<uint>(multi->lines()) && col <= pos; line++)
  {
    col += multi->paragraphLength(line)+1;
  }
  line--;
  col = pos - col + multi->paragraphLength(line) + 1;
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

