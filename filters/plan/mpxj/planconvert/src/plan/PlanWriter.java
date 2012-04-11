/*
 * file:       PlanWriter.java
 * author:     Jon Iles
 * copyright:  (c) Packwood Software 2005
 * date:       Mar 16, 2007
 */

/*
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

package plan;

import java.io.IOException;
import java.io.OutputStream;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.text.DateFormat;
import java.util.TimeZone;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;

import net.sf.mpxj.ConstraintType;
import net.sf.mpxj.DateRange;
import net.sf.mpxj.Day;
import net.sf.mpxj.DayType;
import net.sf.mpxj.Duration;
import net.sf.mpxj.ProjectCalendar;
import net.sf.mpxj.ProjectCalendarException;
import net.sf.mpxj.ProjectCalendarHours;
import net.sf.mpxj.ProjectCalendarDateRanges;
import net.sf.mpxj.ProjectFile;
import net.sf.mpxj.ProjectHeader;
import net.sf.mpxj.Relation;
import net.sf.mpxj.RelationType;
import net.sf.mpxj.Resource;
import net.sf.mpxj.ResourceType;
import net.sf.mpxj.Rate;
import net.sf.mpxj.ResourceAssignment;
import net.sf.mpxj.ScheduleFrom;
import net.sf.mpxj.TimephasedResourceAssignment;
import net.sf.mpxj.TimeUnit;
import net.sf.mpxj.Task;
import net.sf.mpxj.TaskType;
import net.sf.mpxj.utility.DateUtility;
import net.sf.mpxj.utility.Sequence;
import net.sf.mpxj.writer.AbstractProjectWriter;

import plan.schema.Account;
import plan.schema.Accounts;
import plan.schema.ActualEffort;
import plan.schema.Appointment;
// import plan.schema.Calendar;
import plan.schema.CompletionEntry;
import plan.schema.Costplace;
import plan.schema.Criticalpath;
import plan.schema.CriticalpathList;
// import plan.schema.Day;
import plan.schema.Default;
import plan.schema.Document;
import plan.schema.Documents;
import plan.schema.Estimate;
import plan.schema.Level;
import plan.schema.Levels;
import plan.schema.Locale;
import plan.schema.Node;
import plan.schema.ObjectFactory;
import plan.schema.Plan;
import plan.schema.Progress;
import plan.schema.Project;
// import plan.schema.Relation;
import plan.schema.ResourceGroup;
import plan.schema.ResourcegroupRequest;
// import plan.schema.Resource;
import plan.schema.ResourceRequest;
import plan.schema.ResourceTeams;
import plan.schema.Schedule;
import plan.schema.Schedules;
import plan.schema.StandardWorktime;
// import plan.schema.Task;
import plan.schema.Team;
import plan.schema.UsedEffort;
import plan.schema.WbsDefinition;
import plan.schema.Weekday;
import plan.schema.Interval;

/**
 * This class creates a new Plan file from the contents of 
 * a ProjectFile instance.
 */
public final class PlanWriter extends AbstractProjectWriter
{
    /**
    * {@inheritDoc}
    */
    public void write(ProjectFile projectFile, OutputStream stream) throws IOException
    {
        try
        {
            m_projectFile = projectFile;

            if (CONTEXT == null)
            {
            throw CONTEXT_EXCEPTION;
            }

            Marshaller marshaller = CONTEXT.createMarshaller();
            marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);
            if (m_encoding != null)
            {
            marshaller.setProperty(Marshaller.JAXB_ENCODING, m_encoding);
            }

            m_factory = new ObjectFactory();
            m_plan = m_factory.createPlan();
            writePlan();

            marshaller.marshal(m_plan, stream);
        }

        catch (JAXBException ex)
        {
            throw new IOException(ex.toString());
        }

