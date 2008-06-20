{{>beforecontent}}
    <h3>Viewing table {{TABLENAME}}</h3>
    <br/><a href="/">Table list</a><br/>
    <a href="/create/{{TABLENAME}}">Create new record</a>
    <table border="0" width="100%">
        {{TABLEDATA}}
    </table>
{{>aftercontent}}
