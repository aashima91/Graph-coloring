
#include <cstdlib>
#include <iostream>
#include <cilk/cilk.h>
#include "cilktime.h"
#include <map>	
#include <set>
#include <tbb/mutex.h>
#include <cilk/reducer_opadd.h>
using namespace std;
using namespace cilk;
	int* deg;
	int* color;
    int* CopyColorMatrix;
	int** sp;
        cilk::reducer_opadd< int> no_vertex_colored;
       //degree calculation 
	void degree(int nv)
	{
		cilk_for(int i=0;i<nv;i++)
		{
            cilk::reducer_opadd< int> local_degree;
                    
			cilk_for(int j=0;j<nv;j++)
	
				if(sp[i][j]==1)
				local_degree++;
                deg[i]=local_degree.get_value();
				
	
			
		}
	}
	int count_non_colored(int nv,int i)
	{
		cilk::reducer_opadd< int> x;
		cilk_for(int j=0;j<nv;j++)
		{
			if(sp[i][j]==1 && color[j]==0 )
			{
				if (deg[i]<deg[j] || (deg[i]==deg[j] && i<j))
			{
				x++;
			}
		    }
	
		}
		return x.get_value();
	
	}
	
	int find_max(int nv)
	{
	
		int max_color=0;
		for(int i=0;i<nv;i++)
		{
			if(max_color<color[i])
				max_color=color[i];
	
		}
		return max_color;
	}
	int main()
	{
		int max_color;
		int nv,ne,u,v,count=0;
		cin>> nv >> ne;
	
		sp=(int **)calloc(nv,sizeof(int*));
		cilk_for(int i=0;i<nv;i++)
		{
			sp[i]=(int *)calloc(nv,sizeof(int));
		}
                
		while(count<ne)
		{
			cin>>u>>v;
			sp[u-1][v-1]=1;
			sp[v-1][u-1]=1;
			count++;
		}
	
	
		deg=(int*)calloc(nv,sizeof(int));
		color=(int*)calloc(nv,sizeof(int));
        CopyColorMatrix=(int*)calloc(nv,sizeof(int));
		int* backup=(int*)calloc(nv,sizeof(int));
		//cilk::reducer_opadd< int> no_vertex_colored;
		int flag=1;
	
	    static struct timeval time2,time1;
	    gettimeofday(&time1,NULL);
		degree(nv);
		while(no_vertex_colored.get_value()<nv)
		{
	
			cilk_for(int i=0;i<nv;i++)
			{
				if(color[i]==0)
				{
					int non_colored=count_non_colored(nv, i);
	
					int* colors_assigned=(int*)calloc(nv+1,sizeof(int));
					for(int j=0;j<nv;j++)
					{
						//cout<<"First for loop"<<endl;

						if(sp[i][j]==1 && color[j]!=0)
							colors_assigned[color[j]]=1;
					}
	
					int temp=0;
					for(int k=1;k<nv;k++)
					{
						//cout<<"Find unassigned color"<<endl;
						if(colors_assigned[k]==0)
						{
							temp=k;
							break;
						}
					}
		                       
					if(non_colored==0)
					{
						//cout<<"assigning color"<<endl;
						CopyColorMatrix[i]=temp;
						//cout<<"copy color matrix succeeded"<<endl;
						backup[i]=flag;
						//cout<<"backup done"<<endl;						
						no_vertex_colored++;
						//cout<<"color assigned"<<endl;
	
					}
				}
			}
			
			cilk_for(int i=0;i<nv;i++)
			{
                 color[i]=CopyColorMatrix[i];
				for(int j=0;j<nv;j++)
					if(sp[i][j]==1 && backup[j]==flag)
						deg[i]--;
            }
			flag++;
		}
       

	gettimeofday(&time2,NULL);
	long long int totalTime=(time2.tv_sec-time1.tv_sec)*1000000 + (time2.tv_usec-time1.tv_usec);
	cout<<totalTime<<endl; //in microsec
	
		max_color=find_max(nv);
		cout<<max_color<<"\n";
		for(int i=0;i<nv;i++)
		cout<<i+1<<" "<<color[i]<<"\n";
	
	}
	
	
	
