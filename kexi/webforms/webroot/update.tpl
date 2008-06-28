{{>beforecontent}}
    <h3>Edit a row in table: {{TABLENAME}}</h3>
    <br/>

    <div style="margin-top: 20px; margin-bottom: 20px">
    	 {{#SUCCESS}}<h4 style="color: green;">{{MESSAGE}}</h4>{{/SUCCESS}}
    	 {{#ERROR}}<h4 style="color: red;">{{MESSAGE}}</h4>{{/ERROR}}
    </div>
    
    {{#FORM}}

        <form action="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{PKEY_VALUE}}" method="POST">
        <table border="0" width="100%">
            {{FORMDATA}}	
        </table>

	<div style="margin-top: 20px; margin-bottom: 20px">
	<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{FIRST}}"><img src="/toolbox/arrow-left-double.png" alt="First"/></a>&nbsp;
	{{#SHOW_PREV}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{PREV}}"><img src="/toolbox/arrow-left.png" alt="Previous"/></a>&nbsp;
	{{/SHOW_PREV}}
	{{#SHOW_NEXT}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{NEXT}}"><img src="/toolbox/arrow-right.png" alt="Next"/></a>&nbsp;
	{{/SHOW_NEXT}}
	<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{LAST}}"><img src="/toolbox/arrow-right-double.png" alt="Last"/></a>&nbsp;
	</div>

        <input type="hidden" name="dataSent" value="true"/>
        <input type="hidden" name="tableFields" value="{{TABLEFIELDS}}"/>
        <input style="width: 20%;" type="submit" value="Save"/>
        <input style="width: 20%;" type="reset"/>
        </form><br/>
    {{/FORM}}
{{>aftercontent}}
