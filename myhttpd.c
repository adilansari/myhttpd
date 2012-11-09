
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include<malloc.h>
#include <fcntl.h>        
#include <sys/stat.h> 
#include<time.h>
#include<semaphore.h>
#define BUF_SIZE 1024

#define OK_IMAGE    "HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define OK_TEXT     "HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define NOTOK_404   "HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404    "<html><body><h1>FILE NOT FOUND</h1></body></html>"

sem_t sem;
int free_thread;
int sched_flag=0,debug_flag=0,log_flag=0;
char * file=NULL;
pthread_t t_serve;
pthread_mutex_t qmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sthread_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
struct request
{
	int acceptfd;
	int size;
	char file_name[1024];
	unsigned int cli_ipaddr;
	char time_arrival[1024];
	char in_buf[2048];
	
}r2; 
// queue function declarations;
void insertion(int,char*, int, unsigned int,char*,char*);	
//void insertion(int,string, int);
struct request extract_element();
struct request removesjf();
void display();
void print_help_options();





//queue structre
struct node
{
	struct request r;	
	struct node *link;
}*new,*temp,*p,*front=NULL,*rear=NULL;
typedef struct node N;

void display()
{
	if(front==NULL)
		printf("\nempty queue");
	else
	{
		int a;
		
		temp=front;
		while(temp!=NULL)
		{
			a=(temp->r.acceptfd);			
			printf("\n acceptfd is %d, file name is %s, file size is %d , ip addr is %u, request is %s,time is %s",a,temp->r.file_name,temp->r.size,temp->r.cli_ipaddr,temp->r.in_buf,temp->r.time_arrival);
			temp=temp->link;
		}
	}
} 

// queue functions
void insertion(int afd,char *f,int size,unsigned int ip, char * time_arrival,char * in_buf)
{

	
	new=(N*)malloc(sizeof(N));
	int n;
	char a[1024];
	char b[1024];
	char c[1024];
	strcpy(a,f);
	strcpy(b,time_arrival);
	strcpy(c,in_buf);
	new->r.acceptfd=afd;
	strcpy(new->r.file_name,a);
	new->r.cli_ipaddr=ip;
	strcpy(new->r.time_arrival,b);
	strcpy(new->r.in_buf,c);
	
	//new->r.file_name=a;
	new->r.size=size;
	new->link=NULL;
	if(front==NULL)
		front=new;
	else
		rear->link=new;
		rear=new;
	//printf("\n inserted request into queue");
	display();	
		
}

 struct request extract_element()
{

	if(front==NULL)
		printf("\nempty queue");
	else
	{	
		struct request r1;
		p=front;
		//printf("\n element is : %d",p->r.acceptfd);
		front=front->link;
		r1.acceptfd=p->r.acceptfd;
		strcpy(r1.file_name,p->r.file_name);		
		r1.size=p->r.size;
		free(p);
		return(r1);
	}
}
 


struct request removesjf(int num)
{ 
	printf("\nentered removesjf");
	if(front==NULL)
	{
		 printf("\n\nempty list");
	}
	else
	{
		struct node *old,*temp;
		temp=front;
		while(temp!=NULL)
		{ 
			if(temp->r.acceptfd==num)
			{ 
				if(temp==front) 
				front=temp->link; 
				else 
				old->link=temp->link;
				return(temp->r);
				free(temp);
				
			}
			else
			{ 
				old=temp;
				temp=temp->link;
			}
		}
	}
}

// end of queue functions


// thread pool function

