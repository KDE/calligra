#include "kformula_shell.h"

#include <qmsgbox.h>

KFormulaShell::KFormulaShell()
{
}

KFormulaShell::~KFormulaShell()
{
    cleanUp();
}

void KFormulaShell::cleanUp()
{
    if ( m_bIsClean )
	return;

    DefaultShell_impl::cleanUp();

    m_rDoc = 0L;
}

void KFormulaShell::setDocument( KFormulaDocument *_doc )
{
    m_rDoc = OPParts::Document::_duplicate( _doc );

    OPParts::View_var v = _doc->createView();  
    v->setPartShell( this );
    setRootPart( v );
}

void KFormulaShell::fileNew()
{
    m_rDoc = 0L;
  
    m_rDoc = OPParts::Document::_duplicate( new KFormulaDocument );
    if ( !m_rDoc->init() )
	{
	    QMessageBox::critical( this, i18n("KFormula Error"), i18n("Could not init"), i18n("Ok") );
	    return;
	}
  
    OPParts::View_var v = m_rDoc->createView();
    v->setPartShell( this );
    setRootPart( v );

    m_rMenuBar->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_rToolBarFile->setItemEnabled( m_idButtonFile_Print, true );
}

bool KFormulaShell::openDocument( const char *_filename )
{
    m_rDoc = 0L;
  
    m_rDoc = OPParts::Document::_duplicate( new KFormulaDocument );
    if ( !m_rDoc->open( _filename ) )
	return false;
  
    OPParts::View_var view = m_rDoc->createView();
    view->setPartShell( this );
    setRootPart( view );

    m_rMenuBar->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_rToolBarFile->setItemEnabled( m_idButtonFile_Print, true );

    return true;
}

bool KFormulaShell::saveDocument( const char *_filename, const char *_format )
{
    assert( !CORBA::is_nil( m_rDoc ) );

    if ( _format == 0L || *_format == 0 )
	_format = "kch";
  
    return m_rDoc->saveAs( _filename, _format );
}

#include "kformula_shell.moc"
