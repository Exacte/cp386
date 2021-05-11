#include <stdio.h>
#include <stdlib.h>

int n,m;
int **req,**cur_alloc,**max;
int *unalloc,*deadlocked;

void input();
void dl_correct();
int dl_detect();

void main(){
  int i,j,dl;
  printf("Deadlock Detection and correction\n");
  input();
  dl=dl_detect();
  while(dl){
    dl_correct();
    dl=dl_detect();
  }
  printf("\n");
}

void input(){
  int i,j;

  //Obtain n(number of process) and m(number of resources).
  printf("Enter n m: ");
  scanf("%d %d", &n, &m);

  //Allocate matries req(requested resources), cur_alloc(currently allocated resources) and max(matrix of maximum resources in the system) to size n,m.
  req = (int **)malloc(n*sizeof(int *));
  cur_alloc = (int **)malloc(n*sizeof(int *));
  max = (int **)malloc(n*sizeof(int *));
  for(i=0; i<n; i++){
    req[i] = (int *)malloc(m*sizeof(int));
    cur_alloc[i] = (int *)malloc(m*sizeof(int));
    max[i] = (int *)malloc(m*sizeof(int));
  }
  //Allocate vector unalloc(currently unallocated resources) to size m.
  unalloc = (int *)malloc(m*sizeof(int));
  deadlocked = (int *)malloc(m*sizeof(int));

  //Get input for resource request matrix.
  printf("Enter Resource Request Matrix\n");
  for(i=0;i<n;i++)
    for(j=0;j<m;j++)
      scanf("%d",&req[i][j]);

  //Get input for resource allocation matrix.
  printf("Enter Resource Allocation Matrix\n");
  for(i=0;i<n;i++)
    for(j=0;j<m;j++)
      scanf("%d",&cur_alloc[i][j]);

  //Get input for unallocated resource vector.
  printf("Enter Unallocated Resource Vector\n");
  for(j=0;j<m;j++)
    scanf("%d",&unalloc[j]);

  //Initials max resource matrix
  for(i=0;i<n;i++)
    for(j=0;j<m;j++)
      max[i][j]=req[i][j]+cur_alloc[i][j];
  printf("\n");
}

//Function to detect a deadlock in the system
int dl_detect(){
  int finish[n],completed[n],flag=1;;
  int count,total,i,j,k;
  int l=0;

  //Initiliaze finish(An array contains 0's and 1's. 0 meaning a process didnt  finish and 1 meaning process completed)
  for(i=0;i<n;i++)
    finish[i]=0;
  
  while(flag){
    flag=0;
    for(i=0;i<n;i++){
      count=0;
      for(j=0;j<m;j++){
	if((finish[i]==0)&&(req[i][j]<=unalloc[j])){
	  count++;
	  if(count==m){
	    for(k=0;k<m;k++){
	      unalloc[k]+=cur_alloc[i][j];
	      finish[i]=1;
	      flag=1;
	    }
	    if(finish[i]==1){
	      completed[l]=i; //logg Completed process in array completed
	      l++;
	      i=n;
	    }
	  }
	}
      }
    }
  }
  //Logg Deadlocked process in array deadlocked
  j=0;
  flag=0;
  for(i=0;i<n;i++){
    if(finish[i]==0){
      deadlocked[j]=i;
      j++;
      flag=1;
    }
  }
  
  //Output of deadlock detection function
  if(flag==1){
    printf("Deadlock Detected. Deadlock process are: ");
    for(i=0;i<n;i++){
      printf("%d ",deadlocked[i]);
    }
    printf("\n");
  }
  else{
    //Removing terminated processes from list of completed processes
    for(i=0;i<n;i++){
      total=0;
      for(j=0;j<m;j++){
	total+=max[i][j];
      }
      if(total==0){
	n--;
	for(k=i;k<m;k++){
	  completed[k]=completed[k+1];
	}
      }
    }
    printf("No Deadlock Occured. Completed in order: ");
    for(i=0;i<n;i++)
      printf("%d ",completed[i]);
  }
  return flag;
}

//Function to correct deadlock's found in the system
void dl_correct(){
  int count,i,j,p;
  int k=0;
  int total=0;

  //Find process largest total allocated resource. If more than one process with equal largest allocated resource first process found choosen.
  for(i=0;i<n;i++){
    count=0;
    if(i==deadlocked[k]){
      for(j=0;j<m;j++){
	count+=max[i][j];
      }
      k++;
    }
    if(count>total){
      total=count;
      p=i;
    }
  }
  printf("Process Terminated: %d\n", p);

  //Termination of process with largest total allocated resource.
  for(i=0;i<m+1;i++){
    unalloc[i]+=cur_alloc[p][i];
    req[p][i]=0;
    cur_alloc[p][i]=0;
    max[p][i]=0;
  }
}
