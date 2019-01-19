import java.io.*;
import java.util.*;
public class SJF
{
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
        SJF obj=new SJF();
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
    void go()throws IOException
    {
        File f=new File("process.txt");
        BufferedReader br=new BufferedReader(new FileReader(f));
        String st="";
        //System.out.println("Opened file");
        Proc p[]=new Proc[100];
        int count=0,i,val,time=0,complete=0,min=10000,eNext=0,last=-1;
        while ((st = br.readLine()) != null) 
        {
            //System.out.println(st);
            String x[]=st.split(" ");
            p[count]=new Proc(Integer.parseInt(x[0]));
            i=1;
            while(true)
            {
                val=Integer.parseInt(x[i]);
                if(val==-1)
                break;
                else if(p[count].CPU.size()==p[count].IO.size())
                p[count].CPU.add(val);
                else
                p[count].IO.add(val);
                i++;
            }
            count++;
        }
        for(i=0;i<count;i++)
        {
            display(p[i]);
            System.out.println();
        }
        while(complete<count)
        {
            min=100000;
            for(i=0;i<count;i++)
            {
                if(p[i].atime>time||p[i].atime<0)
                continue;
                if(p[i].IO.size()>0&& p[i].lsch!=-1 && time-p[i].lsch>=p[i].IO.getFirst()&&!p[i].ready)
                {
                    p[i].ready=true;
                    if(p[i].IO.size()>0)
                    p[i].IO.removeFirst();
                }
                if(i!=last&&p[i].ready&&p[i].CPU.getFirst()<min)
                {
                    min=p[i].CPU.getFirst();
                    eNext=i;
                }
            }
            if(min==100000)
            {
                time++;
                last=-1;
                continue;
            }
            p[eNext].lsch=time;
            last=eNext;
            time=time+min;
            p[eNext].ready=false;
            p[eNext].CPU.removeFirst();
            System.out.println("Process "+(eNext+1)+": ("+min+") ");
            if(p[eNext].CPU.size()==0)
            {
                complete++;
                p[eNext].atime=-1;
            }
        }
    }
}

