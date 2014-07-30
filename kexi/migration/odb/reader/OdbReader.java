/*A java file that reads hsqldb data
*/
import java.sql.*;
import java.util.zip.*;
import java.io.*;
import org.hsqldb.jdbcDriver;
import java.util.*;
import java.nio.file.Path;
import java.nio.file.Files;
public class OdbReader
{
    Connection con ;
    private File f = null ;

    /*Construct an OdbReader object with path and file name
    *@param tempDir
    *@param f
    *@throws exception
    */
    public OdbReader(String path) throws Exception
    {
        Path tempDir = Files.createTempDirectory("odbDatabase");
        ZipFile file = new ZipFile(path);
        fileUnzip(file, tempDir);
        file.close();
        con = DriverManager.getConnection( "jdbc:hsqldb:file:"+ tempDir+f,"SA","" );
    }

    /*Method to open a zipfile from Odb
    *@param file
    *@param tempDir
    *@throws IOException
    */

    public void fileUnzip(ZipFile file , Path tempDir) throws IOException
    {
        ZipEntry zpEnt;
        Enumeration enVar;
        BufferedOutputStream opStream;
        InputStream ipStream;
        int length;
        List v = new ArrayList();
        enVar = file.entries();
        f = Files.createTempFile(tempDir, "ooTempDatabase", "tmp").toFile();
        f.deleteOnExit();
        while(enVar.hasMoreElements()){
            zpEnt = (ZipEntry)enVar.nextElement();
            //if the file name is database directory, extract that file to temp directory
            if (zpEnt.getName().startsWith("database/")){
                System.out.println("Extracting the file : "+zpEnt.getName());
                byte[] buffer = new byte[1024];
                //creating an inputStream
                ipStream = file.getInputStream(zpEnt);
                //creating an output stream to write entries
                opStream = new BufferedOutputStream(new FileOutputStream(tempDir +f.getName() +"." +zpEnt.getName().substring(9)));
                //add created files in the vector , to delete later.
                v.add(tempDir +f.getName() +"." +zpEnt.getName().substring(9));

                //read file in the buffer and then write it to the output file.
                while((length = ipStream.read(buffer)) >= 0) {
                    opStream.write(buffer, 0, length);
                    }
                opStream.close();
                ipStream.close();
                }            
            }
    }

    public String getTableNames() throws Exception
    {
        String result="";
    	String TABLE_NAME = "TABLE_NAME";
	    String TABLE_SCHEMA = "TABLE_SCHEM";
	    String[] TABLE_TYPES = {"TABLE"};
	    DatabaseMetaData dbmd = con.getMetaData();

	    ResultSet tables = dbmd.getTables(null, null, null, TABLE_TYPES);
	    while (tables.next())
        {
            String temp=tables.getString(TABLE_NAME);
	        result+=temp+",";
	    }
        return result;                                                   
    }
    public String getTableSchema(String tablename) throws Exception
    {
        System.out.println("table name: "+tablename);
        String result="";
        String TABLE_NAME = "TABLE_NAME";
        String TABLE_SCHEMA = "TABLE_SCHEM";
        String[] TABLE_TYPES = {"TABLE"};
        DatabaseMetaData dbmd = con.getMetaData();
        ResultSet tables = dbmd.getTables(null, null, null, TABLE_TYPES);
        while (tables.next()) {
            String temp=tables.getString(TABLE_NAME);
            if(temp.equals(tablename)){
                ResultSet columns=dbmd.getColumns(null,null,tablename,null);
                while(columns.next()){
                    String column=columns.getString("COLUMN_NAME");
                    String datatype=columns.getString("TYPE_NAME");
                    result+=column+","+datatype+",";
                }
            }
        }
        if(!result.isEmpty()) result=result.substring(0,result.length()-1);
        return result;
    }
 

}   
