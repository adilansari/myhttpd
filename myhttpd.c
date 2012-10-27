/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
//HTTP response messages
#define OK_IMAGE    "HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define OK_TEXT     "HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define NOTOK_404   "HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404    "<html><body><h1>FILE NOT FOUND</h1></body></html>"
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
#define BUF_SIZE 1024

int sched_flag=0;
char * file=NULL;
pthread_t t_serve;
pthread_mutex_t qmutex = PTHREAD_MUTEX_INITIALIZER;
struct request
{
	int acceptfd;
	int size;
	char file_name[200];
}; 
// queue function declarations;

void insertion(int,char*, int);	
//void insertion(int,string, int);
struct request extract_element();
void removesjf();
void display();
void print_help_options();


void error(const char *msg)
{
    perror(msg);
    exit(1);
}


//queue structre
struct node
{
	struct request r;	
	//int acceptfd;
	//int size;
	//char *file_name;
	struct node *link;
	}*new,*temp,*p,*front=NULL,*rear=NULL;
typedef struct node N;

void display()
{
	if(front==NULL)
		printf("\nQueue is empty");
	else
	{
		int a;
		printf("\nThe elements are : ");
		temp=front;
		while(temp!=NULL)
		{
			a=(temp->r.acceptfd);			
			printf("\n acceptfd is %d, file name is %s, address of file name is %d file size is %d",a,temp->r.file_name,*(temp->r.file_name),temp->r.size);
			temp=temp->link;
		}
	}
} 

// queue functions
void insertion(int afd,char *f,int size)
{

	
	new=(N*)malloc(sizeof(N));
	int n;
	char a[200];
	strcpy(a,f);
	new->r.acceptfd=afd;
	//new->r.file_name=malloc(sizeof(char *));
	//strcpy(new->r.file_name,f);
	strcpy(new->r.file_name,a);
	//new->r.file_name=a;
	new->r.size=size;
	new->link=NULL;
	if(front==NULL)
		front=new;
	else
		rear->link=new;
		rear=new;
	printf("\n inserted request into queue");
	display();	
		
}

 struct request extract_element()
{

	if(front==NULL)
		printf("\nQueue is empty");
	else
	{	
		struct request r1;
		p=front;
		printf("\nExtracted element is : %d",p->r.acceptfd);
		front=front->link;
		r1.acceptfd=p->r.acceptfd;
		strcpy(r1.file_name,p->r.file_name);		
		r1.size=p->r.size;
		free(p);
		return(r1);
	}
}
 


