/*A java file that reads hsqldb data
*/
import java.sql.*;
import java.util.zip.*;
import java.io.*;
import org.hsqldb.jdbcDriver;
import java.util.*;
import java.nio.file.*;

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
        Class.forName("org.hsqldb.jdbcDriver");
        Path tempDir = Files.createTempDirectory("odbDatabase");
        System.out.println("tempDir:" + tempDir.toString());
        deleteDirOnExit(tempDir.toFile());
        ZipFile file = new ZipFile(path);
        fileUnzip(file, tempDir);
        file.close();
        String connString = "jdbc:hsqldb:file:"+ tempDir+f.getName()+";shutdown=true;ifexists=true";
        con = DriverManager.getConnection(connString, "SA", "");
        System.out.println("Connection is made");
        Statement stmt = con.createStatement();
        stmt.execute("SET DATABASE SQL SYNTAX ORA TRUE");
        System.out.println("first query is executed");
        stmt.close();
    }

    private static void deleteDirOnExit(File dir)
    {
        dir.deleteOnExit(); // deleteOnExit for the folder first - it will get deleted last
        File[] files = dir.listFiles();
        if (files != null)
        {
            for (File f: files)
            {
                if (f.isDirectory())
                {
                    deleteDirOnExit(f);
                }
                else
                {
                    f.deleteOnExit();
                }
            }
        }
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



    public void shutdown() throws SQLException {

        Statement st = con.createStatement();

        // db writes out to files and performs clean shuts down
        // otherwise there will be an unclean shutdown
        // when program ends
        st.execute("SHUTDOWN");
        st.close();
        con.close();    // if there are no other open connection
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

    public String getTableSize(String tablename) throws Exception
    {
        DatabaseMetaData dbmd = con.getMetaData();
        ResultSet columns = dbmd.getColumns(null, null, tablename, null);
        int i = 0;
        int j=0;
        while (columns.next())
          i++;
        Statement stmt=con.createStatement();
        ResultSet rs=stmt.executeQuery("select count(*) from \""+tablename+"\"");
        while(rs.next())
            j=rs.getInt(1);
        stmt.close();
        return ""+i+","+j;
    }

    public String getCellValue(int columnnum, int rownum, String tablename) throws Exception
    {
        Statement stmt=con.createStatement();
        ResultSet rs=stmt.executeQuery("select * from \""+tablename+"\";");
        String result="";
        int temp=0;
        while(rs.next())
        {
            temp++;
            if(temp==rownum)
            {
                result=rs.getString(columnnum);
                break;
            }
        }
        stmt.close();
        return result;
    }
 
//    public static void main(String[] args){
//        try{
//        OdbReader oo=new OdbReader("/home/dhruv/Downloads/databaseOne.odb");
//        System.out.println(oo.getTableNames());
//        } catch(Exception e)
//        {
//            e.printStackTrace();
//        }
//    }
}   
