{{>beforecontent}}
    <h3>Viewing table: {{TABLENAME}}</h3>
    <div style="margin-top: 20px; margin-bottom: 20px">
    <a href="/create/{{TABLENAME}}">Create new record</a>
    </div>
    <table border="0" width="100%">
    <caption>Database elements in table: {{TABLENAME}}</caption>
        {{TABLEDATA}}
    </table>
{{>aftercontent}}