void removesjf(int num)
{ 
	if(front==NULL)
	{
		 printf("\n\nEmpty Linked List.Cant Delete The Data.");
	}
	else
	{
		struct node *old,*temp;
		temp=front;
		while(temp!=NULL)
		{ 
			if(temp->r.acceptfd==num)
			{ 
				if(temp==front) /* First Node case */
				front=temp->link; /* shifted the header node */
				else 
				old->link=temp->link;
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



// thread pool function

void *thread_serve(void *arg)
{
  printf("\nentered serving thread\n");
struct request r= *((struct request *)arg);
printf("\n in  serving thread copied structure\n");

  char           in_buf[BUF_SIZE];      
  char           out_buf[BUF_SIZE];
  char           *file_name;                 // File name
  file_name=malloc(sizeof(char *));
  int acceptfd;
  unsigned int   fh;                         // File handle (file descriptor)
  unsigned int   buf_len;                    // Buffer length for file reads
  unsigned int   retcode;  
  int m; 

printf("\n in  serving thread before copying variables\n");
  acceptfd=r.acceptfd;
  file_name=r.file_name;
printf("\n in  serving thread after copying variables\n");
                // Return code  
/* retcode = recv(acceptfd, in_buf, BUF_SIZE, 0);
            // Input buffer for GET resquest
            // Output buffer for HTML response

 printf("\nin serving thread before getting file name\n");
      /* if receive error --- */
   /*   if (retcode < 0)
	{ 
		  printf("recv error detected ...\n"); 
	}
     
      /* if HTTP command successfully received --- */
/*      else
      {    
        /* Parse out the filename from the GET request --- */
/*        strtok(in_buf, " ");
        file_name = strtok(NULL, " ");	*/
	printf("\nin serving thread before opening file\n");	//logging to file
	FILE * file_des=fopen(file,"a"); 
	printf("\n in  serving thread \n");
	fprintf(file_des,"%s\n",&file_name[1]); 
	
	fclose(file_des);
	 printf("\nin serving thread finished logging\n"); 
 	printf("\nin serving thread file name is %s\n",file_name);
        
      
 
	
//	if(file_name!=NULL)

	{	

       
	printf("\nin serving thread opening file\n");
	 /* Open the requested file (start at 2nd char to get rid of leading "\") */
        fh = open(&file_name[1], O_RDONLY, S_IREAD | S_IWRITE);
	
 	memset(out_buf, 0, sizeof(out_buf));
   
        /* Generate and send the response (404 if could not open the file) */
        if (fh == -1)
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
 
          buf_len = 1;  
          while (buf_len > 0)  
          {
            buf_len = read(fh, out_buf, BUF_SIZE);
            if (buf_len > 0)   
            { 
              send(acceptfd, out_buf, buf_len, 0);     
               //printf("%d bytes transferred ..\n", buf_len);  
		printf("\nin serving thread after sending file\n");
            }
          }
}




/* int n;
printf("hi i am thread \n");
n = write(acceptfd,"\n message recieved from serving thread of thread pool",51);
     	if (n < 0) error("ERROR writing to socket");
     	close(n);
//pthread_join(t_listener,NULL);
*/
}


}

// end of queue functions
//scheduler thread
void *thread_scheduler(void *arg)
{
	unsigned int schedalg=*((unsigned int*)arg);
	int acceptfd,n;
	struct request r2;
	if(schedalg==0)
	{	
		while(1)
		{	
						
			
			if(front!=NULL)
			{	
				
				printf("\nin sched thread before extracting element\n");
				printf("\nscheduler locking mutex\n");		
				pthread_mutex_lock(&qmutex);
				r2=extract_element();
				pthread_mutex_unlock(&qmutex);
				// sleep function
				printf("\nscheduler unlocking mutex\n");
				// call serving thread from thread pool
				/* n = write(acceptfd,"I got your message from scheduler",33);
     				if (n < 0) error("ERROR writing to socket");
     				close(n); */
				printf("\nin sched thread before sending to serving thread\n");
				thread_serve(&r2);
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
		int shortestjob_fd=0;
		int min;
		int a,b;
		while(1)			
		{	pthread_mutex_lock(&qmutex);
			temp=front;
			if (temp==NULL)
				continue;
			else if(temp->link==NULL) 
				shortestjob_fd=temp->r.acceptfd;
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
		
			removesjf(shortestjob_fd);
			// sleep code
			//code fro calling serving threads from thread pool
			n = write(acceptfd,"I got your message from scheduler",33);
			if (n < 0) error("ERROR writing to socket");
			close(n);			
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
	//char fln[200];
	//char *file_name;
	char *fname=malloc(sizeof(char *));	
	struct stat st; 
	int k,j;
	
	int l;
	


	
/*	struct sockaddr_in cli_addr,serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	
	if (sockfd < 0) 
        error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	error("ERROR on binding");   */
	listen(sockfd,5);
	printf("\nin listening thread before listen\n");
// crap starts
	while(1)     
	//for(i=0;i<5;i++)	//replace for loop with infinte while loop
	{	
		
		printf("\nin listening thread before accept\n");
		acceptfd= accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
		if (acceptfd < 0) 
          	printf("ERROR on accept");
		
		
		char *file_name=malloc(sizeof(char *));

		 memset(in_buf, 0, sizeof(in_buf));
		retcode = recv(acceptfd, in_buf, BUF_SIZE, 0);
            // Input buffer for GET resquest
            // Output buffer for HTML response
		printf("\nin listening thread before getting file name\n");
		/* if receive error --- */
		if (retcode < 0)
		{ 
			printf("recv error detected ...\n"); 
		}
     
		/* if HTTP command successfully received --- */
    		else
		{    
		/* Parse out the filename from the GET request --- */
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

			printf(" size of file %s is %zd",file_name,file_size);

			//strcpy(fln,file_name);

			//find the size of the requested file


			//retval = recv(acceptfd, request_buffer, BUFSIZE, 0);						
			//printf("return value is %d",retval);
			printf("\nin listening thread after accepting and before inserting into queue\n");
			//pthread_mutex_lock(&qmutex);
			insertion(acceptfd,file_name,file_size);
			//printf("\n acceptfd is %d, file name is %s, file name is %zd",acceptfd,file_name,file_size);
			printf("inserted ino queue");
			//pthread_mutex_unlock(&qmutex);
			//printf("newsockfd in thread is : %d",newsockfd);
		}
		else
		{
			continue;
		}		


	}				



	
    /* bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd); */  
//pthread_joisn(t_serve[0],NULL);
}


int main(int argc, char *args[])
{
	int thread_status[10];     	
	pthread_t t_listener,t_scheduler,t_serve[10];
	int sockfd,ids;
	char *dir;
	file=malloc(sizeof(char *));
	dir=malloc(sizeof(char *));
	
	

	int portnum=8080,threadnum,sleep_time=60;
	int i;
	int debug_flag=0,help_flag=0,log_flag=0,dir_flag=0,time_flag,threadnum_flag=0;
	
	// Parser code
	for(i=0;i<argc;i++)
	{
		printf("\ncomparing%d",i);
		if(strcmp(args[i],"-d")==0)
		{
			debug_flag=1;
		}
		else if(strcmp(args[i],"-h")==0)
		{
			help_flag=1;
		
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
		else if(strcmp(args[i],"-l")==0)
		{
			threadnum_flag=1;		
			threadnum=atoi(args[i+1]);
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

printf( "\n debug : %d, help: %d, log: %d, file name : %s port num : %d, dir : %d dir name: %s, time :%d ,thread num : %d, sched : %d",debug_flag,help_flag,log_flag,file,portnum,dir_flag,dir,sleep_time,threadnum,sched_flag);	
	//Parser code ends

	if(help_flag==1)
	{
		print_help_options();
		exit(1);
	}
	else if(dir_flag==1)
	{
		if(chdir(dir)<0)
			perror("\ndirectory doesnt exist");
			exit(1);
	}
	


	
	struct sockaddr_in serv_addr;
	printf("before socket creation");
	sockfd = socket(AF_INET, SOCK_STREAM,0);	
	printf("\n after socket creation socket id is %d", sockfd);
	if (sockfd < 0) 
        error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port =htons(portnum);
	printf("before bind");
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	error("ERROR on binding");
	printf("\nafter bind");
	


//int portno;
     /*	int i;
	for(i=0;i<10;i++)
	{
		pthread_create(&t_serve[i],NULL,&thread_serve,sockfd); 
     	} */
     
	
	
     
    
   	 // portno = atoi(args[1]);
	
     
     
	ids=sockfd;
	printf("\n before creating scheduler thread");
	printf("\nbefore creating thread sockfd is %d",ids);
	pthread_create(&t_listener,NULL,&thread_listen,&ids);
	sleep(sleep_time);		// putting scheduler to sleep
	pthread_create(&t_scheduler,NULL,&thread_scheduler,&sched_flag);
        pthread_join(t_listener,NULL);
	pthread_join(t_scheduler,NULL);
	printf("\nafter join in main");
	display();
	close(sockfd);
	return 0;
}


void print_help_options()
{
printf("\n−d : Enter debugging mode. That is, do not daemonize, only accept one connection at a \ntime and enable logging to stdout. Without this option, the web server should run as a daemon process in the background. \n−h : Print a usage summary with all options and exit. \n−l file : Log all requests to the given file. See LOGGING for details.\n−p port : Listen on the given port. If not provided, myhttpd will listen on port 8080. \n−r dir : Set the root directory for the http server to dir. \n−t time : Set the queuing time to time seconds. The default should be 60 seconds. \n−n threadnum: Set number of threads waiting ready in the execution thread pool to threadnum. \nThe default should be 4 execution threads. \n−s sched: Set the scheduling policy. It can be either FCFS or SJF. The default will be FCFS.");
}



