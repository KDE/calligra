
#include "kivio_py_stencil.h"
#include "kivio_view.h"

KivioPage *page;
KivioView *view;
#ifdef HAVE_PYTHON

#include "kivioglobal.h"
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_screen_painter.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"

#include "kivio_py_stencil_spawner.h"

#include "kivio_page.h"

#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <kdebug.h>
#include <math.h>
#include <KoZoomHandler.h>

#include "py_kivio.h"

extern "C" {
   void initkivioc(void);
}

KivioPyStencil::KivioPyStencil()
    : KivioStencil()
{
   m_pConnectorTargets = new QPtrList<KivioConnectorTarget>;
   m_pConnectorTargets->setAutoDelete(true);

   static bool first_time = true;
   if ( first_time ) {
     Py_Initialize();  // initialize python only once
	 //kdDebug(43000) << "init kivioc" << endl;
     initkivioc();
     first_time = false;
   }

   PyObject* mainmod = PyImport_AddModule("__main__");
   globals = PyModule_GetDict(mainmod);

   m_x = old_x = 0.0;
   m_y = old_y = 0.0;
   m_w = old_w = 72.0;
   m_h = old_h = 72.0;
   double x2 = m_x+m_w;
   double y2 = m_y+m_h;


   vars = Py_BuildValue( "{s:d,s:d,s:d,s:d,s:d,s:d,s:{},s:[],s:[],s:{}}",
                            "x", m_x, "y", m_y, "w", m_w, "h", m_h, "x2", x2, "y2", y2 , "style","connectors","connector_targets","shapes");

   resizeCode = "";
}


KivioPyStencil::~KivioPyStencil()
{
}


int KivioPyStencil::init( QString initCode )
{
  runPython(kivio_module);
  if ( !runPython( initCode ) )
    return 1;

  m_w = getDoubleFromDict( vars, "w");
  m_h = getDoubleFromDict( vars, "h");
  m_x = getDoubleFromDict( vars, "x");
  m_y = getDoubleFromDict( vars, "y");

  old_x = m_x;
  old_y = m_y;
  old_w = m_w;
  old_h = m_h;
  return 1;
}


bool KivioPyStencil::loadXML( const QDomElement &e )
{
    QDomNode node;
    QDomElement ele;


    node = e.firstChild();
    while( !node.isNull() )
    {
        QString nodeName = node.nodeName();

        ele = node.toElement();

        if( nodeName == "PyData" )
        {
            resizeCode = XmlReadString( ele, "resizeCode", "" );

            QString sVars = XmlReadString( ele, "vars", "" );

            PyObject* mainmod = PyImport_AddModule("__main__");
            PyObject* gdic = PyModule_GetDict(mainmod);
            PyObject *ldic = Py_BuildValue("{s:s,s:{}}", "ldic", sVars.latin1() ,"res");

            if ( !PyRun_String("import pickle\nres = pickle.loads(ldic)", Py_file_input, gdic, ldic) ) {
                PyErr_Print();
                return false;
            }

            vars = PyDict_GetItemString( ldic, "res" );
            Py_INCREF(vars);
            runPython(kivio_module);

            m_w = getDoubleFromDict( vars, "w");
            m_h = getDoubleFromDict( vars, "h");
            m_x = getDoubleFromDict( vars, "x");
            m_y = getDoubleFromDict( vars, "y");

            old_x = m_x;
            old_y = m_y;
            old_w = m_w;
            old_h = m_h;

        }
        else if( nodeName == "KivioConnectorTargetList" )
        {
            loadConnectorTargetListXML( ele );
        }

        node = node.nextSibling();
    }
    return true;
}


/**
 * Help function for loading from an XML node.
 */
void KivioPyStencil::loadConnectorTargetListXML( const QDomElement &e )
{
    QDomNode node;
    QDomElement ele;
    QString nodeName;
    KivioConnectorTarget *pTarget;

    pTarget = m_pConnectorTargets->first();
    node = e.firstChild();
    while( !node.isNull() && pTarget)
    {
        nodeName = node.nodeName();
        ele = node.toElement();

        if( nodeName == "KivioConnectorTarget" )
        {
            pTarget->loadXML( ele );
        }

        pTarget = m_pConnectorTargets->next();
        node = node.nextSibling();
    }
}


