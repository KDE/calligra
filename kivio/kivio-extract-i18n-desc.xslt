<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--
Copyright 2003 Nicolas GOUTTE <goutte@kde.org>
License GPL 2
-->
<!-- WARNING: if you modify this file, be careful about newlines
(or they end in the result if the XML rules do not tell to drop them.) -->
<xsl:output method="text" indent="no" media-type="text/plain" omit-xml-declaration="yes" encoding="UTF-8"/>
<!-- Match the whole document and then use a for-each to avoid to have one
 new line for each element of the source file. -->
<xsl:template match="/">
<xsl:for-each select="/KivioStencilSpawnerSetCollection">
<xsl:if test="boolean(Title)">i18n(&quot;TEST&quot;,&quot;<xsl:value-of select="Title/@data"/>&quot;) // Title (Collection)&#10;</xsl:if>
</xsl:for-each>
<xsl:for-each select="/KivioStencilSpawnerSet">
<xsl:if test="boolean(Title)">i18n(&quot;TEST&quot;,&quot;<xsl:value-of select="Title/@data"/>&quot;) // Title&#10;</xsl:if>
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