void *thread_serve()
{
	while(1)
	{		
		
		//printf("\nentered serving thread\n");		
		pthread_mutex_lock(&sthread_mutex);		
		pthread_cond_wait(&cond_var,&sthread_mutex);
		
		//printf("\ngot signal\n");
		//wait on condition mutex

		struct request r=r2;
		pthread_mutex_unlock(&sthread_mutex);
		//printf("\n serving thread unlocked sthread_mutex");
		
		//printf("\n is acceptfd %d, file name is %s, file size is %d , ip addr is %u, request is %s,time is %s",a,temp->r.file_name,temp->r.size,temp->r.cli_ipaddr,temp->r.in_buf,temp->r.time_arrival);



		time_t now;					// getting the time the job has been assigned to the serving thread
                time(&now);
       	        struct tm * ct=localtime(&now); //getting localtime
       	        int ch[128], time_serve[128];
       	        struct timeval tv;
       	        strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
       	        snprintf(time_serve, sizeof time_serve, ch, tv.tv_usec); //printing the needed timestamp string
		


		unsigned int ip=r.cli_ipaddr;
  
    /* code adapted from stackoverflow.com */
    
		unsigned char bytes[4];
		bytes[0] = ip & 0xFF;
		bytes[1] = (ip >> 8) & 0xFF;
		bytes[2] = (ip >> 16) & 0xFF;
		bytes[3] = (ip >> 24) & 0xFF;
 /* end of code adapted from stackoverflow.com */
		
		//struct request r= *((struct request *)arg);
		if(debug_flag==0&& log_flag==1)
		{
			
			FILE * file_des=fopen(file,"a"); 
			//printf("\n in  serving thread \n");
		
			fprintf(file_des,"%d.%d.%d.%d\t-\t ", bytes[0], bytes[1], bytes[2], bytes[3]);
			fprintf(file_des,"%s\t %s\t %s \t status\t %d\n",r.time_arrival,time_serve,r.in_buf,r.size);
	
			fclose(file_des);
		}
              
		else if(debug_flag==1)
		{  
			printf("\n%d.%d.%d.%d\t-\t %s\t %s\t %s \t status\t %d\n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,r.size);
			//fprintf(stdout,"%s\t %s\t %s \t status\t %d\n",r.time_arrival,time_serve,r.in_buf,r.size);
	
			




		}

	
		//printf("\n in  serving thread copied structure\n");

		char           in_buf[BUF_SIZE];      
		char           out_buf[BUF_SIZE];
		char           *file_name;                 
		file_name=malloc(sizeof(char *));
		int acceptfd;
		unsigned int   fd1;                         
		unsigned int   buffer_length;                    
		unsigned int   retcode;  
		int m; 

		//printf("\n in  serving thread before copying variables\n");
		acceptfd=r.acceptfd;
		file_name=r.file_name;
		//printf("\n in  serving thread after copying variables\n");
                
	 	//printf("\nin serving thread file name is %s\n",file_name);
        
      
 
	

		{	

       
		//printf("\nin serving thread opening file\n");
		
		/* This part of code adopted from http://kturley.com/simple-multi-threaded-web-server-written-in-c-using-pthreads/ */

        	fd1 = open(&file_name[1], O_RDONLY, S_IREAD | S_IWRITE);
	
 		memset(out_buf, 0, sizeof(out_buf));
   
        	
        	if (fd1 == -1)
        	{
			printf("File %s not found - sending an HTTP 404 \n", &file_name[1]);
        		strcpy(out_buf, NOTOK_404);
        		send(acceptfd, out_buf, strlen(out_buf), 0);
		        strcpy(out_buf, MESS_404);
			send(acceptfd, out_buf, strlen(out_buf), 0);
		}
	        else
	        {
		        printf("File %s is being sent \n", &file_name[1]);
		        if ((strstr(file_name, ".jpg") != NULL)||(strstr(file_name, ".gif") != NULL)) 
		         { strcpy(out_buf, OK_IMAGE); }
 
			else
		          { strcpy(out_buf, OK_TEXT); }
		          send(acceptfd, out_buf, strlen(out_buf), 0);
 
		          buffer_length = 1;  
		          while (buffer_length > 0)  
		          {
			            buffer_length = read(fd1, out_buf, BUF_SIZE);
			            if (buffer_length > 0)   
	            { 
              send(acceptfd, out_buf, buffer_length, 0);     
                 
/* end of code adapted from http://kturley.com/simple-multi-threaded-web-server-written-in-c-using-pthreads/ */
		
	//printf("\nin serving thread after sending file\n");
		

		//pthread_mutex_lock(&sthread_mutex);
		
		sem_post(&sem);
		//printf("\n got mutex ,incremented number of free threads: %d\n",free_thread);
		//pthread_mutex_unlock(&sthread_mutex);
		//printf("\nafter semaphore post\n");
		
            }
          }
}
}




}


}


//scheduler thread
void *thread_scheduler(void *arg)
{
	unsigned int schedalg=*((unsigned int*)arg);
	int acceptfd,n;
	if(schedalg==0)
	{	
		while(1)
		{	
						
			
			if(front!=NULL)
			{	
				
				sem_wait(&sem);		
				//printf("\nin sched thread before extracting element\n");
				//printf("\nscheduler locking mutex\n");		
				pthread_mutex_lock(&sthread_mutex);
				pthread_mutex_lock(&qmutex);
				r2=extract_element();
				//printf("\n popped element in scheduler thread");
				pthread_mutex_unlock(&qmutex);
				
				//printf("\nscheduler unlocked mutex\n");
				// call serving thread from thread pool
				
				//printf("\nin sched thread before sending to serving thread\n");
				
				pthread_cond_signal(&cond_var);
				
				
				free_thread--;
				pthread_mutex_unlock(&sthread_mutex);				
				//printf("\nin sched thread unlocked sthread mutex\n");
				
				//thread_serve(&r2);
				//pthread_create(&t_serve,NULL,&thread_serve,&r);	
				//thread_serve(&r);
						
			}

			else 
			{
				
							
				continue;
			}			
		}
	}
	else
	{
	//code for SJF scheduling algorithm
		//printf("\n entered SJF scheduling algorithm");
		int shortestjob_fd=0;
		int min;
		int a,b;
		while(1)			
		{	pthread_mutex_lock(&qmutex);
			temp=front;
			if (temp==NULL)
			{
				
				continue;
			}
			else if(temp->link==NULL) 
			{
				//printf("\n only one");
				shortestjob_fd=temp->r.acceptfd;
			}
			else
			
			{
				min=temp->r.size;
				while(temp->link!=NULL)  //should modify
				{
					
					b=temp->link->r.size;
					if(min<=b)
					{
						shortestjob_fd=temp->r.acceptfd;
					}		
					else if(min>b)
					{			
						min=temp->link->r.size;						
						shortestjob_fd=temp->link->r.acceptfd;
					}
					printf("\n %d",a);
					temp=temp->link;
				}
			}
			pthread_mutex_lock(&sthread_mutex);
			
			r2=removesjf(shortestjob_fd);
			//printf("extracted element");
			pthread_cond_signal(&cond_var);
			pthread_mutex_unlock(&sthread_mutex);
						
			pthread_mutex_unlock(&qmutex);
		}
	
	}
	
}





// Listening and queueing thread

void *thread_listen(void *arg)
{
	unsigned int sockfd=*((unsigned int*)arg);
	int i,size;	
	unsigned int acceptfd,ids2;
	socklen_t clilen;
	int newsockfd[10],c;
	int n;
	char buffer[256];
	pthread_t t_serve[10];
	struct sockaddr_in cli_addr;	
	clilen = sizeof(cli_addr);
	unsigned int retval;
	char request_buffer[1024];



	int retcode;
	off_t file_size;
	char in_buf[BUF_SIZE];
	
	char *fname=malloc(sizeof(char *));	
	struct stat st; 
	int k,j;
	
	int l;
	


	

	listen(sockfd,5);								// listens
	//printf("\nin listening thread before listen\n");

	while(1)     
	
	{	
		
		//printf("\nin listening thread before accept\n");
		acceptfd= accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
		if (acceptfd < 0) 
          	perror("error in accepting");

    unsigned int ip=cli_addr.sin_addr.s_addr;
  
        


		time_t now;
                time(&now);
                struct tm * ct=localtime(&now); //getting localtime
                int ch[128], time_arrival[128];
                struct timeval tv;
                strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
                snprintf(time_arrival, sizeof time_arrival, ch, tv.tv_usec); //printing the needed timestamp string
               
	

		


	/*	FILE * file_des=fopen(file,"a"); 
	printf("\n in  serving thread \n");
	
	
	fprintf(file_des,"%s\n",time_arrival);
	
	fclose(file_des);
		
	*/	



		char *file_name=malloc(sizeof(char *));

		 memset(in_buf, 0, sizeof(in_buf));
		retcode = recv(acceptfd, in_buf, BUF_SIZE, 0);			
            
		//printf("\nin listening thread before getting file name\n");
		
		if (retcode < 0)
		{ 
			printf("recv error detected ...\n"); 
		}
     
		
    		else
		{    
										
			strtok(in_buf, " ");
		        file_name = strtok(NULL, " ");			
		}

		if(file_name!=NULL)
		{


			//ids2=acceptfd;
			//pthread_create(&t_serve[i],NULL,&thread_serve,&ids2);	
			
			//off_t fsize(const char *filename) 
			

				
			
			k=1,j=0; 
			while(k<strlen(file_name))
			{
				fname[j]=file_name[k];
				k++;
				j++;			
			}		
			    

			    if (stat(fname, &st) == 0)
			    file_size=st.st_size;

			//printf(" size of file %s is %zd",file_name,file_size);

			
			//printf("\nin listening thread after accepting and before inserting into queue\n");
			//pthread_mutex_lock(&qmutex);
			insertion(acceptfd,file_name,file_size,ip,time_arrival,in_buf);
			//printf("\n acceptfd is %d, file name is %s, file name is %zd",acceptfd,file_name,file_size);
			//printf("inserted ino queue");
			//pthread_mutex_unlock(&qmutex);
			//printf("newsockfd in thread is : %d",newsockfd);
		}
		else
		{
			continue;
		}		


	}				



	
   
}


int main(int argc, char *args[])
{
	int thread_status[10];     	
	pthread_t t_listener,t_scheduler,t_serve[10];
	int sockfd,ids;
	char *dir;
	file=malloc(sizeof(char *));
	dir=malloc(sizeof(char *));
	
	

	int portnum=8080,threadnum=4,sleep_time=60;
	int i;
	int help_flag=0,dir_flag=0,time_flag,threadnum_flag=0;
	
	// Parser code
	for(i=0;i<argc;i++)
	{
		//printf("\ncomparing%d",i);
		
		if(strcmp(args[i],"-h")==0)
		{
			help_flag=1;
			
		}
		else if(strcmp(args[i],"-n")==0)
		{
			threadnum=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-d")==0)
		{
			debug_flag=1;
			threadnum=1;
			
		}
		else if(strcmp(args[i],"-l")==0)
		{
			log_flag=1;
			file=args[i+1];
		}
		else if(strcmp(args[i],"-p")==0)
		{
			portnum=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-r")==0)
		{
			dir_flag=1;		
			dir=args[i+1];
		}
		else if(strcmp(args[i],"-t")==0)
		{
			time_flag=1;		
		 	sleep_time=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-s")==0)
		{
			if (strcmp(args[i+1],"FCFS")==0)		
				sched_flag=0;
			else if(args[i+1],"SJF")
				sched_flag=1;
			else
				printf("Please enter a proper scheduling algorithm");
		}
	
	}
	
	//free_thread=threadnum;
	sem_init(&sem,0,threadnum);

//printf( "\n debug : %d, help: %d, log: %d, file name : %s port num : %d, dir : %d dir name: %s, time :%d ,thread num : %d, sched : %d",debug_flag,help_flag,log_flag,file,portnum,dir_flag,dir,sleep_time,threadnum,sched_flag);	
	//Parser code ends

	if(help_flag==1)			// printing help options and exit if -h option is specified
	{
		print_help_options();
		exit(1);
	}
	else if(dir_flag==1)			//changing directory if -d option is specified
	{
		if(chdir(dir)<0)
			{
				perror("\ndirectory doesnt exist");
				exit(1);
			}
	}
	


	
	struct sockaddr_in serv_addr;
	//printf("before socket creation");
	sockfd = socket(AF_INET, SOCK_STREAM,0);			//creation of socket	
	//printf("\n after socket creation socket id is %d", sockfd);
	if (sockfd < 0) 
        perror("error creating socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port =htons(portnum);
	//printf("before bind");
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)		//binding socket
	perror("binding error");
	//printf("\nafter bind");
	



     	int w;
	for(w=0;w<threadnum;w++)
	{
		pthread_create(&t_serve[w],NULL,&thread_serve,NULL); 
     	} 
     
	
	
     
    
   	 
	
     
     
	ids=sockfd;
	//printf("\n before creating scheduler thread");
	//printf("\nbefore creating thread sockfd is %d",ids);
	pthread_create(&t_listener,NULL,&thread_listen,&ids);			//creating listener thread
	sleep(sleep_time);							// putting scheduler to sleep
	pthread_create(&t_scheduler,NULL,&thread_scheduler,&sched_flag);	//creating scheduler thread
        pthread_join(t_listener,NULL);
	pthread_join(t_scheduler,NULL);
	//printf("\nafter join in main");
	display();
	close(sockfd);
	return 0;
}


void print_help_options()
{
printf("\n−d : Enter debugging mode. That is, do not daemonize, only accept one connection at a \ntime and enable logging to stdout. Without this option, the web server should run as a daemon process in the background. \n−h : Print a usage summary with all options and exit. \n−l file : Log all requests to the given file. See LOGGING for details.\n−p port : Listen on the given port. If not provided, myhttpd will listen on port 8080. \n−r dir : Set the root directory for the http server to dir. \n−t time : Set the queuing time to time seconds. The default should be 60 seconds. \n−n threadnum: Set number of threads waiting ready in the execution thread pool to threadnum. \nThe default should be 4 execution threads. \n−s sched: Set the scheduling policy. It can be either FCFS or SJF. The default will be FCFS.");
}












	
