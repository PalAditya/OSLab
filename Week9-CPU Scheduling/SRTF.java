import java.io.*;
import java.util.*;
public class SRTF
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
        SRTF obj=new SRTF();
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
        Proc p[]=new Proc[100];
        int art[]=new int[100];
        Arrays.fill(art,1000000);
        int count=0,i,val,time=0,complete=0,min=10000,eNext=0,last=-1,j=1,iq=-1,index=0;
        while ((st = br.readLine()) != null) 
        {
            //System.out.println(st);
            String x[]=st.split(" ");
            val=Integer.parseInt(x[0]);
            //System.out.println(val);
            p[count]=new Proc(val);
            art[count]=val;
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
        //Arrays.sort(art);
        index=count;
        for(i=0;i<count;i++)
        {
            display(p[i]);
            System.out.println();
        }        
        while(complete<count&&j<10)
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
            last=eNext;
            iq=getAndSetMin(art);
            if(iq!=-1&&time+min>art[iq])
            {
                //System.out.println("If");
                p[eNext].CPU.removeFirst();
                if(time+min-art[iq]!=0)
                    p[eNext].CPU.addFirst(time+min-art[iq]);
                else
                {
                    if(p[eNext].CPU.size()==0)
                    {
                        complete++;
                        p[eNext].atime=-1;
                    }
                }
                if(art[iq]-time>0)
                System.out.println("Process "+(eNext+1)+": ("+(art[iq]-time)+") ");
                time=art[iq];
                art[iq]=100000;
                last=-1;
                //p[eNext].lsch=-1;
                //j++;
            }
            else
            {
                //System.out.println("Else");
                time=time+min;
                p[eNext].ready=false;
                p[eNext].CPU.removeFirst();
                if(min>0)
                System.out.println("Process "+(eNext+1)+": ("+min+") ");
                if(p[eNext].CPU.size()==0)
                {
                    complete++;
                    p[eNext].atime=-1;
                }
                if(p[eNext].IO.size()>0)
                {
                    art[index++]=p[eNext].IO.getFirst()+time;
                }
            }
            p[eNext].lsch=time;
            //System.out.println("Time: "+time);
        }
    }
}