QDomElement KivioPyStencil::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioPyStencil");

    XmlWriteString( e, "id", m_pSpawner->info()->id() );
    XmlWriteString( e, "setId", m_pSpawner->set()->id() );


    QDomElement dE = doc.createElement("PyData");

    // The python variables
    PyObject* mainmod = PyImport_AddModule("__main__");
    PyObject* gdic = PyModule_GetDict(mainmod);
    PyObject *ldic = Py_BuildValue("{s:O,s:{}}", "ldic", vars ,"res");

    char *dump_code = "import copy\n"\
                      "import pickle\n"\
                      "cres = {}\n"\
                      "for key in ldic.keys():\n"\
                      "   try:\n"\
                      "      cres[key] = copy.deepcopy(ldic[key])\n"\
                      "   except:\n"\
                      "      ii=0\n"\
                      "res = pickle.dumps(cres)\n";

    if ( !PyRun_String(dump_code, Py_file_input, gdic, ldic) )
        PyErr_Print();

    QString sVars = PyString_AsString( PyDict_GetItemString( ldic, "res") );

    XmlWriteString( dE, "vars", sVars );
    XmlWriteString( dE, "resizeCode", resizeCode );

    e.appendChild( dE );


    // Save the target list
    QDomElement clE = doc.createElement("KivioConnectorTargetList");
    QDomElement targetE;
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        targetE = pTarget->saveXML( doc );
        clE.appendChild( targetE );

        pTarget = m_pConnectorTargets->next();
    }
    e.appendChild( clE );

    return e;

}

void KivioPyStencil::updateGeometry()
{

   //rescale items :

   rescaleShapes(vars);

   old_x = m_x;
   old_y = m_y;
   old_w = m_w;
   old_h = m_h;

   if ( !resizeCode.isEmpty() )
       runPython(resizeCode);

   // stuff from KivioSMLStensil

    KivioConnectorTarget *pTarget, *pOriginal;

    QPtrList<KivioConnectorTarget> *pOriginalTargets = ((KivioPyStencilSpawner*)m_pSpawner)->targets();

    pTarget = m_pConnectorTargets->first();
    pOriginal = pOriginalTargets->first();

    PyObject *targets = PyDict_GetItemString(vars,"connector_targets");
    int size = PyList_Size( targets );
    int i=0;

    while( pTarget && pOriginal && i<size )
    {
        PyObject *target = PyList_GetItem( targets, i );
        double x = getDoubleFromDict( target,"x");
        double y = getDoubleFromDict( target,"y");

        pTarget  ->setPosition( x, y );
        pOriginal->setPosition( x, y );

        pTarget = m_pConnectorTargets->next();
        pOriginal = pOriginalTargets->next();
        i++;
    }

}

