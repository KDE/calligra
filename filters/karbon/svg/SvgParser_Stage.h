#include "SvgParser.h"
//#include <Frame.h>

class SvgParser;
class KoResourceManager;
class Frame;

class SvgParser_Stage : public SvgParser
{
public:
  
  SvgParser_Stage(KoResourceManager *documentResourceManager);
    virtual ~SvgParser_Stage();

    void parseAppData(const KoXmlElement& e);
    void setAppDataTag();
    void createAppData();
    
private:
    
    QList<Frame*> m_frameList;
    QList<QString> m_attributes;
    
    Frame * m_frame;
    QString *NS; //Namespace
  
 // Frame * frame;

};
