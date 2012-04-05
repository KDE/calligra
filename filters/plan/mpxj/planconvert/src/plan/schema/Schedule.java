//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2012.01.19 at 12:56:12 PM CET 
//


package plan.schema;

import java.util.ArrayList;
import java.util.List;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlElements;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;
import javax.xml.bind.annotation.adapters.NormalizedStringAdapter;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;


/**
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "", propOrder = {
    "criticalpathListOrAppointment"
})
@XmlRootElement(name = "schedule")
public class Schedule {

    @XmlAttribute(required = true)
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String id;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String name;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String type;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String start;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String end;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String duration;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String earlystart;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String earlyfinish;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String latestart;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String latefinish;
    @XmlAttribute(name = "positive-float")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String positiveFloat;
    @XmlAttribute(name = "free-float")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String freeFloat;
    @XmlAttribute(name = "negative-float")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String negativeFloat;
    @XmlAttribute(name = "in-critical-path")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String inCriticalPath;
    @XmlAttribute(name = "not-scheduled")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String notScheduled;
    @XmlAttribute(name = "scheduling-conflict")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String schedulingConflict;
    @XmlAttribute(name = "scheduling-error")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String schedulingError;
    @XmlAttribute(name = "resource-error")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String resourceError;
    @XmlAttribute(name = "resource-overbooked")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String resourceOverbooked;
    @XmlAttribute(name = "resource-not-available")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String resourceNotAvailable;
    @XmlElements({
        @XmlElement(name = "criticalpath-list", type = CriticalpathList.class),
        @XmlElement(name = "appointment", type = Appointment.class)
    })
    protected List<Object> criticalpathListOrAppointment;

    /**
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getId() {
        return id;
    }

    /**
     * Sets the value of the id property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setId(String value) {
        this.id = value;
    }

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getName() {
        return name;
    }

    /**
     * Sets the value of the name property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setName(String value) {
        this.name = value;
    }

    /**
     * Gets the value of the type property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getType() {
        if (type == null) {
            return "Expected";
        } else {
            return type;
        }
    }

    /**
     * Sets the value of the type property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setType(String value) {
        this.type = value;
    }

    /**
     * Gets the value of the start property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getStart() {
        return start;
    }

    /**
     * Sets the value of the start property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setStart(String value) {
        this.start = value;
    }

    /**
     * Gets the value of the end property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEnd() {
        return end;
    }

    /**
     * Sets the value of the end property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEnd(String value) {
        this.end = value;
    }

    /**
     * Gets the value of the duration property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getDuration() {
        return duration;
    }

    /**
     * Sets the value of the duration property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDuration(String value) {
        this.duration = value;
    }

    /**
     * Gets the value of the earlystart property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEarlystart() {
        return earlystart;
    }

    /**
     * Sets the value of the earlystart property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEarlystart(String value) {
        this.earlystart = value;
    }

    /**
     * Gets the value of the earlyfinish property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEarlyfinish() {
        return earlyfinish;
    }

    /**
     * Sets the value of the earlyfinish property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEarlyfinish(String value) {
        this.earlyfinish = value;
    }

    /**
     * Gets the value of the latestart property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getLatestart() {
        return latestart;
    }

    /**
     * Sets the value of the latestart property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setLatestart(String value) {
        this.latestart = value;
    }

    /**
     * Gets the value of the latefinish property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getLatefinish() {
        return latefinish;
    }

    /**
     * Sets the value of the latefinish property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setLatefinish(String value) {
        this.latefinish = value;
    }

    /**
     * Gets the value of the positiveFloat property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPositiveFloat() {
        return positiveFloat;
    }

    /**
     * Sets the value of the positiveFloat property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPositiveFloat(String value) {
        this.positiveFloat = value;
    }

    /**
     * Gets the value of the freeFloat property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getFreeFloat() {
        return freeFloat;
    }

    /**
     * Sets the value of the freeFloat property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setFreeFloat(String value) {
        this.freeFloat = value;
    }

    /**
     * Gets the value of the negativeFloat property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getNegativeFloat() {
        return negativeFloat;
    }

    /**
     * Sets the value of the negativeFloat property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setNegativeFloat(String value) {
        this.negativeFloat = value;
    }

    /**
     * Gets the value of the inCriticalPath property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getInCriticalPath() {
        if (inCriticalPath == null) {
            return "0";
        } else {
            return inCriticalPath;
        }
    }

    /**
     * Sets the value of the inCriticalPath property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setInCriticalPath(String value) {
        this.inCriticalPath = value;
    }

    /**
     * Gets the value of the notScheduled property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getNotScheduled() {
        if (notScheduled == null) {
            return "0";
        } else {
            return notScheduled;
        }
    }

    /**
     * Sets the value of the notScheduled property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setNotScheduled(String value) {
        this.notScheduled = value;
    }

    /**
     * Gets the value of the schedulingConflict property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getSchedulingConflict() {
        if (schedulingConflict == null) {
            return "0";
        } else {
            return schedulingConflict;
        }
    }

    /**
     * Sets the value of the schedulingConflict property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSchedulingConflict(String value) {
        this.schedulingConflict = value;
    }

    /**
     * Gets the value of the schedulingError property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getSchedulingError() {
        if (schedulingError == null) {
            return "0";
        } else {
            return schedulingError;
        }
    }

    /**
     * Sets the value of the schedulingError property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSchedulingError(String value) {
        this.schedulingError = value;
    }

    /**
     * Gets the value of the resourceError property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getResourceError() {
        if (resourceError == null) {
            return "0";
        } else {
            return resourceError;
        }
    }

    /**
     * Sets the value of the resourceError property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setResourceError(String value) {
        this.resourceError = value;
    }

    /**
     * Gets the value of the resourceOverbooked property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getResourceOverbooked() {
        if (resourceOverbooked == null) {
            return "0";
        } else {
            return resourceOverbooked;
        }
    }

    /**
     * Sets the value of the resourceOverbooked property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setResourceOverbooked(String value) {
        this.resourceOverbooked = value;
    }

    /**
     * Gets the value of the resourceNotAvailable property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getResourceNotAvailable() {
        if (resourceNotAvailable == null) {
            return "0";
        } else {
            return resourceNotAvailable;
        }
    }

    /**
     * Sets the value of the resourceNotAvailable property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setResourceNotAvailable(String value) {
        this.resourceNotAvailable = value;
    }

    /**
     * Gets the value of the criticalpathListOrAppointment property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the criticalpathListOrAppointment property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getCriticalpathListOrAppointment().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link CriticalpathList }
     * {@link Appointment }
     * 
     * 
     */
    public List<Object> getCriticalpathListOrAppointment() {
        if (criticalpathListOrAppointment == null) {
            criticalpathListOrAppointment = new ArrayList<Object>();
        }
        return this.criticalpathListOrAppointment;
    }

}