void KivioPyStencil::rescaleShapes( PyObject *o )
{
  if ( PyDict_Check(o) ) {

    PyObject *o_x = PyDict_GetItemString(o,"x");
    if ( o_x ) {
        double x = getDoubleFromDict(o,"x");
        x = m_x+(x-old_x)*m_w/old_w;
        PyDict_SetItemString( o, "x", Py_BuildValue( "d", x ) );
    }

    PyObject *o_y = PyDict_GetItemString(o,"y");
    if ( o_y ) {
        double y = getDoubleFromDict(o,"y");
        y = m_y+(y-old_y)*m_h/old_h;
        PyDict_SetItemString( o, "y", Py_BuildValue( "d", y ) );
    }

    PyObject *o_x2 = PyDict_GetItemString(o,"x2");
    if ( o_x2 ) {
        double x = getDoubleFromDict(o,"x2");
        x = m_x+(x-old_x)*m_w/old_w;
        PyDict_SetItemString( o, "x2", Py_BuildValue( "d", x ) );
    }

    //PyObject *o_y2 = PyDict_GetItemString(o,"y2");
    if ( o_y ) {
        double y = getDoubleFromDict(o,"y2");
        y = m_y+(y-old_y)*m_h/old_h;
        PyDict_SetItemString( o, "y2", Py_BuildValue( "d", y ) );
    }

    PyObject *o_w = PyDict_GetItemString(o,"w");
    if ( o_w ) {
        double w = getDoubleFromDict(o,"w");
        w = w*m_w/old_w;
        PyDict_SetItemString( o, "w", Py_BuildValue( "d", w ) );
    }

    PyObject *o_h = PyDict_GetItemString(o,"h");
    if ( o_h ) {
        double h = getDoubleFromDict(o,"h");
        h = h*m_h/old_h;
        PyDict_SetItemString( o, "h", Py_BuildValue( "d", h ) );
    }

    PyObject *childs = PyDict_Values( o );
    int size = PyList_Size( childs );
    for ( int i=0; i<size; i++ )
        rescaleShapes(PyList_GetItem(childs,i) );

  } else
  if ( PyList_Check(o) ) {

    int size = PyList_Size( o );
    for ( int i=0; i<size; i++ )
        rescaleShapes( PyList_GetItem(o,i) );
  }
}

/**
 * Duplicates this object.
 *
 * Duplicates all aspects of this object except for the
 * stencil connected to the targets.
 */
KivioStencil *KivioPyStencil::duplicate()
{
    KivioPyStencil *pNewStencil = new KivioPyStencil();
    KivioStencil *pReturn;

    pNewStencil->m_x = m_x;
    pNewStencil->m_y = m_y;
    pNewStencil->m_w = m_w;
    pNewStencil->m_h = m_h;

    pNewStencil->old_x = old_x;
    pNewStencil->old_y = old_y;
    pNewStencil->old_w = old_w;
    pNewStencil->old_h = old_h;

    pNewStencil->m_pSpawner = m_pSpawner;
    pNewStencil->resizeCode = resizeCode;

    //make deep copy of vars:
    PyObject* mainmod = PyImport_AddModule("__main__");
    PyObject* gdic = PyModule_GetDict(mainmod);
    PyObject *ldic = Py_BuildValue("{s:O,s:{}}", "ldic", vars ,"res");

    char *copy_code = "import copy\n"\
                      "for key in ldic.keys():\n"\
                      "   try:\n"\
                      "      res[key] = copy.deepcopy(ldic[key])\n"\
                      "   except:\n"\
                      "      i=0\n";
    if ( !PyRun_String(copy_code, Py_file_input, gdic, ldic) )
        PyErr_Print();

    pNewStencil->vars = PyDict_GetItemString( ldic, "res");
    pNewStencil->runPython(kivio_module);

    // Copy the Connector Targets
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    while( pTarget )
    {
        pNewStencil->m_pConnectorTargets->append( pTarget->duplicate() );
        pTarget = m_pConnectorTargets->next();
    }

    *(pNewStencil->protection()) = *m_pProtection;
    *(pNewStencil->canProtect()) = *m_pCanProtect;

    pReturn = pNewStencil;
    return pReturn;
}



void KivioPyStencil::paint( KivioIntraStencilData *d )
{
   paint(d,false);
}

void KivioPyStencil::paintOutline( KivioIntraStencilData *d )
{
   paint(d,true);
}


