#include <KoXmlReader.h>
#include <plugins/pathshapes/rectangle/RectangleShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class PresentationViewPortShape : public RectangleShape
{
public:
    /**
     * @brief PresentationViewPortShape() - Constructor
     * Calls setDefaultValues() and
     * setRectangleStyle()
     */
   PresentationViewPortShape();
   PresentationViewPortShape(const KoXmlElement& e);
   /**
     * @brief ~PresentationViewPortShape() - Destructor
     */
   ~PresentationViewPortShape();
  
    /// reimplemented
    virtual QString pathShapeId() const;
  
    /**
     * @param stream QTextStream to which all frame data is written in specific format.
     */
    void saveSvg(QTextStream * stream);
    
    /**
     * @brief PresentationViewPortShape(const KoXmlElement& e) - Overloaded constructor
     * Parses frame information from a KoXmlElement,
     * And saves it into this frame.
     */
    void loadSvg(const KoXmlElement& e);
        
    /**
     * Sets the default values for all frame attrbutes
     */
    void setDefaultValues();
        
    void setRectangleStyle();
    /**
     * @brief generates a list of shapes contained within this ViewPort and
     * groups them into a KoShapeGroup.
     * All the ViewPort properties will be applied to this KoShapeGroup
     */
    void updateShapeGroup();
    
    void setTitle(const QString& title);
    void setRefId(const QString& refId); 
    void setTransitionProfile(const QString& transProfile);

    void setHide(bool condition);
    void setClip(bool condition);
    void enableTimeout(bool condition);

    void setSequence(int seq);
    void setZoomPercent(int zoomPercent);
    void setTimeout(int timeoutMs);
    void setTransitionDuration(int timeMs);
      
    QString title() const;
    QString refId() const; 
    QString transitionProfile() const;
      
    bool isHide() const;
    bool isClip() const;
    bool isEnableTimeout() const;
      
    int sequence() const;
    int zoomPercent() const;
    int timeout() const;
    int transitionDuration() const;
     
    /**
     * @return m_attributes 
     */
    QList<QString> attributes() const;
      
  private:
    
    QString m_title;
    QString m_refId;
    QString m_transitionProfile;
      
    bool m_hide;
    bool m_clip;
    bool m_timeoutEnable;

    int m_sequence;
    int m_transitionZoomPercent;
    int m_transitionDurationMs;
    int m_timeoutMs;
    
    /**
     * @brief m_attributes List of all attributes a frame can have
     */
    QList<QString> m_attributes;
  
};
