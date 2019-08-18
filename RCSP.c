#include<stdio.h>
#include<stdlib.h>
struct state
{
  int p;                //parent to the current node
  int curr_node;
  int res[100];         //resource consumed on the path
  int s;                //dummy resource
  int cost;             //cost of the path
};
int Extend(struct state *label1,struct state *label2,int cost,int *res,int *ver_r,int *L,int *U,int k,int j)
{
  label2->p=label1->curr_node;                                //making current node parent of new node
  label2->curr_node=j;
  label2->s=1;                                                //dummy resource update
  label2->cost=label1->cost + cost;
  int flag=0;
  for(int i=0;i<k;i++)                                      //update of resources
  {
    label2->res[i]=label1->res[i]+res[i]+ ver_r[i];
    if(label2->res[i]<=L[i]||label2->res[i]>=U[i])          //check for resource contraint/feasibility
      flag =1;
  }
  if(flag==1)                                               //return 1 if not feasible
    return 0;
  else
    return 1;
}
int EFF(struct state *curr_label,struct state *checklabel,int k)    //Function to check dominance
{ int flag=0;
  if(checklabel->s<=curr_label->s)                                  //checking whether checklabel dominates the current label or not
  {
    if(checklabel->cost<=curr_label->cost)
    {
      for(int i=0;i<k;i++)
      {
        if(checklabel->res[i]<=curr_label->res[i])
        {
          flag++;
        }
      }
    }
  }
  if(flag==k)                                           // If check label dominates then current label provides no better route and hence can be neglected
    return 1;
  else
    return 0;
}
struct queque         //for keeping a track of edges up to which paths have been figured out
{
  int E[100];
  int head;
  int tail;
};
int isempty(struct queque *Q)
{
  if(Q->head==Q->tail)
  {
    return 1;
  }
  else
    return 0;
}
void enqueque(struct queque *Q,int i)
{
  Q->E[Q->tail]=i;
  Q->tail=Q->tail + 1;
}
int dequeque(struct queque *Q)
{ if(isempty(Q))
  {
    return 0;
  }
  Q->E[Q->head]=0;
  Q->head=Q->head + 1;
  return 1;
}
struct list                           //list of states generated
{
  int li[100];
  int end;                          //stores the number of states generated for a particular  vertex
};
int main()
{
  int n,m,k;                            //nodes, arcs and resources
  char c;
  FILE* myFile=fopen("rcsp.txt","r");   //opening the file
  fscanf(myFile,"%d %d %d\n",&n,&m,&k);
  int L[k],U[k];                         //lower and upper bound for resources
  int ver_r[n][k];                       //resource consumption for each vertex
  int adj_matrix[n][n];                 //adjacency matrix
  int res[n][n][k];                     //resource consumption for each arc
  for(int x=0;x<n;x++)
  {
    for(int y=0;y<n;y++)
    {
      adj_matrix[x][y]=0;
    }
  }
  for(int i=0;i<k;i++)                //taking lower limit as input
  {
    fscanf(myFile,"%d\n",&L[i]);
  }
  for(int i=0;i<k;i++)                //taking upper limit as input
  {
    fscanf(myFile,"%d\n",&U[i]);
  }
  for(int i=0;i<n;i++)
  { for(int j=0;j<k;j++)
    {
        fscanf(myFile,"%d",&ver_r[i][j]);
    }
    fscanf(myFile,"\n");
  }
  for(int i=0;i<m;i++)
  { int p,c,cost;
    fscanf(myFile,"%d %d %d",&p,&c,&cost);
    adj_matrix[p][c]=cost;
    for(int j=0;j<k;j++)
    {
      fscanf(myFile,"%d",&res[p][c][j]);
    }
    fscanf(myFile,"\n");
  }
   struct list label_list[n];           //list of states for each vertex
   struct state *label=malloc(1*sizeof(struct state));
   int size=0;                           //size variable to dynaically allot memory for states generated
   label[0].p=-1;
   label[0].curr_node=0;
   label[0].s=1;
   label[0].cost=0;
   label_list[0].li[0]=0;
   label_list[0].end=1;
   for(int i=1;i<n;i++)
   {
     label_list[i].end=0;
   }
   for(int i=0;i<k;i++)                 //resource consumed for source vertex
   {
     label[0].res[i]=ver_r[0][i];
   }
   struct queque Q;
   Q.head=0;
   Q.tail=0;
   enqueque(&Q,0);
   while(!isempty(&Q))              //until Q is empty
   {
     int i=Q.E[Q.head];             //vertex selection
     int j=0;
     int no_states=label_list[i].end;
     while(j<=no_states)             //for all states
     {
       for(int l=0;l<n;l++)         //for all neighbours
       {
         if(adj_matrix[i][l]>0)
         { size++;
           label=realloc(label,(size+1)*sizeof(struct state));          //dynamically reallocating memory for the labels/state
           int temp_res[k];
           int temp_ver[k];
           for(int p=0;p<k;p++)
           {
             temp_res[p]=res[i][l][p];
             temp_ver[p]=ver_r[l][p];
           }
           int feasible=Extend(&label[i],&label[size],adj_matrix[i][l],temp_res,temp_ver,L,U,k,l);
           if(feasible==0)                      //if solution not feasible then do not save the state
              size--;
            else
            { int flag=0;
              for(int p=0;p<label_list[l].end;p++)        //If solution feasible then check for dominance
              {
                if(EFF(&label[size],&label[label_list[l].li[p]],k))
                  flag=1;
              }
              if(flag==0||label_list[l].end==0)
              {
                label_list[l].li[label_list[l].end]=size;
                label_list[l].end++;
                enqueque(&Q,l);
              }
              else
                size--;
            }
         }
        }
       j++;
     }
    dequeque(&Q);
   }
   printf("%d\n",label[label_list[n-1].li[label_list[n-1].end-1]].cost);        //printing min cost
   int path=n-1;
   printf("%d",n-1);
   while(path>0)
   {
     printf("->%d",label[label_list[path].li[label_list[path].end-1]].p);
     path=label[label_list[path].li[label_list[path].end-1]].p;
   }
   return 0;

}
