#include <KPresenterViewIface.h>

bool KPresenterViewIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if ( fun == "printDlg()" )
	{
		replyType = "void";
		printDlg( );
		return TRUE;
	}
	if ( fun == "editUndo()" )
	{
		replyType = "void";
		editUndo( );
		return TRUE;
	}
	if ( fun == "editRedo()" )
	{
		replyType = "void";
		editRedo( );
		return TRUE;
	}
	if ( fun == "editCut()" )
	{
		replyType = "void";
		editCut( );
		return TRUE;
	}
	if ( fun == "editCopy()" )
	{
		replyType = "void";
		editCopy( );
		return TRUE;
	}
	if ( fun == "editPaste()" )
	{
		replyType = "void";
		editPaste( );
		return TRUE;
	}
	if ( fun == "editDelete()" )
	{
		replyType = "void";
		editDelete( );
		return TRUE;
	}
	if ( fun == "editSelectAll()" )
	{
		replyType = "void";
		editSelectAll( );
		return TRUE;
	}
	if ( fun == "editCopyPage()" )
	{
		replyType = "void";
		editCopyPage( );
		return TRUE;
	}
	if ( fun == "editDelPage()" )
	{
		replyType = "void";
		editDelPage( );
		return TRUE;
	}
	if ( fun == "editFind()" )
	{
		replyType = "void";
		editFind( );
		return TRUE;
	}
	if ( fun == "editFindReplace()" )
	{
		replyType = "void";
		editFindReplace( );
		return TRUE;
	}
	if ( fun == "editHeaderFooter()" )
	{
		replyType = "void";
		editHeaderFooter( );
		return TRUE;
	}
	if ( fun == "newView()" )
	{
		replyType = "void";
		newView( );
		return TRUE;
	}
	if ( fun == "insertPage()" )
	{
		replyType = "void";
		insertPage( );
		return TRUE;
	}
	if ( fun == "insertPicture()" )
	{
		replyType = "void";
		insertPicture( );
		return TRUE;
	}
	if ( fun == "insertClipart()" )
	{
		replyType = "void";
		insertClipart( );
		return TRUE;
	}
	if ( fun == "toolsMouse()" )
	{
		replyType = "void";
		toolsMouse( );
		return TRUE;
	}
	if ( fun == "toolsLine()" )
	{
		replyType = "void";
		toolsLine( );
		return TRUE;
	}
	if ( fun == "toolsRectangle()" )
	{
		replyType = "void";
		toolsRectangle( );
		return TRUE;
	}
	if ( fun == "toolsCircleOrEllipse()" )
	{
		replyType = "void";
		toolsCircleOrEllipse( );
		return TRUE;
	}
	if ( fun == "toolsPie()" )
	{
		replyType = "void";
		toolsPie( );
		return TRUE;
	}
	if ( fun == "toolsText()" )
	{
		replyType = "void";
		toolsText( );
		return TRUE;
	}
	if ( fun == "toolsAutoform()" )
	{
		replyType = "void";
		toolsAutoform( );
		return TRUE;
	}
	if ( fun == "toolsDiagramm()" )
	{
		replyType = "void";
		toolsDiagramm( );
		return TRUE;
	}
	if ( fun == "toolsTable()" )
	{
		replyType = "void";
		toolsTable( );
		return TRUE;
	}
	if ( fun == "toolsFormula()" )
	{
		replyType = "void";
		toolsFormula( );
		return TRUE;
	}
	if ( fun == "toolsObject()" )
	{
		replyType = "void";
		toolsObject( );
		return TRUE;
	}
	if ( fun == "extraPenBrush()" )
	{
		replyType = "void";
		extraPenBrush( );
		return TRUE;
	}
	if ( fun == "extraConfigPie()" )
	{
		replyType = "void";
		extraConfigPie( );
		return TRUE;
	}
	if ( fun == "extraConfigRect()" )
	{
		replyType = "void";
		extraConfigRect( );
		return TRUE;
	}
	if ( fun == "extraRaise()" )
	{
		replyType = "void";
		extraRaise( );
		return TRUE;
	}
	if ( fun == "extraLower()" )
	{
		replyType = "void";
		extraLower( );
		return TRUE;
	}
	if ( fun == "extraRotate()" )
	{
		replyType = "void";
		extraRotate( );
		return TRUE;
	}
	if ( fun == "extraShadow()" )
	{
		replyType = "void";
		extraShadow( );
		return TRUE;
	}
	if ( fun == "extraBackground()" )
	{
		replyType = "void";
		extraBackground( );
		return TRUE;
	}
	if ( fun == "extraLayout()" )
	{
		replyType = "void";
		extraLayout( );
		return TRUE;
	}
	if ( fun == "extraOptions()" )
	{
		replyType = "void";
		extraOptions( );
		return TRUE;
	}
	if ( fun == "extraLineBegin()" )
	{
		replyType = "void";
		extraLineBegin( );
		return TRUE;
	}
	if ( fun == "extraLineEnd()" )
	{
		replyType = "void";
		extraLineEnd( );
		return TRUE;
	}
	if ( fun == "extraWebPres()" )
	{
		replyType = "void";
		extraWebPres( );
		return TRUE;
	}
	if ( fun == "extraAlignObjLeft()" )
	{
		replyType = "void";
		extraAlignObjLeft( );
		return TRUE;
	}
	if ( fun == "extraAlignObjCenterH()" )
	{
		replyType = "void";
		extraAlignObjCenterH( );
		return TRUE;
	}
	if ( fun == "extraAlignObjRight()" )
	{
		replyType = "void";
		extraAlignObjRight( );
		return TRUE;
	}
	if ( fun == "extraAlignObjTop()" )
	{
		replyType = "void";
		extraAlignObjTop( );
		return TRUE;
	}
	if ( fun == "extraAlignObjCenterV()" )
	{
		replyType = "void";
		extraAlignObjCenterV( );
		return TRUE;
	}
	if ( fun == "extraAlignObjBottom()" )
	{
		replyType = "void";
		extraAlignObjBottom( );
		return TRUE;
	}
	if ( fun == "extraAlignObjs()" )
	{
		replyType = "void";
		extraAlignObjs( );
		return TRUE;
	}
	if ( fun == "screenConfigPages()" )
	{
		replyType = "void";
		screenConfigPages( );
		return TRUE;
	}
	if ( fun == "screenPresStructView()" )
	{
		replyType = "void";
		screenPresStructView( );
		return TRUE;
	}
	if ( fun == "screenAssignEffect()" )
	{
		replyType = "void";
		screenAssignEffect( );
		return TRUE;
	}
	if ( fun == "screenStart()" )
	{
		replyType = "void";
		screenStart( );
		return TRUE;
	}
	if ( fun == "screenStop()" )
	{
		replyType = "void";
		screenStop( );
		return TRUE;
	}
	if ( fun == "screenPause()" )
	{
		replyType = "void";
		screenPause( );
		return TRUE;
	}
	if ( fun == "screenFirst()" )
	{
		replyType = "void";
		screenFirst( );
		return TRUE;
	}
	if ( fun == "screenPrev()" )
	{
		replyType = "void";
		screenPrev( );
		return TRUE;
	}
	if ( fun == "screenNext()" )
	{
		replyType = "void";
		screenNext( );
		return TRUE;
	}
	if ( fun == "screenLast()" )
	{
		replyType = "void";
		screenLast( );
		return TRUE;
	}
	if ( fun == "screenSkip()" )
	{
		replyType = "void";
		screenSkip( );
		return TRUE;
	}
	if ( fun == "screenFullScreen()" )
	{
		replyType = "void";
		screenFullScreen( );
		return TRUE;
	}
	if ( fun == "screenPenColor()" )
	{
		replyType = "void";
		screenPenColor( );
		return TRUE;
	}
	if ( fun == "screenPenWidth(QString)" )
	{
		QDataStream str( data, IO_ReadOnly );
		QString w;
		str >> w;
		replyType = "void";
		screenPenWidth(w );
		return TRUE;
	}
	if ( fun == "helpContents()" )
	{
		replyType = "void";
		helpContents( );
		return TRUE;
	}
	if ( fun == "sizeSelected()" )
	{
		replyType = "void";
		sizeSelected( );
		return TRUE;
	}
	if ( fun == "fontSelected()" )
	{
		replyType = "void";
		fontSelected( );
		return TRUE;
	}
	if ( fun == "textBold()" )
	{
		replyType = "void";
		textBold( );
		return TRUE;
	}
	if ( fun == "textItalic()" )
	{
		replyType = "void";
		textItalic( );
		return TRUE;
	}
	if ( fun == "textUnderline()" )
	{
		replyType = "void";
		textUnderline( );
		return TRUE;
	}
	if ( fun == "textColor()" )
	{
		replyType = "void";
		textColor( );
		return TRUE;
	}
	if ( fun == "textAlignLeft()" )
	{
		replyType = "void";
		textAlignLeft( );
		return TRUE;
	}
	if ( fun == "textAlignCenter()" )
	{
		replyType = "void";
		textAlignCenter( );
		return TRUE;
	}
	if ( fun == "textAlignRight()" )
	{
		replyType = "void";
		textAlignRight( );
		return TRUE;
	}
	if ( fun == "mtextFont()" )
	{
		replyType = "void";
		mtextFont( );
		return TRUE;
	}
	if ( fun == "textEnumList()" )
	{
		replyType = "void";
		textEnumList( );
		return TRUE;
	}
	if ( fun == "textUnsortList()" )
	{
		replyType = "void";
		textUnsortList( );
		return TRUE;
	}
	if ( fun == "textNormalText()" )
	{
		replyType = "void";
		textNormalText( );
		return TRUE;
	}
	if ( fun == "textDepthPlus()" )
	{
		replyType = "void";
		textDepthPlus( );
		return TRUE;
	}
	if ( fun == "textDepthMinus()" )
	{
		replyType = "void";
		textDepthMinus( );
		return TRUE;
	}
	if ( fun == "textSpacing()" )
	{
		replyType = "void";
		textSpacing( );
		return TRUE;
	}
	if ( fun == "textContentsToHeight()" )
	{
		replyType = "void";
		textContentsToHeight( );
		return TRUE;
	}
	if ( fun == "textObjectToContents()" )
	{
		replyType = "void";
		textObjectToContents( );
		return TRUE;
	}
	if ( DCOPObject::process( fun, data, replyType, replyData ) )
		return TRUE;
	return FALSE;
}

