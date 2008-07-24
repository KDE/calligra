{{>beforecontent}}
    <h3>Edit a row in table: {{TABLENAME}}</h3>
    <br/>

    
    
    <div style="margin-top: 20px; margin-bottom: 20px">
    	 {{#SUCCESS}}<h4 style="color: green;">{{MESSAGE}}</h4>{{/SUCCESS}}
    	 {{#ERROR}}<h4 style="color: red;">{{MESSAGE}}</h4>{{/ERROR}}
    </div>
    
    {{#FORM}}
        <form action="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{PKEY_VALUE}}" method="POST">
        <table border="0">
            {{FORMDATA}}	
        </table>

	<div style="margin-top: 20px; margin-bottom: 20px">
	    {{#FIRST_ENABLED}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{FIRST}}"><img src="/f/toolbox/arrow-left-double.png" alt="First"/></a>&nbsp;
	    {{/FIRST_ENABLED}}
	    {{#FIRST_DISABLED}}
	        <img src="/f/toolbox/arrow-left-double-gray.png" alt="First"/>&nbsp;
	    {{/FIRST_DISABLED}}
	    {{#PREV_ENABLED}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{PREV}}"><img src="/f/toolbox/arrow-left.png" alt="Previous"/></a>&nbsp;
	    {{/PREV_ENABLED}}
	    {{#PREV_DISABLED}}
		<img src="/f/toolbox/arrow-left-gray.png" alt="Previous"/>&nbsp;
	    {{/PREV_DISABLED}}
	    {{#NEXT_ENABLED}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{NEXT}}"><img src="/f/toolbox/arrow-right.png" alt="Next"/></a>&nbsp;
	    {{/NEXT_ENABLED}}
	    {{#NEXT_DISABLED}}
		<img src="/f/toolbox/arrow-right-gray.png" alt="Next"/>&nbsp;
	    {{/NEXT_DISABLED}}
	    {{#LAST_ENABLED}}
		<a href="/update/{{TABLENAME}}/{{PKEY_NAME}}/{{LAST}}"><img src="/f/toolbox/arrow-right-double.png" alt="Last"/></a>&nbsp;
	    {{/LAST_ENABLED}}
	    {{#LAST_DISABLED}}
		<img src="/f/toolbox/arrow-right-double-gray.png" alt="Last"/>&nbsp;
	    {{/LAST_DISABLED}}
	</div>

        <input type="hidden" name="dataSent" value="true"/>
        <input type="hidden" name="tableFields" value="{{TABLEFIELDS}}"/>
        <input style="width: 20%;" type="submit" value="Save"/>
        <input style="width: 20%;" type="reset"/>
        </form><br/>
    {{/FORM}}
{{>aftercontent}}
