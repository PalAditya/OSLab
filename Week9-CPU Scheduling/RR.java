import java.io.*;
import java.util.*;
public class RR
{
    int quantum;
    RR(int r)
    {
        quantum=r;
    }
    class Proc
    {
        LinkedList<Integer> CPU;
        LinkedList<Integer> IO;
        int atime,lsch;
        boolean ready;
        Proc(int x)
        {
            atime=x;
            CPU=new LinkedList<>();
            IO=new LinkedList<>();
            ready=true;
            lsch=-1;
        }
    }
    public static void main(String args[])throws IOException
    {
        RR obj=new RR(5);
        obj.go();
    }
    void display(Proc x)
    {
        System.out.print("Time: "+x.atime+"     ");
        Iterator itr=x.CPU.iterator();
        while(itr.hasNext())
        {
            System.out.print(itr.next()+" ");
        }
        System.out.print("      ");
        Iterator itr2=x.IO.iterator();
        while(itr2.hasNext())
        {
            System.out.print(itr2.next()+" ");
        }
    }
    int getAndSetMin(int arr[])
    {
        int x=arr[0],i,ind=0;
        for(i=1;i<100;i++)
        {
            if(arr[i]<x&&arr[i]!=100000)
            {
                ind=i;
                x=arr[i];
            }
        }
        //arr[ind]=100000;
        //System.out.println("Found min "+x);
        if(x!=100000)
            return ind;
        else
            return -1;
    }
    void go()throws IOException
    {
        File f=new File("D:/Documents/process.txt");
        BufferedReader br=new BufferedReader(new FileReader(f));
        String st="";
        //System.out.println("Opened file");
        Proc proc[]=new Proc[100];
        int art[]=new int[100];
        Arrays.fill(art,1000000);
        int count=0,i,val,time=0,complete=0,considered=0,eNext=0,last=-1,j=1,iq=-1,index=0;
        boolean anyScheduled=false;
        while ((st = br.readLine()) != null) 
        {
            String x[]=st.split(" ");
            val=Integer.parseInt(x[0]);
            proc[count]=new Proc(val);
            art[count]=val;
            i=1;
            while(true)
            {
                val=Integer.parseInt(x[i]);
                if(val==-1)
                break;
                else if(proc[count].CPU.size()==proc[count].IO.size())
                proc[count].CPU.add(val);
                else
                proc[count].IO.add(val);
                i++;
            }
            count++;
        }
        index=count;
        for(i=0;i<count;i++)
        {
            display(proc[i]);
            System.out.println();
        }        
        while(complete<count)
        {
            anyScheduled=false;
            for(i=0;i<count+1;i++)
            {
               if(i==count)
               {
                   if(!anyScheduled)
                   time++;
                   continue;
               }
               if(proc[i].atime==-1||proc[i].atime>time)
               continue;
               considered++;
               if(!proc[i].ready)
               {
                   if(proc[i].IO.size()>0)
                   {
                       val=proc[i].IO.getFirst();
                       if(proc[i].lsch+val<=time)
                       {
                           proc[i].ready=true;
                           proc[i].IO.removeFirst();
                       }
                   }
               }
               if(!proc[i].ready)
               continue;
               else
               anyScheduled=true;
               if(proc[i].CPU.getFirst()<=quantum)
               {
                   time+=proc[i].CPU.getFirst();
                   System.out.println("Process "+(i+1)+":("+proc[i].CPU.getFirst()+")");
                   proc[i].CPU.removeFirst();
                   if(proc[i].CPU.size()==0)
                   {
                       complete++;
                       proc[i].atime=-1;
                   }
                   else
                   {
                       proc[i].lsch=time;
                       proc[i].ready=false;
                   }
               }
               else
               {
                   time+=quantum;
                   System.out.println("Process "+(i+1)+":("+quantum+")");
                   val=proc[i].CPU.getFirst();
                   proc[i].CPU.removeFirst();
                   proc[i].CPU.addFirst(val-quantum);
                   proc[i].lsch=time;
               }
            }
        }
    }
}