void KivioPyStencil::paint( KivioIntraStencilData *d, bool outlined )
{
  KoZoomHandler* zoomHandler = d->zoomHandler;

  PyObject *shapes = PyDict_Values( PyDict_GetItemString( vars, "shapes" ) );

  if ( !shapes ) {
    return;
  }

  int size = PyList_Size( shapes );

  for ( int i=0; i<size; i++ ) {
    PyObject *shape = PyList_GetItem( shapes, i );
    if ( !PyDict_Check(shape) )
      continue;

    int fill = KivioFillStyle::kcsNone;

    // if style dosn't defined for shape, applay default for stencil
    setStyle( d, PyDict_GetItemString( vars, "style" ) , fill );
    setStyle( d, shape, fill );

    if ( isSelected() )
      setStyle( d, PyDict_GetItemString( shape, "selected" ) , fill );

    if ( outlined )
        fill = KivioFillStyle::kcsNone;

    QString stype = getStringFromDict( shape, "type" );
    stype = stype.lower();

    double x = zoomHandler->zoomItX(getDoubleFromDict(shape,"x"));
    double y = zoomHandler->zoomItY(getDoubleFromDict(shape,"y"));
    double w = zoomHandler->zoomItX(getDoubleFromDict(shape,"w"));
    double h = zoomHandler->zoomItY(getDoubleFromDict(shape,"h"));
    //double x2 = zoomHandler->zoomItX(getDoubleFromDict(shape,"x2"));
    //double y2 = zoomHandler->zoomItY(getDoubleFromDict(shape,"y2"));

    // get points list
    QPtrList<KivioPoint> points;
    points.setAutoDelete(true);
    PyObject *pyPoints = PyDict_GetItemString( shape, "points" );
    if ( pyPoints && PyList_Check(pyPoints) ) {
      int size = PyList_Size(pyPoints);
      for ( int i=0; i<size; i++ ) {
        PyObject *pyPoint = PyList_GetItem(pyPoints,i);
        double x = zoomHandler->zoomItX(getDoubleFromDict(pyPoint,"x"));
        double y = zoomHandler->zoomItY(getDoubleFromDict(pyPoint,"y"));
        points.append( new KivioPoint( x, y, KivioPoint::kptNormal ) );
      }
    }


    if ( stype == "rectangle" ) {
      if (fill)
        d->painter->fillRect( x, y, w, h );
      else
        d->painter->drawRect( x, y, w, h );
    }

    if ( stype == "textbox" ) {
      int tf = vTextAlign() | hTextAlign();

      QFont f = textFont();
      f.setPointSizeFloat(f.pointSizeFloat() * (((float)zoomHandler->zoom()) / 100.0));

      d->painter->setFont( f );
      QString text = getStringFromDict(shape,"text");

      if ( !text.isEmpty() ) {
        d->painter->drawText( int( x ), int( y ), int( w ), int( h ), tf | Qt::WordBreak, text );
      }
    }

    if ( stype == "arc" ) {
      double a1 = getDoubleFromDict(shape,"a1");
      double a2 = getDoubleFromDict(shape,"a2");
      d->painter->drawArc(x,y,w,h,a1,a2);
    }

    if ( stype == "roundrect" ) {
      double rx = zoomHandler->zoomItX(getDoubleFromDict(shape,"rx"));
      double ry = zoomHandler->zoomItY(getDoubleFromDict(shape,"ry"));

      if (fill) {
        d->painter->fillRoundRect( x, y, w, h, rx, ry );
      } else {
        d->painter->drawRoundRect( x, y, w, h, rx, ry );
      }
    }

    if ( stype == "linearray" ) {
      d->painter->drawLineArray(&points);
    }

    if ( stype == "ellipse" ) {
      if (fill) {
        d->painter->fillEllipse( x, y, w, h );
      } else {
        d->painter->drawEllipse( x, y, w, h );
      }
    }

    if(stype == "polygon") {
      d->painter->drawPolygon(&points);
    }

    if(stype == "polyline") {
      d->painter->drawPolyline(&points);
    }
  }

  KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

  while( pTarget )
  {
    pTarget->paintOutline( d );
    pTarget = m_pConnectorTargets->next();
  }
}



int KivioPyStencil::runPython(QString code)
{

    view = dynamic_cast<KivioView*>(KoDocument::documentList()->first()->views().getFirst());
    if ( view ) {
        page = view->activePage();
    }

    //const char *ccode = code.local8Bit().data();
	const char *ccode = code.latin1();

    //kdDebug(43000) << "code to run:" << endl << ccode << endl;

    PyObject *v = PyRun_String( const_cast<char*>(ccode) , Py_file_input, globals, vars );

    if (v == NULL) {
        PyErr_Print();
        return 0;
    }

    if (Py_FlushLine())
        PyErr_Clear();

    Py_DECREF(v);
    return 1;
}


