{{>beforecontent}}
    <h3>Create a new row in table: {{TABLENAME}}</h3>
    <br/>
    {{#SUCCESS}}<h4 style="color: green;">{{MESSAGE}}</h4>{{/SUCCESS}}
    {{#ERROR}}<h4 style="color: red;">{{MESSAGE}}</h4>{{/ERROR}}
    
    {{#FORM}}
        <form action="/create/{{TABLENAME}}" method="POST">
        <table border="0">
            {{FORMDATA}}	
        </table>
        <input type="hidden" name="dataSent" value="true"/>
        <input type="hidden" name="tableFields" value="{{TABLEFIELDS}}"/>
        <input type="submit"/>
        <input type="reset"/>
        </form>
    {{/FORM}}
{{>aftercontent}}

