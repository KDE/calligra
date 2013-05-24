package plan;

import java.util.Calendar;
import java.util.Calendar;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import plan.PlanWriter;

import net.sf.mpxj.Duration;
import net.sf.mpxj.ProjectCalendar;
import net.sf.mpxj.ProjectFile;
import net.sf.mpxj.ProjectHeader;
import net.sf.mpxj.Relation;
import net.sf.mpxj.RelationType;
import net.sf.mpxj.Resource;
import net.sf.mpxj.ResourceAssignment;
import net.sf.mpxj.Task;
import net.sf.mpxj.TimeUnit;
import net.sf.mpxj.reader.ProjectReader;
import net.sf.mpxj.reader.ProjectReaderUtility;
import net.sf.mpxj.mpx.MPXReader;
import net.sf.mpxj.mpx.MPXWriter;
import net.sf.mpxj.sample.MpxjConvert;

public class PlanConvert {

    static Task pre = null;

    public static void main(String[] args) {
        System.out.println("PlanConvert");
        try
        {
            System.out.println("PlanConvert args: " + args.length);
            if (args.length < 1 || args.length > 2)
            {
                System.out.println("Usage: PlanConvert <input file name> [<plan file name>]");
            }
            else
            {
                System.out.println("Reading input file started: " + args[0]);
                long start = System.currentTimeMillis();
                ProjectReader reader = ProjectReaderUtility.getProjectReader(args[0]);
                System.out.println("Got reader utility");
                ProjectFile projectFile = reader.read(args[0]);
                long elapsed = System.currentTimeMillis() - start;
                System.out.println("Reading input file completed in " + elapsed + "ms.");
                if (args.length == 1)
                {
                    xlistProject(projectFile);
                }
                else
                {
                    System.out.println("Writing Plan output file started: " +  args[1]);
                    PlanWriter w = new PlanWriter();
                    w.write(projectFile, args[1]);
                    System.out.println("Writing Plan output file completed.");
                }
            }
        }

        catch (Exception ex)
        {
            System.out.println("Exception");
            ex.printStackTrace(System.out);
        }
    }

    private static void xlistProject(ProjectFile file)
    {
/*        xlistProjectHeader(file);

        xlistResources(file);

        xlistTasks(file);

        xlistAssignments(file);

        xlistAssignmentsByTask(file);

        xlistAssignmentsByResource(file);

        xlistHierarchy(file);

        xlistTaskNotes(file);

        xlistResourceNotes(file);

        xlistRelationships(file);

        xlistSlack(file);

        xlistCalendars(file);*/
    }

