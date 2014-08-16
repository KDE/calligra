import java.sql.*;
import java.util.zip.*;
import java.io.*;
import org.hsqldb.jdbcDriver;
import java.util.*;
import java.nio.file.Path;
import java.nio.file.Files;

public class OdbReader
{
    Connection con;
    private static File f=null;
    
    //get the connection 
    public OdbReader(Path tempDir , String f) throws Exception
    {
        con = DriverManager.getConnection("jdbc:hsqldb:file:"
        		                           +tempDir
        		                           +f,
        		                           "SA",
        		                           "");
    }
    
    
    //synchronized method to execute given query
    public synchronized void queryOperation(String query) throws SQLException
    {
        Statement stmt;
        ResultSet rsltSet;
        
        //creates statement
        stmt = con.createStatement();
        rsltSet = stmt.executeQuery(query);
        
        getData(rsltSet);
        stmt.close();
        	
    }
    

    public static void getData(ResultSet rsltSet)throws SQLException
    {
        ResultSetMetaData metaInfo = rsltSet.getMetaData();
        int maxColumn = metaInfo.getColumnCount();
        int counter;
        Object obj;
        
        for(;rsltSet.next();){
            for(counter = 0; counter < maxColumn ;counter++){
                obj = rsltSet.getObject(counter + 1);
                
                System.out.println(obj.toString() + " ");
                
            }
            
            System.out.println(" ");
        }
   
    	
    }
    

    public static void fileUnzip(ZipFile file, Path tempDir)throws IOException
    {
    	
        ZipEntry zpEnt;
        Enumeration enVar;
        BufferedOutputStream opStrm;
        InputStream ipStrm;
        int length;
        List v = new ArrayList();
        
        enVar = file.entries();
        f = Files.createTempFile(tempDir, "ooTempDatabase", "tmp").toFile();
        f.deleteOnExit();
        while(enVar.hasMoreElements()){
            zpEnt = (ZipEntry)enVar.nextElement();
            
            //if file name is database directory, extract to temp directory.
            if(zpEnt.getName().startsWith("database/")){
                System.out.println("Extracting the file : " + zpEnt.getName());
                byte[] buffer = new byte[1024];
                
                //creating an inputstream.
                ipStrm = file.getInputStream(zpEnt);
                
                //creating an output stream to write entries.
                opStrm = new BufferedOutputStream(new FileOutputStream(tempDir
                		                                           + f.getName()
                		                                           + "."
                		                                           + zpEnt.getName().substring(9)));
                //add created files in the vector, to delete lateron.
                v.add(tempDir + f.getName() +  "." + zpEnt.getName().substring(9));
                
            
                //read file in the buffer, then write to output file.
                while((length = ipStrm.read(buffer)) >= 0){
                    opStrm.write(buffer, 0, length);
                }
                opStrm.close();
                ipStrm.close();
            }
            
        }
    
    }

                           

    public static void main(String args[])
    {
        OdbReader ob;
        jdbcDriver j = new jdbcDriver();
        ZipFile file;
        Path tempDir;
        
        
        try{
            file = new ZipFile(args[0]);
            tempDir = Files.createTempDirectory("odbDatabase");
            fileUnzip(file, tempDir);
            file.close();
            ob = new OdbReader(tempDir, f.getName());
        }
        catch(Exception e1){
        	//couldnot start ob.
            e1.printStackTrace();
            return;
        }
        
        try{
            ob.queryOperation("select id from \"mytable\"");
            ob.queryOperation("select * from \"mytable\"");
        }
        catch(SQLException e2){
        	//couldnot start ob.
            e2.printStackTrace();
            return;
        }
    	
    }
    
  
}