double KivioPyStencil::getDoubleFromDict( PyObject *dict, const char *key )
{
    if (!PyDict_Check(dict)) return 0.0;

    PyObject *val = PyDict_GetItemString(dict,const_cast<char*>(key));
    if ( val ) {
        if ( PyFloat_Check(val) )
            return PyFloat_AsDouble( val );

        if ( PyInt_Check(val) )
            return PyInt_AsLong( val );

        if ( PyLong_Check(val) )
            return PyLong_AsDouble( val );
    }
    return 0.0;
}

QString KivioPyStencil::getStringFromDict( PyObject *dict, const char *key )
{
    PyObject *val = PyDict_GetItemString(dict,const_cast<char*>(key));
    if ( val && PyString_Check(val) )
        return QString( PyString_AsString(val) );

    return QString("");
}


KivioCollisionType KivioPyStencil::checkForCollision( KoPoint *pPoint, double )
{
    double px = pPoint->x();
    double py = pPoint->y();

    if( !(px < m_x + m_w &&
         px >= m_x &&
         py < m_y + m_h &&
         py >= m_y ) )
    {
        return kctNone;
    }

    return kctBody;

}


/**
 * Return a set of bits representing what resize handles are available.
 */
int KivioPyStencil::resizeHandlePositions()
{
   // Calculate the resize handle positions
   int mask = KIVIO_RESIZE_HANDLE_POSITION_ALL;

   if( m_pProtection->at( kpWidth ) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpE | krhpW);
   }

   if( m_pProtection->at( kpHeight) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpN | krhpS);
   }

   return mask;
}


/**
 * Paints the connector targets of this stencil.
 */
void KivioPyStencil::paintConnectorTargets( KivioIntraStencilData *pData )
{
  QPixmap targetPic;
  KivioPainter *painter;
  double x, y;

  // We don't draw these if we are selected!!!
  if( isSelected() )
    return;

  // Obtain the graphic used for KivioConnectorTargets
  targetPic = Kivio::connectorTargetPixmap();


  KoZoomHandler* zoomHandler = pData->zoomHandler;
  painter = pData->painter;

  KivioConnectorTarget *pTarget;
  pTarget = m_pConnectorTargets->first();
  while( pTarget )
  {
    x = zoomHandler->zoomItX(pTarget->x());
    y = zoomHandler->zoomItY(pTarget->y());

    painter->drawPixmap( x-3, y-3, targetPic );

    pTarget = m_pConnectorTargets->next();
  }
}

/**
 * Attempts to connect a KivioConnectorPoint to this stencil.
 *
 * This function will attempt to locate a KivioConnectorTarget in this
 * stencil with-in a given threshold.  If it finds it, it will connect
 * the point to it, and return the target of the connection.
 */
KivioConnectorTarget *KivioPyStencil::connectToTarget( KivioConnectorPoint *p, double threshHold )
{
    double px = p->x();
    double py = p->y();

    double tx, ty;

    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        tx = pTarget->x();
        ty = pTarget->y();


        if( px >= tx - threshHold &&
            px <= tx + threshHold &&
            py >= ty - threshHold &&
            py <= ty + threshHold )
        {
            // setTarget calls pTarget->addConnectorPoint() and removes
            // any previous connections from p
            p->setTarget( pTarget );
            return pTarget;
        }

        pTarget = m_pConnectorTargets->next();
    }

    return NULL;
}

KivioConnectorTarget *KivioPyStencil::connectToTarget( KivioConnectorPoint *p, int /*targetID*/ )
{
    int id = p->targetId();

    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        if( pTarget->id() == id )
        {
            p->setTarget(pTarget);

            return pTarget;
        }

        pTarget = m_pConnectorTargets->next();
    }

    return NULL;
}