   /**
    * Reads basic summary details from the project header.
    *
    * @param file MPX file
    */
//    private static void xlistProjectHeader(ProjectFile file)
//    {
//       SimpleDateFormat df = new SimpleDateFormat("dd/MM/yyyy HH:mm z");
//       ProjectHeader header = file.getProjectHeader();
//       Date startDate = header.getStartDate();
//       Date finishDate = header.getFinishDate();
//       String formattedStartDate = startDate == null ? "(none)" : df.format(startDate);
//       String formattedFinishDate = finishDate == null ? "(none)" : df.format(finishDate);
// 
//       System.out.println("Project Header: StartDate=" + formattedStartDate + " FinishDate=" + formattedFinishDate);
//       System.out.println();
//    }
// 
//    /**
//     * This method lists all resources defined in the file.
//     *
//     * @param file MPX file
//     */
//    private static void xlistResources(ProjectFile file)
//    {
//       for (Resource resource : file.getAllResources())
//       {
//          System.out.println("Resource: " + resource.getName() + " (Unique ID=" + resource.getUniqueID() + ") Start=" + resource.getStart() + " Finish=" + resource.getFinish());
//       }
//       System.out.println();
//    }
// 
//    /**
//     * This method lists all tasks defined in the file.
//     *
//     * @param file MPX file
//     */
//    private static void xlistTasks(ProjectFile file)
//    {
//       SimpleDateFormat df = new SimpleDateFormat("dd/MM/yyyy HH:mm z");
//       String startDate;
//       String finishDate;
//       String duration;
//       Date date;
//       Duration dur;
// 
//       for (Task task : file.getAllTasks())
//       {
//          date = task.getStart();
//          if (date != null)
//          {
//             startDate = df.format(date);
//          }
//          else
//          {
//             startDate = "(no date supplied)";
//          }
// 
//          date = task.getFinish();
//          if (date != null)
//          {
//             finishDate = df.format(date);
//          }
//          else
//          {
//             finishDate = "(no date supplied)";
//          }
// 
//          dur = task.getDuration();
//          if (dur != null)
//          {
//             duration = dur.toString();
//          }
//          else
//          {
//             duration = "(no duration supplied)";
//          }
// 
//          System.out.println("Task: " + task.getName() + " ID=" + task.getID() + " Unique ID=" + task.getUniqueID() + " (Start Date=" + startDate + " Finish Date=" + finishDate + " Duration=" + duration + " Outline Level=" + task.getOutlineLevel() + " Outline Number=" + task.getOutlineNumber() + " Recurring=" + task.getRecurring() + ")");
//       }
//       System.out.println();
//    }
// 
//    /**
//     * This method lists all tasks defined in the file in a hierarchical
//     * format, reflecting the parent-child relationships between them.
//     *
//     * @param file MPX file
//     */
//    private static void xlistHierarchy(ProjectFile file)
//    {
//       for (Task task : file.getChildTasks())
//       {
//          System.out.println("Task: " + task.getName());
//          listHierarchy(task, " ");
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * Helper method called recursively to list child tasks.
//     *
//     * @param task task whose children are to be displayed
//     * @param indent whitespace used to indent hierarchy levels
//     */
//    private static void xlistHierarchy(Task task, String indent)
//    {
//       for (Task child : task.getChildTasks())
//       {
//          System.out.println(indent + "Task: " + child.getName());
//          listHierarchy(child, indent + " ");
//       }
//    }
// 
//    /**
//     * This method lists all resource assignments defined in the file.
//     *
//     * @param file MPX file
//     */
//    private static void xlistAssignments(ProjectFile file)
//    {
//       Task task;
//       Resource resource;
//       String taskName;
//       String resourceName;
// 
//       for (ResourceAssignment assignment : file.getAllResourceAssignments())
//       {
//          task = assignment.getTask();
//          if (task == null)
//          {
//             taskName = "(null task)";
//          }
//          else
//          {
//             taskName = task.getName();
//          }
// 
//          resource = assignment.getResource();
//          if (resource == null)
//          {
//             resourceName = "(null resource)";
//          }
//          else
//          {
//             resourceName = resource.getName();
//          }
// 
//          System.out.println("Assignment: Task=" + taskName + " Resource=" + resourceName);
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * This method displays the resource assignments for each task. This time
//     * rather than just iterating through the list of all assignments in
//     * the file, we extract the assignments on a task-by-task basis.
//     *
//     * @param file MPX file
//     */
//    private static void xlistAssignmentsByTask(ProjectFile file)
//    {
//       for (Task task : file.getAllTasks())
//       {
//          System.out.println("Assignments for task " + task.getName() + ":");
// 
//          for (ResourceAssignment assignment : task.getResourceAssignments())
//          {
//             Resource resource = assignment.getResource();
//             String resourceName;
// 
//             if (resource == null)
//             {
//                resourceName = "(null resource)";
//             }
//             else
//             {
//                resourceName = resource.getName();
//             }
// 
//             System.out.println("   " + resourceName);
//          }
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * This method displays the resource assignments for each resource. This time
//     * rather than just iterating through the list of all assignments in
//     * the file, we extract the assignments on a resource-by-resource basis.
//     *
//     * @param file MPX file
//     */
//    private static void xlistAssignmentsByResource(ProjectFile file)
//    {
//       for (Resource resource : file.getAllResources())
//       {
//          System.out.println("Assignments for resource " + resource.getName() + ":");
// 
//          for (ResourceAssignment assignment : resource.getTaskAssignments())
//          {
//             Task task = assignment.getTask();
//             System.out.println("   " + task.getName());
//          }
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * This method lists any notes attached to tasks.
//     *
//     * @param file MPX file
//     */
//    private static void xlistTaskNotes(ProjectFile file)
//    {
//       for (Task task : file.getAllTasks())
//       {
//          String notes = task.getNotes();
// 
//          if (notes != null && notes.length() != 0)
//          {
//             System.out.println("Notes for " + task.getName() + ": " + notes);
//          }
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * This method lists any notes attached to resources.
//     *
//     * @param file MPX file
//     */
//    private static void xlistResourceNotes(ProjectFile file)
//    {
//       for (Resource resource : file.getAllResources())
//       {
//          String notes = resource.getNotes();
// 
//          if (notes != null && notes.length() != 0)
//          {
//             System.out.println("Notes for " + resource.getName() + ": " + notes);
//          }
//       }
// 
//       System.out.println();
//    }
// 
//    /**
//     * This method lists task predecessor and successor relationships.
//     *
//     * @param file project file
//     */
//    private static void xlistRelationships(ProjectFile file)
//    {
//       for (Task task : file.getAllTasks())
//       {
//          System.out.print(task.getID());
//          System.out.print('\t');
//          System.out.print(task.getName());
//          System.out.print('\t');
// 
//          dumpRelationList(task.getPredecessors());
//          System.out.print('\t');
//          dumpRelationList(task.getSuccessors());
//          System.out.println();
//       }
//    }
// 
//    /**
//     * Internal utility to dump relationship lists in a structured format
//     * that can easily be compared with the tabular data in MS Project.
//     * 
//     * @param relations relation list
//     */
//    private static void dumpRelationList(List<Relation> relations)
//    {
//       if (relations != null && relations.isEmpty() == false)
//       {
//          if (relations.size() > 1)
//          {
//             System.out.print('"');
//          }
//          boolean first = true;
//          for (Relation relation : relations)
//          {
//             if (!first)
//             {
//                System.out.print(',');
//             }
//             first = false;
//             System.out.print(relation.getTargetTask().getID());
//             Duration lag = relation.getLag();
//             if (relation.getType() != RelationType.FINISH_START || lag.getDuration() != 0)
//             {
//                System.out.print(relation.getType());
//             }
// 
//             if (lag.getDuration() != 0)
//             {
//                if (lag.getDuration() > 0)
//                {
//                   System.out.print("+");
//                }
//                System.out.print(lag);
//             }
//          }
//          if (relations.size() > 1)
//          {
//             System.out.print('"');
//          }
//       }
//    }
// 
//    /**
//     * List the slack values for each task.
//     * 
//     * @param file ProjectFile instance
//     */
//    private static void xlistSlack(ProjectFile file)
//    {
//       for (Task task : file.getAllTasks())
//       {
//          System.out.println(task.getName() + " Total Slack=" + task.getTotalSlack() + " Start Slack=" + task.getStartSlack() + " Finish Slack=" + task.getFinishSlack());
//       }
//    }
// 
//    /**
//     * List details of all calendars in the file.
//     * 
//     * @param file ProjectFile instance
//     */
//    private static void xlistCalendars(ProjectFile file)
//    {
//       for (ProjectCalendar cal : file.getBaseCalendars())
//       {
//          System.out.println(cal.toString());
//       }
// 
//       for (ProjectCalendar cal : file.getResourceCalendars())
//       {
//          System.out.println(cal.toString());
//       }
//    }
// 
// }
}