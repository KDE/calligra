{{>beforecontent}}
    <h3>Deleting row {{PKEY}} in table {{TABLENAME}}</h3>
    <a href="/read/{{TABLENAME}}">Table contents</a>
    <br/><a href="/">Table list</a><br/>
    
    {{#SUCCESS}}<h4 style="color: green;">{{MESSAGE}}</h4>{{/SUCCESS}}
    {{#ERROR}}<h4 style="color: red;">{{MESSAGE}}</h4>{{/ERROR}}
{{>aftercontent}}