int KivioPyStencil::generateIds( int nextAvailable )
{
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    // Iterate through all the targets
    while( pTarget )
    {
        // If this target has something connected to it
        if( pTarget->hasConnections() )
        {
            // Set it's id to the next available id
            pTarget->setId( nextAvailable );

            // Increment the next available id
            nextAvailable++;
        }
        else
        {
            // Otherwise mark it as unused (-1)
            pTarget->setId( -1 );
        }

        pTarget = m_pConnectorTargets->next();
    }

    // Return the next availabe id
    return nextAvailable;
}

void KivioPyStencil::setStyle( KivioIntraStencilData *d, PyObject *s, int &fillStyle )
{
  if ( !s )
    return;

  if ( !PyDict_Check(s) )
    return;

  KivioPainter *p = d->painter;
  KoZoomHandler* zoomHandler = d->zoomHandler;

  PyObject *color = PyDict_GetItemString(s,"color");

  if ( color ) {
    QColor c = readColor(color);

    if ( c.isValid() ) {
      p->setFGColor(c);
    }
  }

  color = PyDict_GetItemString(s,"bgcolor");

  if ( color ) {
    QColor c = readColor(color);

    if ( c.isValid() ) {
      p->setBGColor(c);
    }
  }

  color = PyDict_GetItemString(s,"textcolor");

  if ( color ) {
    QColor c = readColor(color);

    if ( c.isValid() ) {
      p->setTextColor(c);
    }
  }

  PyObject *lineWidth = PyDict_GetItemString(s,"linewidth");

  if ( lineWidth ) {
    double lw = getDoubleFromDict(s,"linewidth");
    p->setLineWidth( zoomHandler->zoomItY(lw) );
  }

  PyObject *o_fillStyle = PyDict_GetItemString(s,"fillstyle");

  if ( o_fillStyle ) {
    QString sfill = getStringFromDict(s,"fillstyle");

    if ( sfill == "solid" ) {
      fillStyle = KivioFillStyle::kcsSolid;
    }

    if ( sfill == "none" ) {
      fillStyle = KivioFillStyle::kcsNone;
    }
  }

  QString  sfont = getStringFromDict(s,"font");
  QFont f;
  int fontSize = (int)getDoubleFromDict(s,"fontsize");

  if(!fontSize) {
    fontSize = 12; // FIXME: Should use some kind of global setting!!!
  }

  f.setPointSize(fontSize);
  f.setPointSizeFloat(f.pointSizeFloat() * (((float)zoomHandler->zoom()) / 100.0));

  if ( !sfont.isEmpty() ) {
    f.setFamily(sfont);
  } else {
    f.setFamily("times"); // FIXME: Should use some kind of global setting!!!
  }

  p->setFont(f);
}

QColor KivioPyStencil::readColor( PyObject *color )
{
    if ( !color )
        return QColor();

    if ( PyString_Check(color) ) {
        return QColor( PyString_AsString(color) );
    }

    if ( PyList_Check(color) ) {
        if ( PyList_Size(color) == 3 ) {
            PyObject  *ro = PyList_GetItem(color,0);
            PyObject  *go = PyList_GetItem(color,1);
            PyObject  *bo = PyList_GetItem(color,2);

            int r=0, g=0, b=0;
            if ( PyNumber_Check(ro) )
                r = PyInt_AsLong( PyNumber_Int(ro));
            if ( PyNumber_Check(go) )
                g = PyInt_AsLong( PyNumber_Int(go));
            if ( PyNumber_Check(bo) )
                b = PyInt_AsLong( PyNumber_Int(bo));

            return QColor(r,g,b);

        }
    }

    return QColor();

}

void KivioPyStencil::PyDebug( PyObject * o )
{
    kdDebug(43000) << "py_debug: " <<  PyString_AsString(PyObject_Str(o)) << endl;
}


QColor KivioPyStencil::fgColor()
{
    QColor color = readColor( PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "color" ) );
    if ( color.isValid() )
        return color;
    else
        return QColor(0,0,0);
}

void KivioPyStencil::setFGColor( QColor c )
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "color"  , Py_BuildValue("[i,i,i]", c.red(), c.green(), c.blue() ) ) ;
}