        finally
        {
            m_projectFile = null;
            m_factory = null;
            m_planProject = null;
            m_plan = null;
        }
    }

    /**
    * This method writes a Plan xml file.
    */
    private void writePlan() throws JAXBException
    {
        m_plan.setEditor("mpxjConverter");
        m_plan.setVersion("0.6.5");
        m_plan.setMime("application/x-vnd.kde.plan");

        m_planProject = m_factory.createProject();
        m_plan.getProjectOrPlanOrSchedule().add(m_planProject);
        writeProject();
        writeStandardWorktime();
        writeLocale();
        writeCalendars();
        writeResourceGroups();
        writeTasks();
        writeRelations();
        writeSchedules();
    }
    /**
    * This method writes project header data to a Plan file.
    */
    private void writeProject()
    {
        ProjectHeader mpxjHeader = m_projectFile.getProjectHeader();

//       m_planProject.setCompany(mpxjHeader.getCompany());
        m_planProject.setId(mpxjHeader.getUniqueID());

        String name = mpxjHeader.getProjectTitle();
        if (name == null)
        {
            name = mpxjHeader.getName();
        }
        List<Task> tasks = m_projectFile.getChildTasks();
        if (name == null && ! tasks.isEmpty())
        {
            name = tasks.get(0).getName();
        }
        if (name == null)
        {
            name = "Imported project";
        }
        m_planProject.setLeader(mpxjHeader.getManager());
        m_planProject.setName(name);
        m_planProject.setStartTime(getDateTimeString(m_projectFile.getStartDate()));
        m_planProject.setEndTime(getDateTimeString(m_projectFile.getFinishDate()));

        String s = "MustStartOn";
        if (mpxjHeader.getScheduleFrom() == ScheduleFrom.FINISH)
            s = "MustFinishOn";
        m_planProject.setScheduling(s);

        Calendar cal = Calendar.getInstance();
        m_planProject.setTimezone(cal.getTimeZone().getID());
   }

    /**
    * This method writes estimate conversion data to a Plan file.
    */
    private void writeStandardWorktime()
    {
        ProjectHeader mpxjHeader = m_projectFile.getProjectHeader();

        StandardWorktime planStandardWorktime = m_factory.createStandardWorktime();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(planStandardWorktime);

        planStandardWorktime.setDay(getMinutesString(mpxjHeader.getMinutesPerDay().longValue()));
        planStandardWorktime.setWeek(getMinutesString(mpxjHeader.getMinutesPerWeek().longValue()));
        planStandardWorktime.setMonth(getMinutesString(mpxjHeader.getMinutesPerDay().longValue()*22));
        planStandardWorktime.setYear(getMinutesString(mpxjHeader.getMinutesPerDay().longValue()*220));
    }

    /**
    * This method writes estimate conversion data to a Plan file.
    */
    private void writeLocale()
    {
        ProjectHeader mpxjHeader = m_projectFile.getProjectHeader();

        Locale planLocale = m_factory.createLocale();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(planLocale);

        planLocale.setCurrencySymbol(mpxjHeader.getCurrencySymbol());
        planLocale.setCurrencyDigits(getIntegerString(mpxjHeader.getCurrencyDigits()));
//         planLocale.setPositivePrefixCurrencySymbol();
//         planLocale.setNegativePrefixCurrencySymbol();
//         planLocale.setPositiveMonetarySignPosition();
//         planLocale.setNegativeMonetarySignPosition();

    }

    /**
    * This method writes calendar data to a Plan file.
    *
    * @throws JAXBException on xml creation errors
    */
    private void writeCalendars() throws JAXBException
    {
        //
        // Process each calendar in turn
        //    
        for (ProjectCalendar mpxjCalendar : m_projectFile.getBaseCalendars())
        {
            plan.schema.Calendar planCalendar = m_factory.createCalendar();
            m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(planCalendar);
            writeCalendar(mpxjCalendar, planCalendar);
        }
    }


    /**
    * This method writes data for a single calendar to a Plan file.
    *
    * @param mpxjCalendar MPXJ calendar instance
    * @param day a MPJX Day instance
    * @param planCalendar Plan calendar instance
    * @throws JAXBException on xml creation errors
    */
    private void writeWeekday(ProjectCalendar mpxjCalendar, Day day, plan.schema.Weekday planWeekday) throws JAXBException
    {
        int[] dayno = new int[] {-1, 6, 0, 1, 2, 3, 4, 5};
        planWeekday.setDay(getIntegerString(dayno[day.getValue()]));
        planWeekday.setState(getWorkingDayString(mpxjCalendar, day));

        if (mpxjCalendar.getWorkingDay(day) == DayType.WORKING)
        {
            for (DateRange mpxjRange : mpxjCalendar.getCalendarHours(day))
            {
                Date rangeStart = mpxjRange.getStart();
                Date rangeEnd = mpxjRange.getEnd();

                if (rangeStart != null && rangeEnd != null)
                {
                    Interval interval = m_factory.createInterval();
                    planWeekday.getInterval().add(interval);

                    interval.setStart(getTimeString(rangeStart));
                    interval.setLength(getIntegerString(rangeEnd.getTime() - rangeStart.getTime()));
                }
            }
        }
    }

    /**
    * This method writes data for a single calendar to a Plan file.
    *
    * @param mpxjCalendar MPXJ calendar instance
    * @param planCalendar Plan calendar instance
    * @throws JAXBException on xml creation errors
    */
    private void writeCalendar(ProjectCalendar mpxjCalendar, plan.schema.Calendar planCalendar) throws JAXBException
    {
        planCalendar.setId(getIntegerString(mpxjCalendar.getUniqueID()));
        planCalendar.setName(getString(mpxjCalendar.getName()));

        //
        // Set working and non working days for weekdays
        //
        Weekday weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.MONDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.TUESDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.WEDNESDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.THURSDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.FRIDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.SATURDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);
        weekday = m_factory.createWeekday();
        writeWeekday(mpxjCalendar, Day.SUNDAY, weekday);
        planCalendar.getCalendarOrWeekdayOrDay().add(weekday);

        //
        // Set working and non working days for specific dates
        //
        for (ProjectCalendarException mpxjCalendarException : mpxjCalendar.getCalendarExceptions())
        {
            Date rangeStartDay = mpxjCalendarException.getFromDate();
            Date rangeEndDay = mpxjCalendarException.getToDate();
            while (DateUtility.getDayStartDate(rangeStartDay).getTime() == DateUtility.getDayEndDate(rangeEndDay).getTime())
            {
                System.out.println("Day: " + mpxjCalendarException);
                //
                // Exception covers a single day
                //
                plan.schema.Day day = m_factory.createDay();
                planCalendar.getCalendarOrWeekdayOrDay().add(day);
                day.setDate(getDateString(mpxjCalendarException.getFromDate()));
                day.setState(mpxjCalendarException.getWorking() ? "2" : "1");
            }
        }

        //
        // Process any derived calendars
        //
        for (ProjectCalendar mpxjDerivedCalendar : mpxjCalendar.getDerivedCalendars())
        {
            plan.schema.Calendar planDerivedCalendar = m_factory.createCalendar();
            planCalendar.getCalendarOrWeekdayOrDay().add(planDerivedCalendar);
            writeCalendar(mpxjDerivedCalendar, planDerivedCalendar);
        }
    }

    /**
    * This method writes resource group data to a Plan file.
    */
    private void writeResourceGroups()
    {
        ResourceGroup workresourcegroup = m_factory.createResourceGroup();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(workresourcegroup);

        workresourcegroup.setId("UniqueWorkResourceGroupId");
        workresourcegroup.setName("Resources");
        workresourcegroup.setType("Work");

        ResourceGroup materialresourcegroup = m_factory.createResourceGroup();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(materialresourcegroup);
        materialresourcegroup.setId("UniqueMaterialResourceGroupId");
        materialresourcegroup.setName("Materials");
        materialresourcegroup.setType("Material");

        ResourceGroup costresourcegroup = m_factory.createResourceGroup();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(costresourcegroup);
        costresourcegroup.setId("UniqueCostResourceGroupId");
        costresourcegroup.setName("Cost");
        costresourcegroup.setType("Material");

        for (Resource mpxjResource : m_projectFile.getAllResources())
        {
            if (mpxjResource.getType() == ResourceType.WORK)
            {
                writeResource(mpxjResource, workresourcegroup);
            }
            else if (mpxjResource.getType() == ResourceType.MATERIAL)
            {
                writeResource(mpxjResource, materialresourcegroup);
            }
            else
            {
                writeResource(mpxjResource, costresourcegroup);
            }
        }
    }
   /**
    * This method writes data for a single resource to a Plan file.
    *
    * @param mpxjResource MPXJ Resource instance
    * @param planGroup Plan ResourceGroup instance
    */
    private void writeResource(Resource mpxjResource, ResourceGroup planGroup)
    {
        plan.schema.Resource planResource = m_factory.createResource();
        planGroup.getResource().add(planResource);

        ProjectCalendar resourceCalendar = mpxjResource.getResourceCalendar();
        if (resourceCalendar != null)
        {
            planResource.setCalendarId(getIntegerString(resourceCalendar.getUniqueID()));
        }

        planResource.setEmail(mpxjResource.getEmailAddress());
        planResource.setId(getResourceId(mpxjResource));
        planResource.setName(getString(mpxjResource.getName()));
        planResource.setInitials(mpxjResource.getInitials());
        planResource.setType(mpxjResource.getType() == ResourceType.MATERIAL ? "Material" : "Work");
        planResource.setUnits("100");

        // TODO convert rate with different unit than hour
        Rate rate = mpxjResource.getStandardRate();
        if (rate != null) {
            planResource.setNormalRate(Double.toString(rate.getAmount()));
        }
        rate = mpxjResource.getOvertimeRate();
        if (rate != null) {
            planResource.setOvertimeRate(Double.toString(rate.getAmount()));
        }
    }

   /**
    * This method writes task data to a Plan file.
    *
    * @throws JAXBException on xml creation errors
    */
    private void writeTasks() throws JAXBException
    {
        for (Task task : m_projectFile.getChildTasks())
        {
            writeTask(task, m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams());
        }
    }

    /**
        * This method writes data for a single task to a Plan file.
        *
        * @param mpxjTask MPXJ Task instance
        * @param taskList list of child tasks for current parent
        */
    private void writeTask(Task mpxjTask, List<Object> tasks) throws JAXBException
    {
        plan.schema.Task planTask = m_factory.createTask();
        tasks.add(planTask);

        planTask.setId(getTaskId(mpxjTask));
        planTask.setWbs(mpxjTask.getWBS());
        planTask.setName(getString(mpxjTask.getName()));
        planTask.setScheduling(getSchedulingString(mpxjTask.getConstraintType()));

        writeEstimate(mpxjTask, planTask);
        writeRequests(mpxjTask, planTask);
        writeTaskSchedules(mpxjTask, planTask);

        String note = mpxjTask.getNotes();
        if (note != null )
        {
            planTask.setDescription(note);
        }
        writeProgress(mpxjTask, planTask);



      //
      // Write child tasks
      //
      for (Task task : mpxjTask.getChildTasks())
      {
         writeTask(task, planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress());
      }
   }

    /**
    * This method writes task resource requests to a Plan file.
    *
    */
    private void writeRequests(Task mpxjTask, plan.schema.Task planTask)
    {
        System.out.println("writeRequests: " + mpxjTask +", "+ planTask);
        if (mpxjTask.getMilestone() || ! mpxjTask.getChildTasks().isEmpty())
        {
            return;
        }
        ResourcegroupRequest planWorkGroup = null;
        ResourcegroupRequest planMaterialGroup = null;
        ResourcegroupRequest planCostGroup = null;
        for (ResourceAssignment mpxjAssignment : mpxjTask.getResourceAssignments())
        {
            System.out.println("writeRequests: " + mpxjAssignment + " : " + mpxjAssignment.getResource());
            if (mpxjAssignment.getResource() == null)
            {
                continue;
            }
            ResourcegroupRequest group = null;
            if (mpxjAssignment.getResource().getType() == ResourceType.MATERIAL)
            {
                if (planMaterialGroup == null)
                {
                    planMaterialGroup = m_factory.createResourcegroupRequest();
                    planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planMaterialGroup);
                    planMaterialGroup.setGroupId("UniqueMaterialResourceGroupId");
                }
                group = planMaterialGroup;
            }
            else if (mpxjAssignment.getResource().getType() == ResourceType.WORK)
            {
                if (planWorkGroup == null)
                {
                    planWorkGroup = m_factory.createResourcegroupRequest();
                    planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planWorkGroup);
                    planWorkGroup.setGroupId("UniqueWorkResourceGroupId");
                }
                group = planWorkGroup;
            }
            else if (mpxjAssignment.getResource().getType() == ResourceType.COST)
            {
                if (planCostGroup == null)
                {
                    planCostGroup = m_factory.createResourcegroupRequest();
                    planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planCostGroup);
                    planCostGroup.setGroupId("UniqueCostResourceGroupId");
                }
                group = planWorkGroup;
            }
            if (group != null)
            {
                writeRequest(mpxjAssignment, group);
            }
        }
    }
    /**
    * This method writes a task resource request to a Plan file.
    *
    */
    private void writeRequest(ResourceAssignment mpxjAssignment, ResourcegroupRequest planGroup)
    {
        ResourceRequest planResourceRequest = m_factory.createResourceRequest();
        planGroup.getResourceRequest().add(planResourceRequest);

        planResourceRequest.setResourceId(getResourceId(mpxjAssignment.getResource()));
        planResourceRequest.setUnits(getIntegerString(mpxjAssignment.getUnits()));
    }

    /**
    * This method writes task estimate data to a Plan file.
    *
    */
    private void writeEstimate(Task mpxjTask, plan.schema.Task planTask)
    {
        Estimate planEstimate = m_factory.createEstimate();
        planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planEstimate);

        planEstimate.setType(getEstimateType(mpxjTask.getType()));

        if (mpxjTask.getMilestone())
        {
            planEstimate.setExpected("0");
            planEstimate.setOptimistic("0");
            planEstimate.setPessimistic("0");
        }
        else
        {
            Duration duration = mpxjTask.getDuration();
            if (mpxjTask.getType() == TaskType.FIXED_DURATION)
            {
                // if duration is in working time a calendar must be added
                switch (duration.getUnits())
                {
                    case MINUTES :
                    case HOURS :
                    case DAYS :
                    case WEEKS :
                    case MONTHS :
                    case YEARS :
                    {
                        // add a calendar, use base calendar for now
                        planEstimate.setCalendarId(getIntegerString(m_projectFile.getBaseCalendars().get(0).getUniqueID()));
                        break;
                    }
                    default :
                    {
                        break;
                    }
                }
            }
            else // effort
            {
                // if duration is in calendar time, it must be
                // converted from ELAPSED_* (working time)
                switch (duration.getUnits())
                {
                    case ELAPSED_MINUTES :
                    {
                        duration = duration.convertUnits(TimeUnit.MINUTES, m_projectFile.getProjectHeader());
                        break;
                    }
                    case ELAPSED_HOURS :
                    {
                        duration = duration.convertUnits(TimeUnit.HOURS, m_projectFile.getProjectHeader());
                        break;
                    }
                    case ELAPSED_DAYS :
                    {
                        duration = duration.convertUnits(TimeUnit.DAYS, m_projectFile.getProjectHeader());
                        break;
                    }
                    case ELAPSED_WEEKS :
                    {
                        duration = duration.convertUnits(TimeUnit.WEEKS, m_projectFile.getProjectHeader());
                        break;
                    }
                    case ELAPSED_MONTHS :
                    {
                        duration = duration.convertUnits(TimeUnit.MONTHS, m_projectFile.getProjectHeader());
                        break;
                    }
                    case ELAPSED_YEARS :
                    {
                        duration = duration.convertUnits(TimeUnit.YEARS, m_projectFile.getProjectHeader());
                        break;
                    }
                    default :
                    {
                        break;
                    }
                }
            }
            planEstimate.setUnit(getUnitString(duration));
            String est = getEstimateString(duration);
            planEstimate.setExpected(est);
            planEstimate.setOptimistic(est);
            planEstimate.setPessimistic(est);
        }
    }

    /**
    * This method writes task progress data to a Plan file.
    *
    */
    private void writeProgress(Task mpxjTask, plan.schema.Task planTask)
    {
        if (mpxjTask.getActualStart() == null)
        {
            return;
        }
        Progress planProgress = m_factory.createProgress();
        planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planProgress);

        planProgress.setEntrymode("EnterEffortPerResource"); 
        planProgress.setStarted("1");
        planProgress.setStartTime(getDateTimeString(mpxjTask.getActualStart()));
        if (mpxjTask.getActualFinish() != null)
        {
            planProgress.setFinished("1");
            planProgress.setFinishTime(getDateTimeString(mpxjTask.getActualFinish()));
        }
        writeCompletion(mpxjTask, planProgress);
        writeUsedEffort(mpxjTask, planProgress);
    }
    /**
    * This method writes task progress data to a Plan file.
    *
    */
    private void writeCompletion(Task mpxjTask, Progress planProgress)
    {
        CompletionEntry planCompletionEntry = m_factory.createCompletionEntry();
        planProgress.getCompletionEntryOrUsedEffort().add(planCompletionEntry);

        // FIXME: how to get better data? (use time phased data?)

        Date date = mpxjTask.getActualStart();
        if (mpxjTask.getActualFinish() != null)
        {
            date = mpxjTask.getActualFinish();
        }
        planCompletionEntry.setDate(getDateTimeString(date));
        planCompletionEntry.setPercentFinished(getIntegerString(mpxjTask.getPercentageWorkComplete()));
        Duration work = mpxjTask.getActualWork();//TODO + mpxjTask.getActualOvertimeWork();
        planCompletionEntry.setPerformedEffort(getDurationString(work));
        work = mpxjTask.getRemainingWork();//TODO + mpxjTask.getRemainingOvertimeWork();
        planCompletionEntry.setRemainingEffort(getDurationString(work));
//         planCompletionEntry.setNote();
    }
    /**
    * This method writes task used effort data to a Plan file.
    *
    */
    private void writeUsedEffort(Task mpxjTask, Progress planProgress)
    {
        List<ResourceAssignment> resourceAssignments = mpxjTask.getResourceAssignments();
        if (resourceAssignments.isEmpty())
        {
            return;
        }
        UsedEffort planUsedEffort = m_factory.createUsedEffort();
        planProgress.getCompletionEntryOrUsedEffort().add(planUsedEffort);

        for (ResourceAssignment mpxjAssignment : mpxjTask.getResourceAssignments())
        {
            plan.schema.Resource planResource = m_factory.createResource();
            planUsedEffort.getResource().add(planResource);

            planResource.setId(getResourceId(mpxjAssignment.getResource()));
            writeActualEffort(mpxjAssignment, planResource);
        }
    }
    /**
    * This method writes resource actual effort data to a Plan file.
    *
    */
    private void writeActualEffort(ResourceAssignment mpxjAssignment, plan.schema.Resource planResource)
    {
        if (mpxjAssignment.getActualStart() == null)
        {
            return;
        }
        ActualEffort planActualEffort = m_factory.createActualEffort();
        planResource.getActualEffort().add(planActualEffort);

        List<TimephasedResourceAssignment> list = mpxjAssignment.getTimephasedComplete();
        if (list.isEmpty())
        {
            Date date = mpxjAssignment.getActualStart();
            if (mpxjAssignment.getActualFinish() != null)
            {
                date = mpxjAssignment.getActualFinish();
            }
            planActualEffort.setDate(getDateTimeString(date));
            planActualEffort.setOvertimeEffort(getDurationString(mpxjAssignment.getOvertimeWork()));
            planActualEffort.setNormalEffort(getDurationString(mpxjAssignment.getActualWork()));
        }
        else
        {
            for (TimephasedResourceAssignment mpxjTpa : list)
            {
                writeActualEffort(mpxjTpa, planActualEffort);
            }
        }
    }
    /**
    * This method writes resource actual effort data to a Plan file.
    *
    */
    private void writeActualEffort(TimephasedResourceAssignment mpxjTpa, ActualEffort planActualEffort)
    {
        Date currentDate = DateUtility.getDayStartDate(mpxjTpa.getStart());
        long end = DateUtility.getDayEndDate(mpxjTpa.getFinish()).getTime();
        while (currentDate.getTime() < end)
        {
            planActualEffort.setDate(getDateString(currentDate));
            planActualEffort.setNormalEffort(getDurationString(mpxjTpa.getWorkPerDay()));
            currentDate = addDays(currentDate, 1);
        }
    }

    /**
    * This method writes task scheduling data to a Plan file.
    *
    */
    private void writeTaskSchedules(Task mpxjTask, plan.schema.Task planTask)
    {
        Schedules planSchedules = m_factory.createSchedules();
        planTask.getTaskOrEstimateOrDocumentsOrResourcegroupRequestOrSchedulesOrProgress().add(planSchedules);

        Schedule planSchedule = m_factory.createSchedule();
        planSchedules.getPlanOrSchedule().add(planSchedule);
        writeTaskSchedule(mpxjTask, planSchedule);
    }

    /**
    * This method writes task scheduling data to a Plan file.
    *
    */
    private void writeTaskSchedule(Task mpxjTask, Schedule planSchedule)
    {
        planSchedule.setId("1"); 
        planSchedule.setName("Plan");
        planSchedule.setType("Expected");
        planSchedule.setStart(getDateTimeString(mpxjTask.getStart()));
        planSchedule.setEnd(getDateTimeString(mpxjTask.getFinish()));
        planSchedule.setDuration(getDurationString(mpxjTask.getStart(), mpxjTask.getFinish()));

        planSchedule.setEarlystart(getDateTimeString(mpxjTask.getEarlyStart()));
        planSchedule.setEarlyfinish(getDateTimeString(mpxjTask.getEarlyFinish()));
        planSchedule.setLatestart(getDateTimeString(mpxjTask.getLateStart()));
        planSchedule.setLatefinish(getDateTimeString(mpxjTask.getLateFinish()));

        planSchedule.setPositiveFloat(getDurationString(mpxjTask.getTotalSlack()));
//         planSchedule.setNegativeFloat();
        planSchedule.setFreeFloat(getDurationString(mpxjTask.getFreeSlack()));
//         planSchedule.setStartFloat(getDurationString(mpxjTask.getStartSlack()));
//         planSchedule.setFinishFloat(getDurationString(mpxjTask.getFinishSlack()));

        planSchedule.setNotScheduled("0");
        planSchedule.setSchedulingConflict("0");
        planSchedule.setResourceError("0");
        planSchedule.setResourceOverbooked("0");
        planSchedule.setResourceNotAvailable("0");
        planSchedule.setInCriticalPath("0");
    }

    /**
    * This method writes task relations to a Plan file.
    */
    private void writeRelations()
    {
        for(Task mpxjTask : m_projectFile.getAllTasks())
        {
            List<Relation> predecessors = mpxjTask.getPredecessors();
            if (predecessors != null)
            {
                for (Relation rel : predecessors)
                {
                    Task child = rel.getSourceTask();
                    Task parent = rel.getTargetTask();
                    plan.schema.Relation planRelation = m_factory.createRelation();
                    planRelation.setParentId(getTaskId(parent));
                    planRelation.setChildId(getTaskId(child));
                    planRelation.setLag(getDurationString(rel.getLag()));
                    planRelation.setType(RELATIONSHIP_TYPES.get(rel.getType()));
                    m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(planRelation);
                }
            }
        }
    }

    /**
    * This method writes scheduling data to a Plan file.
    *
    */
    private void writeSchedules()
    {
        Schedules planSchedules = m_factory.createSchedules();
        m_planProject.getWbsDefinitionOrLocaleOrAccountsOrCalendarOrStandardWorktimeOrResourceGroupOrTaskOrRelationOrSchedulesOrResourceTeams().add(planSchedules);

        Plan planPlan = m_factory.createPlan();
        planSchedules.getPlanOrSchedule().add(planPlan);
        writePlan(planPlan);

        Schedule planSchedule = m_factory.createSchedule();
        planPlan.getProjectOrPlanOrSchedule().add(planSchedule);
        writeSchedule(planSchedule);
        writeAppointments(planSchedule);
    }

    /**
    * This method writes scheduling plan data to a Plan file.
    *
    */
    private void writePlan(Plan planPlan)
    {
        planPlan.setId("1");
        planPlan.setName("Plan");
    }

    /**
    * This method writes scheduling data to a Plan file.
    *
    */
    private void writeSchedule(Schedule planSchedule)
    {
        planSchedule.setId("1");
        planSchedule.setName("Plan");
        planSchedule.setType("Expected");
    }

    /**
    * This method writes appointment data to a Plan file.
    *
    */
    private void writeAppointments(Schedule planSchedule)
    {
        Date start = null;
        Date finish = null;
        Map<Resource,List<Task>> map = new HashMap<Resource,List<Task>>();
        for (ResourceAssignment mpxjAssignment : m_projectFile.getAllResourceAssignments())
        {
            Task task = mpxjAssignment.getTask();
            Resource resource = mpxjAssignment.getResource();
            if ( task != null && resource != null ) {
                Appointment planAppointment = m_factory.createAppointment();
                planSchedule.getCriticalpathListOrAppointment().add(planAppointment);

                planAppointment.setTaskId(getTaskId(task));
                planAppointment.setResourceId(getResourceId(resource));
                writeIntervals(planAppointment, mpxjAssignment);
            }
            if (DateUtility.compare(task.getStart(), start) < 0)
            {
                start = task.getStart();
            }
            if (DateUtility.compare(finish, task.getFinish() ) > 0)
            {
                finish = task.getFinish();
            }
        }
        if (start == null)
        {
            start = m_projectFile.getStartDate();
        }
        if (finish == null)
        {
            finish = m_projectFile.getFinishDate();
        }
        planSchedule.setStart(getDateTimeString(start));
        planSchedule.setEnd(getDateTimeString(finish));
        planSchedule.setDuration(getDurationString(start, finish));
    }

    /**
    * This method writes appointment intervals to a Plan file.
    *
    */
    private void writeIntervals(Appointment planAppointment, ResourceAssignment mpxjAssignment)
    {
        Date currentDate = mpxjAssignment.getStart();
        Date endDate = mpxjAssignment.getFinish();

        ProjectCalendar calendar = mpxjAssignment.getCalendar();
        Date endTime = DateUtility.getCanonicalTime(endDate);
        Calendar cal = Calendar.getInstance();

        Boolean found = false;
        while (currentDate.getTime() <= endDate.getTime())
        {
            Date startTime = DateUtility.getCanonicalTime(currentDate);

            cal.setTime(currentDate);
            Day day = Day.getInstance(cal.get(Calendar.DAY_OF_WEEK));
            ProjectCalendarDateRanges ranges = calendar.getHours(day);

            for (DateRange range : ranges)
            {
                Date rangeStart = range.getStart();
                Date rangeEnd = range.getEnd();
                Date start = setTime(currentDate, rangeStart);
                // FIXME timezone problems
                Date end = new Date(start.getTime() + rangeEnd.getTime() - rangeStart.getTime());
                // Check if interval within time frame
                if (start.getTime() < currentDate.getTime())
                {
                    start = currentDate;
                }
                if (end.getTime() > endDate.getTime())
                {
                    end = endDate;
                }
                if (start.getTime() < end.getTime()) {
                    Interval interval = m_factory.createInterval();
                    planAppointment.getInterval().add(interval);

                    interval.setStart(getDateTimeString(start));
                    interval.setEnd(getDateTimeString(end));
                    interval.setLoad(getIntegerString(mpxjAssignment.getUnits()));
                    found = true;
                }
            }
            cal.setTime(currentDate);
            cal.add(Calendar.DATE, 1);
            currentDate = DateUtility.getDayStartDate(cal.getTime());            
        }
        if (!found)
        {
            System.out.println("writeIntervals: No intervals! "+mpxjAssignment);
        }
    }

    /**
    * Convert a Plan date-time value into a Java date.
    * 
    * 20070222T080000Z
    * 
    * @param value Plan date-time
    * @return Java Date instance
    */
    private String getDateTime(Date value)
    {
        StringBuffer result = new StringBuffer(16);

        if (value != null)
        {
            Calendar cal = Calendar.getInstance();
            cal.setTime(value);

            result.append(m_fourDigitFormat.format(cal.get(Calendar.YEAR)));
            result.append(m_twoDigitFormat.format(cal.get(Calendar.MONTH) + 1));
            result.append(m_twoDigitFormat.format(cal.get(Calendar.DAY_OF_MONTH)));
            result.append("T");
            result.append(m_twoDigitFormat.format(cal.get(Calendar.HOUR_OF_DAY)));
            result.append(m_twoDigitFormat.format(cal.get(Calendar.MINUTE)));
            result.append(m_twoDigitFormat.format(cal.get(Calendar.SECOND)));
            result.append("Z");
        }

        return (result.toString());
    }

   /**
    * Convert an Integer value into a String.
    * 
    * @param value Integer value
    * @return String value
    */
   private String getIntegerString(Number value)
   {
      return (value == null ? null : Integer.toString(value.intValue()));
   }

   /**
    * Convert an int value into a String.
    * 
    * @param value int value
    * @return String value
    */
   private String getIntegerString(int value)
   {
      return (Integer.toString(value));
   }

   /**
    * Used to determine if a particular day of the week is normally 
    * a working day.
    * 
    * @param mpxjCalendar ProjectCalendar instance
    * @param day Day instance
    * @return boolean flag
    */
   private boolean isWorkingDay(ProjectCalendar mpxjCalendar, Day day)
   {
      boolean result = false;

      switch (mpxjCalendar.getWorkingDay(day))
      {
         case WORKING :
         {
            result = true;
            break;
         }

         case NON_WORKING :
         {
            result = false;
            break;
         }

         case DEFAULT :
         {
            result = isWorkingDay(mpxjCalendar.getBaseCalendar(), day);
            break;
         }
      }

      return (result);
   }

   /**
    * Returns a flag represented as a String, indicating if 
    * the supplied day is a working day.
    * 
    * @param mpxjCalendar MPXJ ProjectCalendar instance
    * @param day Day instance
    * @return boolean flag as a string
    */
   private String getWorkingDayString(ProjectCalendar mpxjCalendar, Day day)
   {
      String result = null;

      switch (mpxjCalendar.getWorkingDay(day))
      {
         case WORKING :
         {
            result = "2";
            break;
         }

         case NON_WORKING :
         {
            result = "1";
            break;
         }

         case DEFAULT :
         {
            result = "0";
            break;
         }
      }

      return (result);
   }

    /**
    * Convert a Java date into a Plan time.
    * 
    * 0800
    * 
    * @param value Java Date instance
    * @return Plan time value
    */
    private String getTimeString(Date value)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(value);
        int hours = cal.get(Calendar.HOUR_OF_DAY);
        int minutes = cal.get(Calendar.MINUTE);
        int seconds = cal.get(Calendar.SECOND);

        StringBuffer sb = new StringBuffer(8);
        sb.append(m_twoDigitFormat.format(hours));
        sb.append(':');
        sb.append(m_twoDigitFormat.format(minutes));
        sb.append(':');
        sb.append(m_twoDigitFormat.format(seconds));

        return (sb.toString());
    }

    /**
    * Convert a Java date into a Plan date.
    * 
    * 20070222
    * 
    * @param value Java Date instance
    * @return Plan date
    */
    private String getDateString(Date value)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(value);

        int year = cal.get(Calendar.YEAR);
        int month = cal.get(Calendar.MONTH) + 1;
        int day = cal.get(Calendar.DAY_OF_MONTH);

        StringBuffer sb = new StringBuffer(8);
        sb.append(m_fourDigitFormat.format(year));
        sb.append('-');
        sb.append(m_twoDigitFormat.format(month));
        sb.append('-');
        sb.append(m_twoDigitFormat.format(day));

        return (sb.toString());
    }

    /**
    * Convert a Java date into a Plan date-time string.
    * 
    * 20070222T080000Z
    * 
    * @param value Java date
    * @return Plan date-time string
    */
    private String getDateTimeString(Date value)
    {
        String result = null;
        if (value != null)
        {
            Calendar cal = Calendar.getInstance();
            cal.setTime(value);
            Integer offset = cal.getTimeZone().getOffset(cal.getTime().getTime()) / (60 *1000);
            StringBuffer sb = new StringBuffer(23);
            sb.append(m_fourDigitFormat.format(cal.get(Calendar.YEAR)));
            sb.append('-');
            sb.append(m_twoDigitFormat.format(cal.get(Calendar.MONTH) + 1));
            sb.append('-');
            sb.append(m_twoDigitFormat.format(cal.get(Calendar.DAY_OF_MONTH)));
            sb.append('T');
            sb.append(m_twoDigitFormat.format(cal.get(Calendar.HOUR_OF_DAY)));
            sb.append(':');
            sb.append(m_twoDigitFormat.format(cal.get(Calendar.MINUTE)));
            sb.append(':');
            sb.append(m_twoDigitFormat.format(cal.get(Calendar.SECOND)));
            sb.append(offset < 0 ? '-' : '+');
            sb.append(m_twoDigitFormat.format(offset / 60)); // hours
            sb.append(':');
            sb.append(m_twoDigitFormat.format(offset % 60)); // minutes
            result = sb.toString();
        }
        return result;
    }//2009-09-14T16:00:00+02:00

    private String getMinutesString(long value)
    {
        long min = value % 60;
        long hours = value / 60;
        return Long.toString(hours) + 'h' + Long.toString(min) + 'm'; 
    }
    /**
    * Converts the duration in milliseconds into a string
    * 
    * Plan represents durations as a number of milliseconds
    * formatted as a bb:cc:dd.e where:
    * a = days, bb = hours, cc = minutes, dd = seconds, e = milliseconds
    */
    private String getDurationString(long value)
    {
        String result;
        long ms = value % 1000;
        value /= 1000;
        long sec = value % 60;
        value /= 60;
        long min = value % 60;
        value /= 60;
        long hour = value % 24;
        value /= 24;
        long day = value;
        result = Long.toString(day) + ' ' + Long.toString(hour) + ':' + Long.toString(min) + ':' + Long.toString(sec) + '.' + Long.toString(ms);
        return result;
    }

    /**
    * Converts the duration between two dates into a string
    */
    private String getDurationString(Date start, Date end)
    {
        if (start == null || end == null) {
            return getDurationString(0);
        }
        return getDurationString(end.getTime() - start.getTime());
    }

    /**
    * Converts an MPXJ Duration instance into the string representation 
    * of a Plan duration.
    */
    private String getDurationString(Duration value)
    {
        double seconds = 0;

        if (value != null)
        {

            switch (value.getUnits())
            {
            case MINUTES :
            case ELAPSED_MINUTES :
            {
                seconds = value.getDuration() * 60;
                break;
            }

            case HOURS :
            case ELAPSED_HOURS :
            {
                seconds = value.getDuration() * (60 * 60);
                break;
            }

            case DAYS :
            {
                double minutesPerDay = m_projectFile.getProjectHeader().getMinutesPerDay().doubleValue();
                seconds = value.getDuration() * (minutesPerDay * 60);
                break;
            }

            case ELAPSED_DAYS :
            {
                seconds = value.getDuration() * (24 * 60 * 60);
                break;
            }

            case WEEKS :
            {
                double minutesPerWeek = m_projectFile.getProjectHeader().getMinutesPerWeek().doubleValue();
                seconds = value.getDuration() * (minutesPerWeek * 60);
                break;
            }

            case ELAPSED_WEEKS :
            {
                seconds = value.getDuration() * (7 * 24 * 60 * 60);
                break;
            }

            case MONTHS :
            {
                double minutesPerDay = m_projectFile.getProjectHeader().getMinutesPerDay().doubleValue();
                double daysPerMonth = m_projectFile.getProjectHeader().getDaysPerMonth().doubleValue();
                seconds = value.getDuration() * (daysPerMonth * minutesPerDay * 60);
                break;
            }

            case ELAPSED_MONTHS :
            {
                seconds = value.getDuration() * (30 * 24 * 60 * 60);
                break;
            }

            case YEARS :
            {
                double minutesPerDay = m_projectFile.getProjectHeader().getMinutesPerDay().doubleValue();
                double daysPerMonth = m_projectFile.getProjectHeader().getDaysPerMonth().doubleValue();
                seconds = value.getDuration() * (12 * daysPerMonth * minutesPerDay * 60);
                break;
            }

            case ELAPSED_YEARS :
            {
                seconds = value.getDuration() * (365 * 24 * 60 * 60);
                break;
            }

            default :
            {
                break;
            }
            }
        }
        return getDurationString((long)(seconds*1000));
    }

    /**
    * Converts an MPXJ Duration instance into the string representation 
    * of a Plan estimate.
    * If type is FIXED_WORK (or FIXED_UNITS) estimate must be in working time
    * If type is FIXED_DURATION estimate must be in calendar time
    */
    private String getEstimateString(Duration value)
    {
        double v =  value.getDuration();
        return "" + v;
    }
    /**
    * Converts an MPXJ Duration instance into the string representation 
    * of a Plan unit.
    */
    private String getUnitString(Duration value)
    {
        String unit = "d";
        if (value != null)
        {
            switch (value.getUnits())
            {
                case MINUTES :
                case ELAPSED_MINUTES :
                {
                    unit = "m";
                    break;
                }
                case HOURS :
                case ELAPSED_HOURS :
                {
                    unit = "h";
                    break;
                }
                case DAYS :
                case ELAPSED_DAYS :
                {
                    unit = "d";
                    break;
                }
                case WEEKS :
                case ELAPSED_WEEKS :
                {
                    unit = "w";
                    break;
                }
                case MONTHS :
                case ELAPSED_MONTHS :
                {
                    unit = "M";
                    break;
                }
                case YEARS :
                case ELAPSED_YEARS :
                {
                    unit = "Y";
                    break;
                }
                default :
                {
                    break;
                }
            }
        }
        return unit;
    }

    /**
    * Convert a string representation of the task type
    * into a TaskType instance.
    * 
    * @param value string value
    * @return TaskType value
    */
    private String getEstimateType(TaskType value)
    {
        String result = "Effort";
        if (value != null && value == TaskType.FIXED_DURATION)
        {
            result = "Duration";
        }
        return (result);
    }

    /**
    * Writes a string value, ensuring that null is mapped to an empty string.
    * 
    * @param value string value
    * @return string value
    */
    private String getString(String value)
    {
        return (value == null ? "" : value);
    }

    /**
    * Gets the resource id, using GUID if available else unique id
    */
    private String getResourceId(Resource resource)
    {
        return resource.getGUID() == null
                    ? getIntegerString(resource.getUniqueID())
                    : resource.getGUID().toString();
    }

    /**
    * Gets the task id, using GUID if available else unique id
    */
    private String getTaskId(Task task)
    {
        return task.getGUID() == null
                    ? getIntegerString(task.getUniqueID())
                    : task.getGUID().toString();
    }

    /**
    * Gets the scheduling constraint
    */
    private String getSchedulingString(ConstraintType mpjxConstraintType)
    {
        String result = "ASAP";
        switch (mpjxConstraintType)
        {
            case AS_SOON_AS_POSSIBLE :
                result = "ASAP";
                break;
            case AS_LATE_AS_POSSIBLE :
                result = "ALAP";
                break;
            case MUST_START_ON :
                result = "MustStartOn";
                break;
            case MUST_FINISH_ON :
                result = "MustFinishOn";
                break;
            case START_NO_EARLIER_THAN :
                result = "StartNotEarlier";
                break;
            case START_NO_LATER_THAN :
                result = "ASAP"; // Note: not supported
                break;
            case FINISH_NO_EARLIER_THAN :
                result = "ALAP"; // Note: not supported
                break;
            case FINISH_NO_LATER_THAN :
                result = "FinishNotLater";
                break;
            default:
                break;
        }
        return result;
    }

    /**
    * Given a date represented by a Date instance, set the time
    * component of the date based on the hours and minutes of the
    * time supplied by the Date instance.
    * 
    * @param date Date instance representing the date
    * @param canonicalTime Date instance representing the time of day
    * @return new Date instance with the required time set
    */
    public static Date setTime(Date date, Date time)
    {
        Date result;
        if (time == null)
        {
            result = date;
        }
        else
        {
            Calendar cal = Calendar.getInstance();
            cal.setTime(time);
            cal.set(Calendar.DAY_OF_YEAR, 1);
            cal.set(Calendar.YEAR, 1970);
            cal.set(Calendar.MILLISECOND, 0);
            Date canonicalTime = cal.getTime();

            result = DateUtility.getDayStartDate(date);
            long offset = canonicalTime.getTime();
            result = new Date(result.getTime() + offset);
        }
        return result;
    }

    /**
    * Add days to date
    */
    public Date addDays(Date date, int days)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.add(Calendar.DATE, days);
        return cal.getTime();
    }

    /**
    * Set the encoding used to write the file. By default UTF-8 is used.
    * 
    * @param encoding encoding name
    */
    public void setEncoding(String encoding)
    {
        m_encoding = encoding;
    }

    /**
    * Retrieve the encoding used to write teh file. If this value is null,
    * UTF-8 is used.
    * 
    * @return encoding name
    */
    public String getEncoding()
    {
        return m_encoding;
    }

    private String m_encoding;
    private ProjectFile m_projectFile;
    private ObjectFactory m_factory;
    private Plan m_plan;
    private Project m_planProject;

    private NumberFormat m_twoDigitFormat = new DecimalFormat("00");
    private NumberFormat m_fourDigitFormat = new DecimalFormat("0000");

    private static Map<RelationType, String> RELATIONSHIP_TYPES = new HashMap<RelationType, String>();
    static
    {
        RELATIONSHIP_TYPES.put(RelationType.FINISH_FINISH, "Finish-Finish");
        RELATIONSHIP_TYPES.put(RelationType.FINISH_START, "Finish-Start");
        RELATIONSHIP_TYPES.put(RelationType.START_FINISH, "Finish-Start"); //Note: START_FINISH not supported
        RELATIONSHIP_TYPES.put(RelationType.START_START, "Start-Start");
    }

    /**
    * Cached context to minimise construction cost.
    */
    private static JAXBContext CONTEXT;

    /**
    * Note any error occurring during context construction.
    */
    private static JAXBException CONTEXT_EXCEPTION;

    static
    {
        try
        {
            //
            // JAXB RI property to speed up construction
            //
            System.setProperty("com.sun.xml.bind.v2.runtime.JAXBContextImpl.fastBoot", "true");

            //
            // Construct the context
            //
            CONTEXT = JAXBContext.newInstance("plan.schema", PlanWriter.class.getClassLoader());
        }

        catch (JAXBException ex)
        {
            CONTEXT_EXCEPTION = ex;
            CONTEXT = null;
        }
    }
}
