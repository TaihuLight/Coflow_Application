#include "myunp.h"

/*Adjust Parameter for Generating Coflow Traffic*/
unsigned length_lamda = 2;
unsigned int MAXLEN = 9600000; //the order of magnitude with a threshold value:MAXLEN*1.5 = kernel threashold

unsigned int coflowid_lamda = 5;
unsigned int coflowid_range = 50;
/*end*/

unsigned int MAXTCPLEN = 7200;

int id = 0;

char DESTIP[16];

typedef struct
{
unsigned char Code;
unsigned char Len;
unsigned char Taskid;
unsigned long Reserved;
}TASKID; 

//----- Function prototypes -------------------------------------------------
double expon(double x);       // Returns an exponential random variable
double rand_val(int seed);    // Jain's RNG
double expon_random(int seed, double lam);


double expon_random(int seed, double lam)
{


  double lambda;              // Mean rate
  double exp_rv;              // Exponential random variable
  int 	 random_number_seed;

  random_number_seed = seed;
//  rand_val(random_number_seed);
	
  lambda = lam;

  // Generate and output exponential random variables

  exp_rv = expon(1.0 / lambda);
  return exp_rv;

}

//===========================================================================
//=  Function to generate exponentially distributed random variables        =
//=    - Input:  Mean value of distribution                                 =
//=    - Output: Returns with exponentially distributed random variable     =
//===========================================================================
double expon(double x)
{
  double z;                     // Uniform random number (0 < z < 1)
  double exp_value;             // Computed exponential value to be returned

  // Pull a uniform random number (0 < z < 1)
  do
  {
    z = rand_val(0);
  }
  while ((z == 0) || (z == 1));

  // Compute exponential random variable using inversion method
  exp_value = -x * log(z);

  return(exp_value);
}

//=========================================================================
//= Multiplicative LCG for generating uniform(0.0, 1.0) random numbers    =
//=   - x_n = 7^5*x_(n-1)mod(2^31 - 1)                                    =
//=   - With x seeded to 1 the 10000th x value should be 1043618065       =
//=   - From R. Jain, "The Art of Computer Systems Performance Analysis," =
//=     John Wiley & Sons, 1991. (Page 443, Figure 26.2)                  =
//=========================================================================
double rand_val(int seed)
{
  const long  a =      16807;  // Multiplier
  const long  m = 2147483647;  // Modulus
  const long  q =     127773;  // m div a
  const long  r =       2836;  // m mod a
  static long x;               // Random int value
  long        x_div_q;         // x divided by q
  long        x_mod_q;         // x modulo q
  long        x_new;           // New x value

  // Set the seed if argument is non-zero and then return zero
  if (seed > 0)
  {
    x = seed;
    return(0.0);
  }

  // RNG using integer arithmetic
  x_div_q = x / q;
  x_mod_q = x % q;
  x_new = (a * x_mod_q) - (r * x_div_q);
  if (x_new > 0)
    x = x_new;
  else
    x = x_new + m;

  // Return a random value between 0.0 and 1.0
  return((double) x / m);
}



int my_write(int fd,void *buffer,int length) {

	int bytes_left; 
	int written_bytes; 
	char *ptr; 
	ptr=buffer; 

	bytes_left=length; 
	while(bytes_left>0) 

	{    written_bytes=write(fd,ptr,bytes_left); 
         if(written_bytes<=0) 
         {        
         	if(errno==EINTR) 
            	written_bytes=0; 

            else 
				return(-1); 
         } 

         bytes_left-=written_bytes; 
         ptr+=written_bytes;  
	} 

	return(0); 

} 


void str_cli2(int sockfd){
	char sendline[MAXTCPLEN];	
	long length;

	memset(sendline, 'a', MAXTCPLEN);
	
	length = expon_random(1,length_lamda)* MAXLEN;
	
	while(length > MAXTCPLEN){
		my_write(sockfd, sendline, MAXTCPLEN);
		length = length - MAXTCPLEN;
	}
		
	my_write(sockfd, sendline, length);
	close(sockfd);		
}


void handle(){

	int sockfd;
	struct sockaddr_in	servaddr;
	TASKID option;
	int n,m;
	unsigned int coflowid;
	int df_value = IP_PMTUDISC_DONT;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, DESTIP, &servaddr.sin_addr);

	srand (time(NULL));
	coflowid = expon_random(1,coflowid_lamda)*coflowid_range + 1;

//	printf("coflowid: %d\n", coflowid);

	bzero(&option, sizeof(TASKID)); 
	option.Code = 154;
	option.Len = 5;
	option.Taskid = coflowid;

	n = setsockopt(sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &df_value, sizeof(df_value));
//  printf("n:%d\n", n);
	
	n = setsockopt(sockfd,IPPROTO_IP,IP_OPTIONS,(char *)&option, option.Len);	
//	printf("n:%d\n", n);

	
	m = connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
//	printf("connect() m:%d\n", m);

	str_cli2(sockfd);
	
}



int
main(int argc, char **argv)
{

	struct sockaddr_in	servaddr;
	pthread_t thread[1000];
	int ret_thrd;
	int tmp;
	void *retval;
	int i = 0;
	int threadNum = 400;

	if (argc != 2)
		err_sys("usage: tcpcli <IPaddress>");

	strcpy(DESTIP, argv[1]);

	rand_val(1);

	while(i < threadNum) {

		//Create Threads
		ret_thrd = pthread_create(&thread[i], NULL, (void *)&handle, NULL);
		
		if(i == (threadNum - 1)){
    			if (ret_thrd != 0) {
       	 			printf("Create Thread%d Failed\n", i);
    			} else {
        			printf("Create Thread%d\n", i);
			}
		}
		i++;

	}


	i = 0;
	while(i < threadNum){
	//Get the return value of thread1
   		tmp = pthread_join(thread[i], &retval);
		
		if(i == (threadNum - 1)){
    			if (tmp != 0) {
        			printf("cannot join with thread%d\n", i);
   			} else {
    				printf("thread%d end\n",i);
			}
		}
		i++;
	}

	return 0;
}


