
#include "kivio_py_stencil.h"

#ifdef HAVE_PYTHON

#include "kivio_common.h"
#include "kivio_config.h"
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_screen_painter.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"

#include "kivio_py_stencil_spawner.h"

#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <kdebug.h>
#include <math.h>


KivioPyStencil::KivioPyStencil()
    : KivioStencil()
{
   m_pConnectorTargets = new QList<KivioConnectorTarget>;
   m_pConnectorTargets->setAutoDelete(true);

   static bool first_time = true;
   if ( first_time ) {
     Py_Initialize();  // initialize python only once
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


   vars = Py_BuildValue( "{s:d,s:d,s:d,s:d,s:d,s:d,s:{}}", "x", m_x, "y", m_y, "w", m_w, "h", m_h, "x2", x2, "y2", y2 , "style");


   resizeCode = "";

}

KivioPyStencil::~KivioPyStencil()
{
}

int KivioPyStencil::init( QString initCode )
{
  if ( !runPython( initCode ) )
    return 0;

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

    XmlWriteString( e, "title", m_pSpawner->info()->title() );
    XmlWriteString( e, "setName", m_pSpawner->set()->name() );


    QDomElement dE = doc.createElement("PyData");

    // The python variables
    PyObject* mainmod = PyImport_AddModule("__main__");
    PyObject* gdic = PyModule_GetDict(mainmod);
    PyObject *ldic = Py_BuildValue("{s:O,s:{}}", "ldic", vars ,"res");

    if ( !PyRun_String("import pickle\nres = pickle.dumps(ldic)", Py_file_input, gdic, ldic) )
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

    QList<KivioConnectorTarget> *pOriginalTargets = ((KivioPyStencilSpawner*)m_pSpawner)->targets();
    pTarget = m_pConnectorTargets->first();
    pOriginal = pOriginalTargets->first();

    PyObject *targets = PyDict_GetItemString(vars,"connector_targets");
    int size = PyList_Size( targets );
    int i=0;

    while( pTarget && pOriginal && i<size )
    {
        PyObject *target = PyList_GetItem( targets, i );
        float x = getDoubleFromDict( target,"x");
        float y = getDoubleFromDict( target,"y");

        pTarget->setPosition( x, y );

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

    PyObject *o_y2 = PyDict_GetItemString(o,"y2");
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

    // make deep copy of vars:
    PyObject* mainmod = PyImport_AddModule("__main__");
    PyObject* gdic = PyModule_GetDict(mainmod);
    PyObject *ldic = Py_BuildValue("{s:O,s:{}}", "ldic", vars ,"res");

    if ( !PyRun_String("import copy\nres = copy.deepcopy(ldic)", Py_file_input, gdic, ldic) )
        PyErr_Print();

    pNewStencil->vars = PyDict_GetItemString( ldic, "res");


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

   float scale = d->scale;

   PyObject *shapes = PyDict_Values( PyDict_GetItemString( vars, "shapes" ) );
   if ( !shapes )
         return;

   int size = PyList_Size( shapes );

   for ( int i=0; i<size; i++ ) {
       PyObject *shape = PyList_GetItem( shapes, i );
       if ( !PyDict_Check(shape) )
         continue;

       int fill = KivioFillStyle::kcsNone;

       // if style dosn't defined for shape, applay default for stencil
       setStyle( d->painter, PyDict_GetItemString( vars, "style" ) , fill );
       setStyle( d->painter, shape, fill );

       if ( isSelected() )
         setStyle( d->painter, PyDict_GetItemString( shape, "selected" ) , fill );

       if ( outlined )
          fill = KivioFillStyle::kcsNone;

       QString stype = getStringFromDict( shape, "type" );
       stype = stype.lower();

       double x = getDoubleFromDict(shape,"x")*scale;
       double y = getDoubleFromDict(shape,"y")*scale;
       double w = getDoubleFromDict(shape,"w")*scale;
       double h = getDoubleFromDict(shape,"h")*scale;
       double x2 = getDoubleFromDict(shape,"x2")*scale;
       double y2 = getDoubleFromDict(shape,"y2")*scale;

       // get points list
       QList<KivioPoint> points;
       points.setAutoDelete(true);
       PyObject *pyPoints = PyDict_GetItemString( shape, "points" );
       if ( pyPoints && PyList_Check(pyPoints) ) {
          int size = PyList_Size(pyPoints);
          for ( int i=0; i<size; i++ ) {
             PyObject *pyPoint = PyList_GetItem(pyPoints,i);
             double x = getDoubleFromDict(pyPoint,"x")*scale;
             double y = getDoubleFromDict(pyPoint,"y")*scale;
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
//           QFont f = pShapeData->textFont();
//           f.setPointSize( f.pointSize() * _scale );
//           painter->setFont( f );
//           painter->setTextColor( pShapeData->textColor() );


          int tf = vTextAlign() | hTextAlign();
          QString text = getStringFromDict(shape,"text");
          if ( !text.isEmpty() )
            d->painter->drawText( x, y, w, h, tf | Qt::WordBreak, text );
       }

       if ( stype == "arc" ) {
          double a1 = getDoubleFromDict(shape,"a1");
          double a2 = getDoubleFromDict(shape,"a2");
          d->painter->drawArc(x,y,w,h,a1,a2);
       }

       if ( stype == "roundrect" ) {
          double rx = getDoubleFromDict(shape,"rx")*scale;
          double ry = getDoubleFromDict(shape,"ry")*scale;
          if (fill)
            d->painter->fillRoundRect( x, y, w, h, rx, ry );
          else
            d->painter->drawRoundRect( x, y, w, h, rx, ry );
       }

       if ( stype == "linearray" ) {
          d->painter->drawLineArray(&points);
       }

       if ( stype == "ellipse" ) {
          if (fill)
            d->painter->fillEllipse( x, y, w, h );
          else
            d->painter->drawEllipse( x, y, w, h );
       }


   }
}



int KivioPyStencil::runPython(QString code)
{
    const char *ccode = code.latin1();

    //qDebug( "local8bit :%s", code.local8Bit() );

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


KivioCollisionType KivioPyStencil::checkForCollision( KivioPoint *pPoint, float )
{
    float px = pPoint->x();
    float py = pPoint->y();

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
    QPixmap *targetPic;
    KivioPainter *painter;
    float x, y;

    // We don't draw these if we are selected!!!
    if( isSelected() )
      return;

    // Obtain the graphic used for KivioConnectorTargets
    targetPic = KivioConfig::config()->connectorTargetPixmap();


    float _scale = pData->scale;
    painter = pData->painter;

    KivioConnectorTarget *pTarget;
    pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        x = pTarget->x() * _scale;
        y = pTarget->y() * _scale;

        painter->drawPixmap( x-3, y-3, *targetPic );

        pTarget = m_pConnectorTargets->next();
    }
}

/**
 * Attempts to connect a KivioConnectorPoint to this stencil.
 *
 * This function will attempt to locate a KivioConnectorTarget in this
 * stencil with-in a given threshhold.  If it finds it, it will connect
 * the point to it, and return the target of the connection.
 */
KivioConnectorTarget *KivioPyStencil::connectToTarget( KivioConnectorPoint *p, float threshHold )
{
    float px = p->x();
    float py = p->y();

    float tx, ty;

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

void KivioPyStencil::setStyle( KivioPainter *p, PyObject *s, int &fillStyle )
{
    if ( !s )
        return;

    if ( !PyDict_Check(s) )
        return;

    PyObject *color = PyDict_GetItemString(s,"color");
    if ( color ) {
        QColor c = readColor(color);
        if ( c.isValid() )
            p->setFGColor(c);
    }

    color = PyDict_GetItemString(s,"bgcolor");
    if ( color ) {
        QColor c = readColor(color);
        if ( c.isValid() )
            p->setBGColor(c);
    }

    color = PyDict_GetItemString(s,"textcolor");
    if ( color ) {
        QColor c = readColor(color);
        if ( c.isValid() )
            p->setTextColor(c);
    }

    PyObject *lineWidth = PyDict_GetItemString(s,"linewidth");
    if ( lineWidth ) {
        float lw = getDoubleFromDict(s,"linewidth");
        p->setLineWidth( lw );
    }


    PyObject *o_fillStyle = PyDict_GetItemString(s,"fillstyle");
    if ( o_fillStyle ) {
        QString sfill = getStringFromDict(s,"fillstyle");
        if ( sfill == "solid" )
            fillStyle = KivioFillStyle::kcsSolid;

        if ( sfill == "none" )
            fillStyle = KivioFillStyle::kcsNone;
    }

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
    qDebug( "py_debug : %s",  PyString_AsString(PyObject_Str(o)) );
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


float KivioPyStencil::lineWidth()
{
    PyObject *lw = PyDict_GetItemString( PyDict_GetItemString(vars,"style"), "linewidth" );
    if ( lw )
        if ( PyNumber_Check(lw) )
            return ( PyInt_AsLong( PyNumber_Int(lw)) );
    return 1.0;
}

void KivioPyStencil::setLineWidth( float w )
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



#endif // HAVE_PYTHON
