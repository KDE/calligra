/**
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * Bilibao@ouverture.it
 */

#include "part.h"
#include <qfile.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include <kurl.h>
#include <kapp.h>
#include <kmessagebox.h>
#include <qstack.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qprinter.h>

FormulaEditor::FormulaEditor( KPartShell *_shell, QWidget *_parent ) : KPart( _shell, _parent )
{
  // This widget will contain the widget on which you will paint,
    // scrollbars and perhaps some additional rulers.
    kformulaw = new Kformulaw( this );
    widget = kformulaw;

    // Tell the shell that we live
    shell->insertDocument( this );

    // If your widget contains children ( for example the widget
    // may contain scrollbars, rulers and a canvas, then you have to add
    // the following 2 lines for every child widget, except the scrollbars.
    // The event filter makes this part the active part if you click on it.
    widget->installEventFilter( this );
    widget->setFocusPolicy( QWidget::ClickFocus );
    // 'widget' is covered. So we want to installe the event filter in
    // the visible widget. This widget will in addition receive the keyboard
    // events if it is active. Clicking on this widget will make your
    // part the active one.
    kformulaw->getPaperWidget()->installEventFilter( this );
    kformulaw->getPaperWidget()->setFocusPolicy( QWidget::ClickFocus );

    
    childList.setAutoDelete( TRUE );
    
    documentChanged = FALSE;
}

void FormulaEditor::activate( bool _status )
{
    if ( _status )
    {
    /****************************************************************
     *
     * Here you may write some code that changes the title bar
     * of the window if your part becomes the active one.
     *
     ****************************************************************/
	QString tmp;
	if ( fileURL.isEmpty() )
	{
	    tmp.sprintf( "FormulaEditor" );
	    shell->setCaption( this, tmp.data() );
	}
	else
	{
	    tmp.sprintf( "FormulaEditor: %s", fileURL.data() );
	    shell->setCaption( this, tmp.data() );
	}
	
	widget->setFocus();
    }

    KPart::activate( _status );
}

void FormulaEditor::showFileMenu()
{
    /****************************************************************
     *
     * In this function you must construct your file menu.
     *
     ****************************************************************/

    // File Menu
    QPopupMenu *file = new QPopupMenu;
    CHECK_PTR( file );
    file->insertItem( "&New", this, SLOT(slotNewWindow()), ALT+Key_N );
    file->insertSeparator();
    file->insertItem( "&Save", this, SLOT(slotSave()), ALT+Key_S );
    file->insertItem( "Save &As", this, SLOT(slotSaveAs()), ALT+Key_A );
    file->insertItem( "&Open ...", this, SLOT(slotOpen()), ALT+Key_O );
    file->insertSeparator();
    file->insertItem( "Page La&yout ...", this, SLOT(slotPageLayout()), ALT+Key_Y );
    file->insertItem( "Print ...", this, SLOT(slotPrint()), ALT+Key_P );
    file->insertSeparator();
    file->insertItem( "&Close", this, SLOT(slotClose()), ALT+Key_C );
    file->insertItem( "&Quit",  this, SLOT(slotQuit()), ALT+Key_Q );

    KMenuBar* menubar = getShell()->getMenuBar();
    
    menubar->insertItem( "&File", file );
}

void FormulaEditor::showGUI()
{
    /****************************************************************
     *
     * In this function you must construct your menubars.
     * This function is going to be called several times.
     * Dont create the file menu here. Only if you are the top level
     * part you are allowed to display a file menu. Have a look at
     * @ref #showFileMenu
     *
     ****************************************************************/

    QPopupMenu *edit = new QPopupMenu;
    CHECK_PTR( edit );
    edit->insertItem( "&Do Something", this, SLOT( slotDummy() ) );

    QPopupMenu *help = new QPopupMenu;
    CHECK_PTR( help );
    connect (help, SIGNAL (activated (int)), SLOT (slotHelp (int)));
    help->insertItem( "&About ...",ID_HELP_ABOUT);
    help->insertItem( "&Using kxcl ...",ID_HELP_USING);
    help->insertSeparator();
    help->insertItem( "What is &KDE ...",ID_HELP_KDE);     

    insertMenu = new QPopupMenu;
    CHECK_PTR( insertMenu );
    QList<KPartEntry>& list = getShell()->getAvailableParts();
    KPartEntry *e;
    for ( e = list.first(); e != 0L; e = list.next() )
	insertMenu->insertItem( e->getPartName() );
    connect( insertMenu, SIGNAL( activated( int ) ), this, SLOT( slotInsertPart( int ) ) );

    KMenuBar* menubar = getShell()->getMenuBar();
    
    menubar->insertItem( "&Edit", edit );
    menubar->insertItem( "&Insert", insertMenu );
    menubar->insertSeparator();
    menubar->insertItem( "&Help", help );            

    menubar->update();

    KPart::showGUI();

    // We should show our scrollbars & rulers now
    kformulaw->showGUI( TRUE );
}