QColor KivioPyStencil::bgColor()
{
    QColor color = readColor( PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "bgcolor" ) );
    if ( color.isValid() )
        return color;
    else
        return QColor(0,0,0);
}

void KivioPyStencil::setBGColor( QColor c )
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "bgcolor"  , Py_BuildValue("[i,i,i]", c.red(), c.green(), c.blue() ) ) ;
}


QColor KivioPyStencil::textColor()
{
    QColor color = readColor( PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "textcolor" ) );
    if ( color.isValid() )
        return color;
    else
        return QColor(0,0,0);
}

void KivioPyStencil::setTextColor( QColor c )
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "textcolor"  , Py_BuildValue("[i,i,i]", c.red(), c.green(), c.blue() ) ) ;
}


double KivioPyStencil::lineWidth()
{
    PyObject *lw = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "linewidth" );
    if ( lw )
        if ( PyNumber_Check(lw) )
            return ( PyInt_AsLong( PyNumber_Int(lw)) );
    return 1.0;
}

void KivioPyStencil::setLineWidth( double w )
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "linewidth"  , Py_BuildValue("f",w ) ) ;
}


void KivioPyStencil::setText( const QString &s )
{
   PyObject *to = PyDict_GetItemString( PyDict_GetItemString(vars,"shapes"), "text" );
   if ( to )
      PyDict_SetItemString(to, "text", Py_BuildValue("s", s.latin1() ));
}


QString KivioPyStencil::text()
{
    PyObject *to = PyDict_GetItemString( PyDict_GetItemString(vars,"shapes"), "text" );
    if ( to ) {
        return getStringFromDict(to, "text");
    }
    return QString("");
}

void KivioPyStencil::setTextFont( const QFont &f )
{
    double fs = f.pointSizeFloat();
    QString family = f.family();

    int bold     = f.bold();
    int italic   = f.italic();
    int underline= f.underline();

    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "fontsize"  , Py_BuildValue("f",fs ) ) ;
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "font"  , Py_BuildValue("s",family.latin1() ) ) ;
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "bold"  , Py_BuildValue("i",bold ) ) ;
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "italic"  , Py_BuildValue("i",italic ) ) ;
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "underline"  , Py_BuildValue("i",underline ) ) ;
}

QFont KivioPyStencil::textFont()
{
    PyObject *fn = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "font" );
    PyObject *fs = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "fontsize" );
    PyObject *bd = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "bold" );
    PyObject *it = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "italic" );
    PyObject *ul = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "underline" );

    QFont f;

    if ( fs )
        if ( PyNumber_Check(fs))
            f.setPointSize( PyInt_AsLong( PyNumber_Int(fs)));

    if ( bd )
        if ( PyNumber_Check(bd))
            f.setBold( PyInt_AsLong( PyNumber_Int(bd)));

    if ( it )
        if ( PyNumber_Check(it))
            f.setItalic( PyInt_AsLong( PyNumber_Int(it)));

    if ( ul )
        if ( PyNumber_Check(ul))
            f.setUnderline( PyInt_AsLong( PyNumber_Int(ul)));

    if ( fn )
        if ( PyString_Check(fn))
            f.setFamily( PyString_AsString(fn));

    return f;
}

int KivioPyStencil::hTextAlign()
{
    PyObject *hta = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "htextalign" );

    if ( hta )
        if ( PyNumber_Check(hta) )
            return ( PyInt_AsLong( PyNumber_Int(hta)));

    return Qt::AlignHCenter;
}

int KivioPyStencil::vTextAlign()
{
    PyObject *vta = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "vtextalign" );

    if ( vta )
        if ( PyNumber_Check(vta) )
            return ( PyInt_AsLong( PyNumber_Int(vta)));

    return Qt::AlignVCenter;
}

void KivioPyStencil::setHTextAlign(int hta)
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "htextalign"  , Py_BuildValue("i",hta));
}

void KivioPyStencil::setVTextAlign(int vta)
{
    PyDict_SetItemString(  PyDict_GetItemString(vars,"style") , "vtextalign"  , Py_BuildValue("i",vta));
}

#endif // HAVE_PYTHON

