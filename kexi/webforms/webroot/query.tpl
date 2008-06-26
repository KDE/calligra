{{>beforecontent}}
    <h3>Viewing query results for: {{QUERYNAME}}</h3>
    <div style="margin-top: 20px; margin-bottom: 20px">
    <a href="/create/{{TABLENAME}}">Create new record</a>
    </div>
    <table border="0" width="100%">
    <caption>Database elements in table: {{TABLENAME}}</caption>
        {{QUERYDATA}}
    </table>
{{>aftercontent}}
