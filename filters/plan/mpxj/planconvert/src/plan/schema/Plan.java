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
    "projectOrPlanOrSchedule"
})
@XmlRootElement(name = "plan")
public class Plan {

    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String author;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String email;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String editor;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String mime;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String version;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String id;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String name;
    @XmlAttribute(name = "scheduling-direction")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String schedulingDirection;
    @XmlAttribute(name = "recalculate-from")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String recalculateFrom;
    @XmlAttribute(name = "check-external-appointments")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String checkExternalAppointments;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String distribution;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String baselined;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String recalculate;
    @XmlAttribute
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String overbooking;
    @XmlAttribute(name = "scheduler-plugin-id")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String schedulerPluginId;
    @XmlElements({
        @XmlElement(name = "project", type = Project.class),
        @XmlElement(name = "plan", type = Plan.class),
        @XmlElement(name = "schedule", type = Schedule.class)
    })
    protected List<Object> projectOrPlanOrSchedule;

    /**
     * Gets the value of the author property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getAuthor() {
        return author;
    }

    /**
     * Sets the value of the author property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setAuthor(String value) {
        this.author = value;
    }

    /**
     * Gets the value of the email property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEmail() {
        return email;
    }

    /**
     * Sets the value of the email property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEmail(String value) {
        this.email = value;
    }

    /**
     * Gets the value of the editor property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getEditor() {
        return editor;
    }

    /**
     * Sets the value of the editor property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setEditor(String value) {
        this.editor = value;
    }

    /**
     * Gets the value of the mime property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getMime() {
        if (mime == null) {
            return "application/x-vnd.kde.plan";
        } else {
            return mime;
        }
    }

    /**
     * Sets the value of the mime property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setMime(String value) {
        this.mime = value;
    }

    /**
     * Gets the value of the version property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getVersion() {
        return version;
    }

    /**
     * Sets the value of the version property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setVersion(String value) {
        this.version = value;
    }

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
     * Gets the value of the schedulingDirection property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getSchedulingDirection() {
        if (schedulingDirection == null) {
            return "0";
        } else {
            return schedulingDirection;
        }
    }

    /**
     * Sets the value of the schedulingDirection property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSchedulingDirection(String value) {
        this.schedulingDirection = value;
    }

    /**
     * Gets the value of the recalculateFrom property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getRecalculateFrom() {
        return recalculateFrom;
    }

    /**
     * Sets the value of the recalculateFrom property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setRecalculateFrom(String value) {
        this.recalculateFrom = value;
    }

    /**
     * Gets the value of the checkExternalAppointments property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getCheckExternalAppointments() {
        if (checkExternalAppointments == null) {
            return "1";
        } else {
            return checkExternalAppointments;
        }
    }

    /**
     * Sets the value of the checkExternalAppointments property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setCheckExternalAppointments(String value) {
        this.checkExternalAppointments = value;
    }

    /**
     * Gets the value of the distribution property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getDistribution() {
        if (distribution == null) {
            return "0";
        } else {
            return distribution;
        }
    }

    /**
     * Sets the value of the distribution property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDistribution(String value) {
        this.distribution = value;
    }

    /**
     * Gets the value of the baselined property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getBaselined() {
        if (baselined == null) {
            return "0";
        } else {
            return baselined;
        }
    }

    /**
     * Sets the value of the baselined property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setBaselined(String value) {
        this.baselined = value;
    }

    /**
     * Gets the value of the recalculate property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getRecalculate() {
        if (recalculate == null) {
            return "0";
        } else {
            return recalculate;
        }
    }

    /**
     * Sets the value of the recalculate property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setRecalculate(String value) {
        this.recalculate = value;
    }

    /**
     * Gets the value of the overbooking property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getOverbooking() {
        if (overbooking == null) {
            return "0";
        } else {
            return overbooking;
        }
    }

    /**
     * Sets the value of the overbooking property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setOverbooking(String value) {
        this.overbooking = value;
    }

    /**
     * Gets the value of the schedulerPluginId property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getSchedulerPluginId() {
        return schedulerPluginId;
    }

    /**
     * Sets the value of the schedulerPluginId property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSchedulerPluginId(String value) {
        this.schedulerPluginId = value;
    }

    /**
     * Gets the value of the projectOrPlanOrSchedule property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the projectOrPlanOrSchedule property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getProjectOrPlanOrSchedule().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link Project }
     * {@link Plan }
     * {@link Schedule }
     * 
     * 
     */
    public List<Object> getProjectOrPlanOrSchedule() {
        if (projectOrPlanOrSchedule == null) {
            projectOrPlanOrSchedule = new ArrayList<Object>();
        }
        return this.projectOrPlanOrSchedule;
    }

}