void FormulaEditor::hideGUI()
{
    KPart::hideGUI();
    
    // Should we hide our scrollbars & rulers now
    if ( !isTopLevelPart() )
	kformulaw->showGUI( FALSE );
}

void FormulaEditor::slotInsertPart( int _id )
{
    QString p = insertMenu->text( _id );
    // Is this part really installed ?
    KPartEntry* e = getShell()->findPartByName( p );
    if ( e )
	insertNewPart( e );
}

void FormulaEditor::insertNewPart( KPartEntry* _entry )
{
    /********************************************************************
     *
     * Modify this function to your needs. It creates a new child kpart,
     * positions it and appends it to our list of children.
     * You may want to organize your children in a different way.
     *
     ********************************************************************/

    KPart *bp = getShell()->newPart( _entry->getLibName(), kformulaw->getPaperWidget() );

    if ( bp == 0L )
    {
	QMessageBox::message( "Error", "Could not load part" );
	return;
    }
    
    bp->setGeometry( 10, 10, 200, 200 );
    bp->show();
    childList.append( bp );
}

void FormulaEditor::slotQuit()
{
    if ( getShell()->closeShells() )
	exit(1);
}

void FormulaEditor::slotClose()
{
    if ( closePart() )
	delete getShell();
}

void FormulaEditor::slotOpen()
{
    /**********************************************************************
     *
     * Put some fileselector stuff in here.
     *
     **********************************************************************/
warning("Open..");
     fileURL="/tmp/kformula.test";
warning("........");
  //if (!fileURL.isEmpty) 
    load( "/tmp/kformula.test");
}

void FormulaEditor::slotSaveAs()
{
    /**********************************************************************
     *
     * Put some fileselector stuff in here.
     *
     **********************************************************************/
fileURL="/tmp/kformula.test"; //QFileDialog::getSaveFileName()
    save( fileURL );
}

void FormulaEditor::slotSave()
{
    fileURL="/tmp/kformula.test";
    if (!fileURL.isNull() ) 
    save( fileURL ); else  save( QFileDialog::getSaveFileName() );
}

static const char *MSG_ABOUT="Kformula Editor\n by Andrea Rizzi bilibao@ouverture.it\n 22/11/1997,v.0.1.1";

void FormulaEditor::slotHelp(int id)
{
    /**********************************************************************
     *
     * Write your help stuff here.
     *
     **********************************************************************/

    switch(id)
    {
    case ID_HELP_ABOUT:
	KMsgBox::message(0,"About FormulaEditor",MSG_ABOUT,KMsgBox::INFORMATION);
	break;
	
    case ID_HELP_USING:
	KApplication::getKApplication()->invokeHTMLHelp("FormulaEditor/index.html",0);
	break;
	
    case ID_HELP_KDE:
	KApplication::getKApplication()->invokeHTMLHelp("index.html",0);
	break;
    }
}

