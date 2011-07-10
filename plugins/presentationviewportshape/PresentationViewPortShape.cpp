#include "PresentationViewPortShape.h"

void PresentationViewPortShape::loadSvg(const KoXmlElement& e)
{
  //static int s = 0;
  //TODO:Use namespace instead of writing 'calligra:' again and again
    if(e.hasAttribute("calligra:title")) {
      setTitle(e.attribute("calligra:title"));
      }

    if(e.hasAttribute("calligra:refid")) {
      setRefId(e.attribute("calligra:refid"));
      }

    if(e.hasAttribute("calligra:clip")) {
      setClip((e.attribute("calligra:clip") == "true"));
      }

    if(e.hasAttribute("calligra:hide")) {
      setHide((e.attribute("calligra:hide") == "true"));
      }

    if(e.hasAttribute("calligra:sequence")) {
      setSequence(e.attribute("calligra:sequence").toInt());
      //setSequence(++s);
      }
     
     if(e.hasAttribute("calligra:timeout-ms")) {
      setTimeout(e.attribute("calligra:timeout-ms").toInt());
      }
    
    if(e.hasAttribute("calligra:timeout-enable")) {
      enableTimeout(e.attribute("calligra:timeout-enable").toInt());
      }
    
    if(e.hasAttribute("calligra:transition-profile")) {
      setTransitionProfile(e.attribute("calligra:transition-profile"));
      }
    
    if(e.hasAttribute("calligra:transition-duration-ms")) {
      setTransitionDuration(e.attribute("calligra:transition-duration-ms").toInt());
      }
      
    if(e.hasAttribute("calligra:transition-zoom-percent")) {
      setZoomPercent(e.attribute("calligra:transition-zoom-percent").toInt());
      }
    
    PresentationViewPortShape();
}

PresentationViewPortShape::PresentationViewPortShape() 
{
    m_attributes.append("title");
    m_attributes.append("refid");
    m_attributes.append("transition-profile");
    m_attributes.append("hide");
    m_attributes.append("clip");
    m_attributes.append("timeout-enable");
    m_attributes.append("sequence");
    m_attributes.append("transition-zoom-percent");
    m_attributes.append("timeout-ms");
    m_attributes.append("transition-duration-ms");
        
    updateShapeGroup();
    setDefaultValues();
    }

PresentationViewPortShape::~PresentationViewPortShape() 
{
}
        
static void printIndentation(QTextStream *stream, unsigned int indent)
//void SvgWriter_generic::printIndentation(QTextStream* stream, unsigned int indent)
{
    static const QString INDENT("  ");
    for (unsigned int i = 0; i < indent;++i)
    *stream << INDENT;
}

QString PresentationViewPortShape::pathShapeId() const
{
  return PresentationViewPortShapeId;
}

void PresentationViewPortShape::setRectangleStyle()
{

}

void PresentationViewPortShape::updateShapeGroup()
{

}

void PresentationViewPortShape::saveSvg(QTextStream * stream)
{
    unsigned indent = 1;
    //QTextStream * stream;
    
    printIndentation(stream, indent++);
    *stream << "<calligra:frame" << endl;
       
    printIndentation(stream, indent);
    *stream << "calligra:" << "title=\"" << title() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "refid=\"" << refId() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-profile=\"" << transitionProfile() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "hide=\"";
    if(isHide())
      *stream << "true";
    else
     *stream << "false";
    *stream << "\"" << endl;
     
    printIndentation(stream, indent);
    *stream << "calligra:" << "clip=\"";
    if(isClip())
      *stream << "true";
    else
      *stream << "false";
    *stream << "\"" << endl;
    
    printIndentation(stream, indent);
    *stream << "calligra:" << "timeout-enable=\"" << isEnableTimeout() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "sequence=\"" << sequence() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-zoom-percent=\"" << zoomPercent() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "timeout-ms=\"" << timeout() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-duration-ms=\"" << transitionDuration() << "\"" << endl;
              
    *stream << "/>" << endl;
     
}
   
void PresentationViewPortShape::setDefaultValues()
{
    m_title = "No Title";
    m_refId = "No Ref ID"; 
    m_transitionProfile = "linear";
    m_hide = false;
    m_clip = true;
    m_timeoutEnable = false;

    m_sequence = 0;
    m_transitionZoomPercent = 100;
    m_transitionDurationMs = 1000; 
    m_timeoutMs = 5000;
}

void PresentationViewPortShape::setTitle(const QString& title) 
{  
    m_title = title;
}
        
void PresentationViewPortShape::setRefId(const QString& refId)
{
    m_refId = refId;
}
        
void PresentationViewPortShape::setSequence(int sequence) 
{
    m_sequence = sequence;
}
        
void PresentationViewPortShape::setZoomPercent(int zoomPercent) 
{
    m_transitionZoomPercent = zoomPercent;
}
        
void PresentationViewPortShape::setTransitionProfile(const QString& transProfile) 
{
    m_transitionProfile = transProfile;
}
        
void PresentationViewPortShape::setTransitionDuration(int timeMs) 
{
    m_transitionDurationMs = timeMs;
}
        
void PresentationViewPortShape::setTimeout(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
}

void PresentationViewPortShape::enableTimeout(bool condition)
{
    m_timeoutEnable = condition;
}

void PresentationViewPortShape::setClip(bool condition)
{
    m_clip = condition;
}

void PresentationViewPortShape::setHide(bool condition)
{
    m_hide = condition;
 }

QString PresentationViewPortShape::title() const
{
    return m_title;
}
        
QString PresentationViewPortShape::refId() const
{
    return m_refId;
}
       
QString PresentationViewPortShape::transitionProfile() const
{
    return m_transitionProfile;
}

bool PresentationViewPortShape::isHide() const
{
    return m_hide;
}

bool PresentationViewPortShape::isClip() const
{
    return m_clip;
}

bool PresentationViewPortShape::isEnableTimeout() const
{
    return m_timeoutEnable;
}

int PresentationViewPortShape::sequence() const
{
    return m_sequence;
}
        
int PresentationViewPortShape::zoomPercent() const
{
    return m_transitionZoomPercent;
}
        
int PresentationViewPortShape::timeout() const
{
    return m_timeoutMs;
}

int PresentationViewPortShape::transitionDuration() const 
{
    return m_transitionDurationMs;
}
          
QList< QString > PresentationViewPortShape::attributes() const
{
    return m_attributes;
}
