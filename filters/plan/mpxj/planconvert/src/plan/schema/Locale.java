//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2012.01.19 at 12:56:12 PM CET 
//


package plan.schema;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;
import javax.xml.bind.annotation.adapters.NormalizedStringAdapter;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;


/**
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "")
@XmlRootElement(name = "locale")
public class Locale {

    @XmlAttribute(name = "currency-symbol")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String currencySymbol;
    @XmlAttribute(name = "currency-digits")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String currencyDigits;
    @XmlAttribute(name = "positive-prefix-currency-symbol")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String positivePrefixCurrencySymbol;
    @XmlAttribute(name = "negative-prefix-currency-symbol")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String negativePrefixCurrencySymbol;
    @XmlAttribute(name = "positive-monetary-sign-position")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String positiveMonetarySignPosition;
    @XmlAttribute(name = "negative-monetary-sign-position")
    @XmlJavaTypeAdapter(NormalizedStringAdapter.class)
    protected String negativeMonetarySignPosition;

    /**
     * Gets the value of the currencySymbol property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getCurrencySymbol() {
        return currencySymbol;
    }

    /**
     * Sets the value of the currencySymbol property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setCurrencySymbol(String value) {
        this.currencySymbol = value;
    }

    /**
     * Gets the value of the currencyDigits property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getCurrencyDigits() {
        return currencyDigits;
    }

    /**
     * Sets the value of the currencyDigits property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setCurrencyDigits(String value) {
        this.currencyDigits = value;
    }

    /**
     * Gets the value of the positivePrefixCurrencySymbol property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPositivePrefixCurrencySymbol() {
        return positivePrefixCurrencySymbol;
    }

    /**
     * Sets the value of the positivePrefixCurrencySymbol property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPositivePrefixCurrencySymbol(String value) {
        this.positivePrefixCurrencySymbol = value;
    }

    /**
     * Gets the value of the negativePrefixCurrencySymbol property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getNegativePrefixCurrencySymbol() {
        return negativePrefixCurrencySymbol;
    }

    /**
     * Sets the value of the negativePrefixCurrencySymbol property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setNegativePrefixCurrencySymbol(String value) {
        this.negativePrefixCurrencySymbol = value;
    }

    /**
     * Gets the value of the positiveMonetarySignPosition property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPositiveMonetarySignPosition() {
        return positiveMonetarySignPosition;
    }

    /**
     * Sets the value of the positiveMonetarySignPosition property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPositiveMonetarySignPosition(String value) {
        this.positiveMonetarySignPosition = value;
    }

    /**
     * Gets the value of the negativeMonetarySignPosition property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getNegativeMonetarySignPosition() {
        return negativeMonetarySignPosition;
    }

    /**
     * Sets the value of the negativeMonetarySignPosition property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setNegativeMonetarySignPosition(String value) {
        this.negativeMonetarySignPosition = value;
    }

}