bool FormulaEditor::save( const char *_url )
{
    /****************************************************************
     *
     * No need to play around in this function. It is only important
     * for you that 'fileURL' is set here.
     *
     ****************************************************************/

KURL u( _url );
fileURL=u.path();
QFile f(fileURL);
int i;
FormulaBlock *bl;
if (f.open(IO_WriteOnly)) {
    warning("save 2");
    QDataStream st(&f);
    st << "KOFO";   // KOfficeFOrmula
    for(i=0;i<2000;i++) 
     if ( ! ((FormulaDisplay *)kformulaw->getPaperWidget())->isFree[i])
     {
      bl=((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[i]; 
    st << (Q_INT16) bl->getID();
    st << (Q_INT16) bl->gettype();   
    st << (Q_INT16) bl->getnext();   
    st << (Q_INT16) bl->getprev();   
    st << (Q_INT16) bl->getchild1();
    st << (Q_INT16) bl->getchild2();
    st << (Q_INT16) bl->getchild3();
    st << (Q_INT16) bl->getsp();
    st << (const char *) bl->getcont();
    st << (QFont) bl->fontc;    
             }
    st << (Q_INT16) -1;
    st << "OFOK";    // END of KOFO
    f.close();     
} else 	warning( "COULD NOT SAVE FormulaEditor\n");


/*
    KURL u( _url );
    if ( u.isMalformed() )
	return FALSE;
    
    KorbSession* korb = new KorbSession( u.path(), IO_WriteOnly );
    korb->setAuthor( "(c) KDE Project, kde@kde.org" );

    OBJECT o_obj = save( korb );

    if ( o_obj == 0 )
    {
	warning( "COULD NOT SAVE FormulaEditor\n");
	korb->release();
	delete korb;
	return FALSE;
    }
    
    korb->setRootObject( o_obj );
    korb->release();
    delete korb;
*/
    fileURL = _url;
    
    return TRUE;
}

OBJECT FormulaEditor::save( KorbSession* _korb )
{
    /****************************************************************
     *
     * If 'saveChildren' does not do a good job for you, you must
     * do your complete savings here.
     * Right now this function calls the two convenience functions
     *
     * save( QIODevice* )
     * saveChildren( ..... )
     *
     ****************************************************************/

    // For use as values in the ObjectType property
    TYPE t_obj   =  _korb->registerType( "KDE:FormulaEditor:Object" );
    TYPE t_data  =  _korb->registerType( "KDE:FormulaEditor:DataType" );

    PROPERTY p_data = _korb->registerProperty( "KDE:FormulaEditor:Data" );

    // This line is needed!
    OBJECT o_obj = KPart::save( _korb, t_obj );
    if ( !o_obj )
    {
	warning( "COULD NOT SAVE FormulaEditor->KPart\n");
	return 0;
    }

    // Save your children
    if ( !saveChildren( _korb, o_obj ) )
    {
	warning( "COULD NOT SAVE FormulaEditor->Children\n");
	return 0;
    }

    // Save your stuff. Of course you may use the whole bunch
    // of korb functions here. But sometimes a simple QIODevice
    // is all you need...
    VALUE value = _korb->newValue( o_obj, p_data, t_data );
    KorbDevice *device = 0L;
    device = _korb->getDeviceForValue( value, device );
    // The real saving is done here.
    bool result = save( device );
    _korb->release( device );
    if ( !result )
    {
	warning( "COULD NOT SAVE FormulaEditor->Custom\n");
	return 0;
    }
    
    return o_obj;
}

bool FormulaEditor::saveChildren( KorbSession* korb, OBJECT _o_obj )
{
    /****************************************************************************
     *
     * Modify this function if this is not the way you want to save your children.
     * You may erase this function completely and store your children any way
     * you like. Somehow you must call your childrens save function and you
     * must store the return value of the save function somewhere, otherwise
     * you dont find your child upon loading :-)
     *
     *****************************************************************************/

    TYPE t_pList  =  korb->registerType( "KDE:FormulaEditor:KPartList" );
    PROPERTY p_parts = korb->registerProperty( "KDE:FormulaEditor:KParts" );

    // A list of all KParts ids
    QStack<OBJECT> plStack;
    QStack<KPart> kStack;
    plStack.setAutoDelete( TRUE );

    // Save all KParts.
    QListIterator<KPart> pl( childList );
    for ( ; pl.current(); ++pl ) 
    {
	OBJECT *o_pl = new OBJECT( pl.current()->save( korb ) );
	    
	if ( *o_pl == 0 )
	{
	    warning( "COULD NOT SAVE FormulaEditor->Children->KPart\n");
	    return FALSE;
	}
	
	plStack.push( o_pl );
	kStack.push( pl.current() );
    } 

    /**
     * Write the list of KParts.
     * Since we store a whole bunch of them, we open a stream to do
     * the job, but you may do it different of course.
     */
    QDataStream stream;
    VALUE value = korb->newValue( _o_obj, p_parts, t_pList );
    KorbDevice *device = 0L;
    device = korb->getDeviceForValue( value, device );
    stream.setDevice( device );
    stream << (UINT32)plStack.count();
    while ( !plStack.isEmpty() )
    {
	/**
	 * Here we store the object reference ( return value of the save function )
	 * and the position of our children.
	 */
	stream << *( plStack.pop() );
	/**
	 * The child KParts could be scrolled, so we have to correct this
	 * in order of saving their correct position.
	 */
	stream << ( kStack.top()->x() + kformulaw->getXOffset() );
	stream << ( kStack.pop()->y() + kformulaw->getYOffset() );
    }
    stream.unsetDevice();
    korb->release( device );
    delete device;

    return TRUE;
}

bool FormulaEditor::save( QIODevice * )
{

    /***************************************************************************************
     *
     * Save your stuff here. If a function like 'saveChildren' does not help you, then
     * you must not use this function, since it does not allow you to save your children.
     * You need a handle to some KorbSession in order of saving other KParts. This is only
     * a suggestion for a convenience function.
     *
     ***************************************************************************************/

    return TRUE;
}

bool FormulaEditor::load( const char *_url )
{
    /****************************************************************
     *
     * No need to  play around in this function. It is only important
     * for you that 'fileURL' is set here.
     *
     ****************************************************************/

KURL u( _url );
fileURL=u.path();
warning("File:");
warning(fileURL);
QFile f( fileURL);
int i;
FormulaBlock *bl;
Q_INT16 a,t,n,p,c1,c2,c3,sz;
QFont fo;
char *str;
QString s,out;
bl=((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[0];
a=bl->getnext();
if (a!=-1) {
bl=((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[a];
((FormulaDisplay *)kformulaw->getPaperWidget())->deleteIt(bl); 
}
if (f.open(IO_ReadOnly) ) { 
    QDataStream st(&f);
    st >> str;   // KOfficeFOrmula
    i=0;
    do {
    st >> a ;
    if(a!= -1) {
    st >> t >> n >> p >> c1 >> c2 >> c3 >> sz >> str >> fo;
    if(a==0) {
	    bl=((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[0];
	    bl->settype(t);
	    bl->setnext(n);
	    bl->setprev(p);	    
	    bl->setchild1(c1);
	    bl->setchild2(c2);
	    bl->setchild3(c3);
	    bl->setcont(str);    
	    bl->setsp(sz);
	    bl->fontc=fo;    
	    } else {
	    if(t==4) c3 = sz;
    ((FormulaDisplay *)kformulaw->getPaperWidget())->addBlock(t,a,n,p,str,c1,c2,c3);       
     bl=((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[a];
     bl->fontc=fo;    
    //	    out.sprintf("%s  ID:%i Type:%i Next-prev:%i-%1 Children:%i %i %i sp:%i",str,a,t,n,p,c1,c2,c3,sz);
//	    warning(out);
	    }
        }
      } while(a!=-1);
    f.close();     
((FormulaDisplay *)kformulaw->getPaperWidget())->repaint();
}
/*    KURL u( _url );
    if ( u.isMalformed() )
    {
	warning( "URL is malformed\n");
	return FALSE;
    }
    
    KorbSession* korb = new KorbSession( u.path(), IO_ReadOnly );    

    QString author = korb->getAuthor();
    author.detach();

    printf("The Author is: %s\n",author.data());

    OBJECT o_root = korb->getRootObject();
    if ( o_root == 0 )
    {
	warning( "COULD NOT LOAD: no root object\n");
	return FALSE;
    }
    
    bool ret = load( korb, o_root );
    
    if ( !ret )
    {
	warning( "COULD NOT LOAD: Kformula\n");
	korb->release();
	delete korb;
	return FALSE;
    }

    korb->release();
    delete korb;
*/
    fileURL = _url;
    
    return TRUE;
}

bool FormulaEditor::load( KorbSession *korb, OBJECT _part )
{
    /*************************************************************
     *
     * Have a look at the corresponding save function.
     *
     *************************************************************/

    TYPE t_obj  =  korb->findType( "KDE:FormulaEditor:Object" );
    if ( !t_obj || !KPart::load( korb, _part, t_obj ) )
    {
	warning( "COULD NOT LOAD: no FormulaEditor found or KPart error\n");
	return FALSE;
    }
    
    PROPERTY p_data = korb->findProperty( "KDE:FormulaEditor:Data" );
    TYPE t_data = korb->findType( "KDE:FormulaEditor:DataType" );
    if ( !p_data || !t_data )
    {
	warning( "COULD NOT LOAD: Kformula->Properties not found\n");
	return FALSE;
    }
    
    VALUE value = korb->findValue( _part, p_data, t_data );
    if ( !value )
    {
	warning( "COULD NOT LOAD: Kformula no value\n");
	return FALSE;
    }
    
    KorbDevice *dev = 0L;
    dev = korb->getDeviceForValue( value, dev );
    if ( !dev )
    {
	warning( "COULD NOT LOAD: Kformula no device for value\n");
	return FALSE;
    }
    
    bool result = load( dev );
    
    korb->release( dev );
    delete dev;

    if ( !result )
	return FALSE;
    
    if ( !loadChildren( korb, _part ) )
    {
	warning( "COULD NOT LOAD: Kformula children\n");
	return FALSE;
    }
    
    return TRUE;
}

bool FormulaEditor::loadChildren( KorbSession *korb, OBJECT _o_obj )
{
    /*************************************************************
     *
     * Have a look at the corresponding save function.
     *
     *************************************************************/

    TYPE t_pList  =  korb->findType( "KDE:FormulaEditor:KPartList" );

    PROPERTY p_parts = korb->findProperty( "KDE:FormulaEditor:KParts" );
    PROPERTY p_partClass = korb->findProperty( "KDE:kpart:Class" );

    if ( !t_pList || !p_parts || !p_partClass )
    {
	warning( "COULD NOT LOAD: Kformula->children->Properties\n");
	return FALSE;
    }
    
    // Load all KParts
    VALUE value = korb->findValue( _o_obj, p_parts, t_pList );
    if ( value == 0L )
    {
	warning( "COULD NOT LOAD: kformula->children->values\n");
	return FALSE;
    }
    
    QDataStream stream;
    KorbDevice *dev = 0L;
    dev = korb->getDeviceForValue( value, dev );
    stream.setDevice( dev );
    UINT32 p;
    stream >> p;

    // Iteration over all parts
    for ( UINT32 j = 0; j < p; j++ )
    {
	OBJECT o_p;
	UINT32 x,y;
	stream >> o_p;
	stream >> x;
	stream >> y;
	// Find about the kind of class
	QString clas = korb->readStringValue( o_p, p_partClass );
	if ( clas.isNull() )
	{
	    warning("ERROR: while loading KPart\nDont know class");
	    stream.unsetDevice();
	    korb->release( dev );
	    delete dev;
	    return FALSE;
	}

	// Try to create a new part
	KPart *bp = getShell()->newPart( clas.data(), kformulaw->getPaperWidget() );
	if ( !bp || !bp->load( korb, o_p ) )
	{
	    warning("ERROR: while loading KPart '%s'\n",clas.data());
	    stream.unsetDevice();
	    korb->release( dev );
	    delete dev;
	    return FALSE;
	}
	
	// Append the part to the list of our children and show it.
	bp->move( x, y );
	bp->show();
	childList.append( bp );
    }
    
    stream.unsetDevice();
    korb->release( dev );
    delete dev;

    return TRUE;
}

bool FormulaEditor::load( QIODevice* )
{
    /*************************************************************
     *
     * Have a look at the corresponding save function.
     *
     *************************************************************/

    return TRUE;
}

bool FormulaEditor::closePart()
{
    /*******************************************************************
     *
     * This function is called if the user wants to close this part
     * or its parent. You have the right for a "veto".
     *
     *******************************************************************/
    if ( hasDocumentChanged() )
    {
	return QMessageBox::query( "FormulaEditor Warning", "The document has been modified.\r\nDo you really want to close it?\n\rAll changes will be lost!", "Yes", "No" );
    }
    
    return TRUE;
}

bool FormulaEditor::isParentOf( KPart *_part )
{
    /************************************************************
     *
     * Check wether you are the parent of the KPart.
     *
     ************************************************************/

    KPart *p;
    for ( p = childList.first(); p != 0L; p = childList.next() )
	if ( p == _part || p->isParentOf( _part ) )
	    return TRUE;
    
    return FALSE;
}

void FormulaEditor::print( QPainter &_painter, bool )
{
    _painter.save();
    
    /****************************************************************
     *
     * Print your stuff here.
     *
     ****************************************************************/

    // We print all child parts now
    KPart *part = 0L;
    for ( part = childList.first(); part != 0L; part = childList.next() )
    {
	// Dont forget to correct the position of the parts, since they
	// may be scrolled.
	// We move (0,0) to match (0,0) of the kpart.
	_painter.translate( part->x() + kformulaw->getXOffset(),
			    part->y() + kformulaw->getYOffset() );
	part->print( _painter );
	_painter.translate( -( part->x() + kformulaw->getXOffset() ),
			    -( part->y() + kformulaw->getYOffset() ) );
    }

    _painter.restore();
}

void FormulaEditor::slotPrint()
{
    QPrinter prt;
    // Printer Dialog
    if ( prt.setup(0) )
    {
	QPainter p;
	p.begin( &prt );
	p.setPen(black);
	int old;
	p.setBackgroundColor(white);
        old=((FormulaDisplay *)kformulaw->getPaperWidget())->getCurrent();
	((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[old]->setactive(0);
	((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[0]->getDimensions();  
        ((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[0]->
		    PaintIt(&p,50,((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[0]->getLYU(),0);
	// Print this part and its children
	print( p );
	p.end();
	((FormulaDisplay *)kformulaw->getPaperWidget())->Blocks[old]->setactive(1);
    }
}

FormulaEditor::~FormulaEditor()
{
    shell->removeDocument( this );
}

#include "part.moc"






