#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdint.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>//For ETH_P_IP
#include <net/ethernet.h>//For ETH_P_IP
#include <netinet/ip.h>//For struct ip IPVERSION IP_DEFTTL
#include <netinet/udp.h>//For struct updhdr
#include <netpacket/packet.h>//For struct sockaddr_ll PACKET_BROADCAST

struct data_header
{
    unsigned int    rru_id;
    unsigned char     data_type;
    unsigned short    len;
    unsigned char     chan;
    unsigned short    frame_num;
    unsigned short    seq_no;
    unsigned char     frg_flag;
    unsigned char     dgain;
    unsigned char     freq;
    unsigned char     res;
}__attribute__ ((packed));
#define DATA_HEADER_LEN sizeof(struct data_header)


struct req_msg
{
    unsigned short    crc_err_cnt;
    unsigned short    pkg_lost_cnt;
    unsigned short    pkg_later_cnt;
    unsigned int    time_sof;
    unsigned int    time_eof;
}__attribute__ ((packed));

#define PKG_LEN			1088
#define DL_PKG_NUM		(4 * 10)
#define UL_PKG_NUM		(4 * 15)

#define DL_MAX_SYMBOL		20
#define UL_MAX_SYMBOL		20
#define MAX_ANT_NUM		2
#define MAX_TX_BUF_LEN		2000
#define MAX_RX_BUF_LEN		2000
#define MAX_DATA_BUF_LEN	(4 * PKG_LEN * DL_MAX_SYMBOL * MAX_ANT_NUM)

void usage(const char *progname);
void data_print(struct data_header * p_header);
void payload_print(unsigned char* buf, unsigned short len);
void feedback_info_print(struct req_msg* p_req_msg);
int strval(const char *str);

/* 	å‡½æ•°å£°æ˜	*/
int connect_to_rru(void);
int send_dl_packet(int sock, unsigned char* p_data_buf, unsigned int dl_symbol_num, unsigned short frame_num, unsigned char ant_num);
int data_link_creat(void);

/* 	å…¨å±€å˜é‡	*/
unsigned short    g_frame_num = 0;
unsigned int    g_rru_id = 0x11223344;
int signaled = 0;
int g_socket = -1;
int up_cap_chan =1;

long g_dl_frame_num = 0;
long g_dl_pkg_num = 0;
long g_ul_skip_num = 0;
long g_ul_frame_num = 0;
long g_ul_pkg_num = 0;
long g_ul_dump_pkg_num = 0;

//Add by AT
long g_ul_frame_loose = 0;
long g_ul_last_seq = 0;
long g_ul_last_flag = 0;
long g_ul_last_chan = 0;
long g_packet_lost_num = 0;
long g_packet_late_num = 0;
long g_packet_snd_time = 0;
long g_packet_rcv_time = 0;
long g_dl_send_delay = 0;
	

FILE* g_fp_in;
FILE* g_fp_out;

unsigned char g_rx_buf[MAX_RX_BUF_LEN];
unsigned char g_data_buf[MAX_DATA_BUF_LEN];
char g_calc_ul_pow=0;//æ˜¯å¦è®¡ç®—ä¸Šè¡ŒåŠŸç‡
double g_ul_pow_i[2] = {0};
double g_ul_pow_q[2] = {0};
/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
*/
//set the default value
const char* input_filename = "tx_server.dat";
const char* output_filename = "rx_server.dat";	

//New Style
int bbuip=0;
short bbu_udp_port = 33333;//é€šé“å»ºç«‹è¯·æ±‚ç«¯å£
short bbu_iq_port  = 33338;//IQæ•°æ®ç«¯å£
short bbu_cm_port  = 30000;//CMæ•°æ®ç«¯å£
int rruip=0;
short rru_udp_port = 33334;
short rru_iq_port = 33336;
short rru_cm_port = 30000;
//New Stype End

int ant_num = 2;
int dl_symbol_num = 10;
int ul_dump_num = 2;
int verbose = 0;

void shut_down(int signum)  
{  
	signaled = 1;
} 

/*
 *	å¿ƒè·³å¤„ç†çº¿ç¨‹
 */
void* ht_process(void* arg)
{
	int sockfd,csockfd;//TCP Socket
	size_t addr_len;
	struct sockaddr_in bbu_addr, rru_addr, addr ;
	char buf[]={
		0x00,0x00,//å‘½ä»¤ä½“é•¿åº¦
		0x00,//BBU ID
		0x00,0x00,0x00,0x00,//RRU ID
		0x00,0x01,//æµæ°´å·
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,//æ—¶é—´
		0x04,//å‘½ä»¤å­—ï¼šå¿ƒè·³åŒ…
		0xFF,//åº”ç­”æ ‡å¿—
		0x00,0x00,0x00,0x00,//æ‰©å±•å­—æ®µ
		0x27,0x6F//æ ¡éªŒå’Œ
	};
	unsigned int cnt=0;
	int ret;

	printf("ht process created....\n");

	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("Fail to create ht socket!\n");
		exit(1);
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = bbuip; // = rruip + 1
	addr.sin_port = htons(bbu_cm_port);
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret <0)
	{
		fprintf(stderr,"ht process Bind Error: %s\n", strerror(errno));
		exit(-1);
	}

	ret = listen(sockfd, 3);
	if (ret<0)
	{
		fprintf(stderr,"ht process Listen Error: %s\n", strerror(errno));
		exit(-1);
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = rruip;
	addr.sin_port = htons(rru_cm_port);

	fprintf(stdout, "Waiting to accept RRU\n");
	addr_len = sizeof(struct sockaddr_in);
	while (1) {

		csockfd = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
		if (csockfd<0) {
			fprintf(stderr, "RRU is offline: %d\n", cnt, strerror(errno));
			ret = connect_to_rru();
			if (ret<0) {
				printf("Fail retry %d...\n", ++cnt);
				sleep(1);
				continue;
			}
		}

		printf("RRU %s is online\n", inet_ntoa(addr.sin_addr));
		cnt = 0;
		while(1) {
			ret = send(csockfd, (char *)buf, sizeof(buf), 0);
			if (ret<0) {
				printf("Loop connection to RRU!\n");
				cnt=0;
				close(csockfd);
				break;
			}
			sleep(3);
			printf("%04d:Heart %s !\n", cnt, cnt%2?"Bon":"Bon-Bon");
			cnt++;
		}
	}
}



#if 0
void *tx_process(void* arg)
{
	size_t ret;

	printf("tx process created....\n");

	g_fp_in = fopen(input_filename, "rb");
	if(g_fp_in == NULL)
	{
		perror(input_filename);
		exit(1);		
	}

	ret = fread(g_data_buf, 1, MAX_DATA_BUF_LEN, g_fp_in);
	if(ret < PKG_LEN * 4 * dl_symbol_num * ant_num)
	{
		fprintf(stderr, "Input file has not enough I/Q data.\n");
		exit(1);
	}

	while(1)
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond,&mutex);		
		pthread_mutex_unlock(&mutex);

		send_dl_packet(g_socket, g_data_buf, dl_symbol_num, g_frame_num, ant_num);

		g_dl_frame_num ++;
	}
}

void* rx_process(void* arg)
{
	size_t len;
	struct data_header *p_data_header;
	struct req_msg* p_req_msg;

	printf("rx process created....\n");

	g_fp_out = fopen(output_filename, "wb");
	if(g_fp_out == NULL)
	{
		perror(output_filename);
		exit(1);		
	}

	while(1)
	{
		len = recv(g_socket, g_rx_buf, MAX_RX_BUF_LEN, 0);
		if(len <= 0)
		{
			printf("error: recvfrom length error.\n");
			exit(1);
		}

		p_data_header = (struct data_header * )g_rx_buf;

		if(p_data_header->data_type == 0xFF)
		{
			if (1!=(p_data_header->frame_num - g_frame_num)) {
				printf("ERR: UP request frame_num from %d to %d\n",
						g_frame_num, p_data_header->frame_num);
			}
			g_frame_num = p_data_header->frame_num;

			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&cond);								
			pthread_mutex_unlock(&mutex);

			p_req_msg = (struct req_msg*)(g_rx_buf + DATA_HEADER_LEN);
			if(verbose)
			{ 
				data_print(p_data_header);
				feedback_info_print(p_req_msg);
			}

			{
				short *ps;
				int   *pi;
				ps = (short *)(g_rx_buf+sizeof(struct data_header)+2);
				pi = (int *)(g_rx_buf+sizeof(struct data_header)+6);
				if ((*ps)!=g_packet_lost_num || (*(ps+1))!=g_packet_late_num) {
					printf("ERR:\n");
					printf("	Prev lost %04X late %04X snt %08X rcv%08X\n",
							g_packet_lost_num, g_packet_late_num,
							g_packet_snd_time, g_packet_rcv_time);
					printf("	This lost %04X late %04X snt %08X rcv%08X\n",
							*ps, *(ps+1),
							*pi, *(pi+1));
				}
				g_packet_lost_num = *ps;
				g_packet_late_num = *(ps+1);
				g_packet_snd_time = *pi;
				g_packet_rcv_time = *(pi+1);
			}
		}
		else if(p_data_header->data_type == 0x01)
		{
			g_ul_pkg_num ++;
			if(verbose) {
				data_print(p_data_header);
			}
			//payload_print(rx_buf + DATA_HEADER_LEN, p_data_header->len);

			{//è®¡ç®—ä¸Šè¡ŒåŠŸç‡
				double power;
				short *p = (short *)(g_rx_buf + DATA_HEADER_LEN);
				int index = p_data_header->chan - 1;
				if (g_calc_ul_pow ) {

					//å¸§å¤´æ—¶æ¸…é›¶
					if (p_data_header->seq_no==0) {
						g_ul_pow_i[index] = 0;
						g_ul_pow_q[index] = 0;
					}

					//IQåˆ†åˆ«ç´¯åŠ 
					for (int i = 0; i<544; i+=2) {
						g_ul_pow_i[index] += p[i]   * p[i];
						g_ul_pow_q[index] += p[i+1] * p[i+1];
					}

					//ç»“å¸
					if (p_data_header->seq_no==59) {
						power  = g_ul_pow_i[index] + g_ul_pow_q[index];
						power /= UL_PKG_NUM*PKG_LEN/4;
						power  = sqrt(power);
						power /= pow(2,15)-1;//å½’ä¸€åŒ–
						power = 20 * log10(power);
						if (power<-30.0) {
							printf(">>>>>>>>>>>");
						}

						if (index)	printf("\t\t\t\t");

						printf("UL#%1X %5d: %.2f\n",index, p_data_header->frame_num, power);
					}

				}
			}


			//ç»Ÿè®¡ä¸Šè¡Œå¸§æ•°ã€æœªdumpçš„å¸§æ•°
			if((p_data_header->chan == up_cap_chan) && (p_data_header->frg_flag == 0x01)) {
				g_ul_frame_num ++;
			} else if (p_data_header->chan != up_cap_chan) {
				g_ul_skip_num++;
			}

			//ä¸Šè¡Œseqä¸è¿ç»­é—®é¢˜å¤„ç†
			if (p_data_header->chan == up_cap_chan) {
				if (
					((p_data_header->seq_no-g_ul_last_seq)!=1) &&
					(p_data_header->seq_no !=0)
				){
					printf("\tBad seq from %ld to %d\n", g_ul_last_seq, p_data_header->seq_no);
					g_ul_frame_loose  ++;
				}
				g_ul_last_seq = p_data_header->seq_no;
				g_ul_last_flag = p_data_header->frg_flag;
				g_ul_last_chan = p_data_header->chan;
			}

			//ä¸Šè¡Œæ•°æ®æ•è·å¤„ç†
			if((p_data_header->chan == up_cap_chan) && (g_ul_frame_num >= 1) && (g_ul_frame_num <= ul_dump_num))
			{
				fwrite(g_rx_buf + DATA_HEADER_LEN, 1, PKG_LEN, g_fp_out);
				g_ul_dump_pkg_num ++;
			}
			if (g_ul_frame_num==ul_dump_num) {
				printf("\næ•è·%då¸§å·²å®Œæˆ\n", ul_dump_num);
			}
		}
		else
		{
			//printf("error: receiver data type error.\n");
		}
	}
}
#endif
int main(int argc, char **argv)
{
	pthread_t tid_rx = 0;
	pthread_t tid_tx = 0;
	pthread_t tid_ht = 0;
	int ch;
	int ret;

	while ((ch = getopt(argc, argv, "i:o:dm:n:r:a:b:p:vhPD:u:")) != EOF)
	{
		switch (ch)
		{
			case 'i':
				input_filename = optarg;
				break;
			case 'o':
				output_filename = optarg;
				break;
			case 'd':
				ant_num = 2;
				break;			
			case 'D'://ä¸‹è¡Œå‘åŒ…å»¶æ—¶ï¼Œå•ä½us
				g_dl_send_delay = atoi(optarg);
				break;
			case 'm':
				dl_symbol_num = atoi(optarg);
				if(dl_symbol_num > 20)
				{
					fprintf(stderr, "Error: too many dl symbols.\n");
					exit(1);
				}
				break;
			case 'n':
				ul_dump_num = atoi(optarg);
				break;
			/*
			 * IPåœ°å€å°†è‡ªåŠ¨è¯†åˆ«
			case 'a':
				bbuip = inet_addr(optarg);
				break;
			case 'b':
				rruip = inet_addr(optarg);	
				break;
			*/
			case 'P'://ä¸Šè¡Œæ˜¯å¦è®¡ç®—å¹¶æ‰“å°åŠŸç‡
				g_calc_ul_pow = 1;
				break;
			case 'p':
				bbu_iq_port = atoi(optarg);
				break;
			case 'r':
				g_rru_id = strval(optarg);	
				break;
			case 'v':
				verbose = 1;
				break;			
			case 'u':
				up_cap_chan = atoi(optarg);
				if (up_cap_chan!=1 && up_cap_chan!=2) {
					up_cap_chan = 1;
				}
				break;			
			case 'h':
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	//æ‰€æœ‰è¾“å‡ºéƒ½ä¸ç¼“å­˜
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	connect_to_rru();

	signal(SIGINT, shut_down);  
	signal(SIGTERM, shut_down);  

/*
	ret = pthread_create(&tid_tx, NULL, tx_process, NULL);
	if(ret != 0)
	{
		fprintf (stderr,"can't create thread: %s\n",strerror(ret));
		exit(1);
	}

	ret = pthread_create(&tid_rx, NULL, rx_process, NULL);
	if(ret != 0)
	{
		fprintf (stderr,"can't create thread: %s\n",strerror(ret));
		exit(1);
	}
*/
	ret = pthread_create(&tid_ht, NULL, ht_process, NULL);
	if(ret != 0)
	{
		fprintf (stderr,"can't create thread: %s\n",strerror(ret));
		exit(1);
	}

	while(!signaled)
	{
		sleep(1);
	}
/*
	if(tid_rx != 0)
	{
		pthread_cancel(tid_rx);
		pthread_join(tid_rx, NULL);    
	}

	if(tid_tx != 0)
	{
		pthread_cancel(tid_tx);
		pthread_join(tid_tx, NULL);    
	}
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	//clear before exit
	fflush(g_fp_out); 
	close(g_socket);
	fclose(g_fp_in);
	fclose(g_fp_out);

	printf("\n----------------statistics information-----------------\n");

	printf("DL: %ld frm, %ld pkt.\n", g_dl_frame_num, g_dl_pkg_num);
	printf("UL: %ld frm, %ld pkt, loose %ld frm\n", g_ul_frame_num, g_ul_pkg_num, g_ul_frame_loose);
	printf("TT: %ld frm, %ld pkt to : %s\n", g_ul_skip_num, g_ul_dump_pkg_num, output_filename);
	*/

	return 0;
}


void usage(const char *progname)
{
  const char *p = strrchr(progname, '/');	// drop leading directory path
  if (p)
    p++;

  if (strncmp(p, "lt-", 3) == 0)		// drop lt- libtool prefix
    p += 3;
  
  fprintf(stderr, "ç”¨æ³•: %s [å„ç§é€‰é¡¹]\n", p);
  fprintf(stderr, "é€‰é¡¹:\n");
  fprintf(stderr, "  -h                   show this message and exit\n");
  fprintf(stderr, "  -i INPUT_FILE_NAME   specify file to transmit I/Q data[default=rx_server.dat]\n");
  fprintf(stderr, "  -o OUTPUT_FILENAME   specify file to receive I/Q data [default=tx_server.dat]\n");
  fprintf(stderr, "  -d                   using two antennas [default=sigle antenna]\n");
  fprintf(stderr, "  -D NUM_OF_DL_DELAY   Delay N us between each DL packet\n");
  fprintf(stderr, "  -m DL_SYMBOL_NUM     Specify the dl symbol number[default=10], <=20\n");
  fprintf(stderr, "  -n DUMPED_UL_FRAME   specify the number of dumped ul frame[default=2]\n");
  fprintf(stderr, "  -r RRU               specify the RRU ID [default=0x11223344]\n");
  //fprintf(stderr, "  -a SERVER_IP_ADDRESS specify the server ip address[default=192.168.9.1]\n");
  //fprintf(stderr, "  -b RRU_IP_ADDRESS    specify the rru ip address[default=192.168.9.3]\n");
  fprintf(stderr, "  -p SERVER_UDP_PORT   specify the udp port of the server [default=33338]\n");
  fprintf(stderr, "  -u UP_CAP_CHANNEL    æŒ‡å®šè¦æ•è·çš„é€šé“å·ï¼ˆ1/2ï¼‰ï¼Œé»˜è®¤æ˜¯1\n");
  fprintf(stderr, "  -P                   Calculate UL power print\n");
  fprintf(stderr, "  -v                   verbose ouput\n");
}

void data_print(struct data_header * p_header)
{
    switch(p_header->data_type)
    {
        case 0xFF:
            printf("*****************data request packet*******************\n");
            break;
        case 0x00:
            printf("----------------DL data packet-------------------------\n");
            break;
        case 0x01:
            printf("----------------UL data packet-------------------------\n");	
            break;
        default:
            printf("----------------Undefined data packet------------------\n");
            exit(1);
    }
	
    printf("rru_id:  %x\n", p_header->rru_id);
    printf("payload length:  %d\n", p_header->len);
    printf("channel number: %d\n", p_header->chan);
    printf("frame number:  %d\n", p_header->frame_num);
    printf("sequence number:  %d\n", p_header->seq_no);
    printf("fragment flag (0->single, 1->header, 2->body, 3->tail):  %d\n", p_header->frg_flag);
    printf("digital gain value: %d\n", p_header->dgain);
    printf("frequency index: %d\n", p_header->freq);
   // printf("-------------------------------------------------------------\n");
    
}

void payload_print(unsigned char* buf, unsigned short len)
{
    int i;

    for(i = 0; i < len; i++)
    {
        if((i % 16) == 0)
        {
            printf("\n");
        }
        printf("%2x ", buf[i]);
    }
}

void feedback_info_print(struct req_msg* p_req_msg)
{
    printf("Ethernet packer CRC error count: %d\n", p_req_msg->crc_err_cnt);
    printf("Ethernet packet lost count: %d\n", p_req_msg->pkg_lost_cnt);
    printf("Ethernet packet later count: %d\n", p_req_msg->pkg_later_cnt);
    printf("Intervla of the first Ethernet packet: %d (us)\n", p_req_msg->time_sof / 125);
    printf("Interval of the last Ethernet packet: %d (us)\n", p_req_msg->time_eof / 125);
   // printf("-------------------------------------------------------------\n");
    if(p_req_msg->time_sof  > 125000)
    {
        printf("---------------------------------------------------------------------------------------error.\n");
    }
}

int strval(const char *str)
{
	int ret=0;
	char ch='\0';
	const char *p=str;

	if ((str==NULL) || strlen(str)==0)
		return -1;

	if ( (strlen(str)>2) && str[0]=='0' && str[1]=='x') {//hex
		while(ch = *p++){
			if (!isxdigit(ch)){
				continue;
			} else {
				ret *= 16;
			}

			if(isdigit(ch)){
				ret += ch - '0';
			}else{
				ret += toupper(ch) - 'A' + 10;
			}
		}
	} else {
		ret = atoi(str);
	}
	return ret;
}


static unsigned char tx_buf[MAX_TX_BUF_LEN];

int data_link_creat(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in rru_addr; 
	
    int sock;

    sock = socket(AF_INET,SOCK_DGRAM, 0);
    if(sock < 0)
    {
        return sock;
    }
	
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = bbuip;
    server_addr.sin_port = htons(bbu_iq_port);
	
    bzero(&rru_addr, sizeof(struct sockaddr_in));
    rru_addr.sin_family = AF_INET;
    rru_addr.sin_addr.s_addr = rruip;
    rru_addr.sin_port = htons(rru_iq_port);
	
    if(bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr,"Bind Error: %s\n", strerror(errno));
        return -1;
    }
	
    if(connect(sock, (struct sockaddr *)&rru_addr, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr, "Connect Error: %s\n", strerror(errno));
        return -1;		
    }
 	
	return sock;
}

void dl_header_bulid(struct data_header* p_header, unsigned char chan, unsigned short seq_no, unsigned char flag)
{
    //set rru id
    p_header->rru_id = g_rru_id;

    //set data type 
    p_header->data_type = 0;	
	
    //set payload length
    p_header->len = PKG_LEN;	
	
    //set channel number
    p_header->chan = chan;	
	
    //set frame number
    p_header->frame_num = g_frame_num;	
	
    //set seqence number
    p_header->seq_no = seq_no;

    //set the fragment number
    p_header->frg_flag = flag;	
}

int send_dl_packet(int sock, unsigned char* p_data_buf, unsigned int dl_symbol_num, unsigned short frame_num, unsigned char ant_num)
{
    int pkg_idx;
    int dl_pkg_num;
    int flag;
    struct data_header header;
	
    int ret;
    int ant_idx;
	
    dl_pkg_num = dl_symbol_num * 4;

    memset(&header, 0, sizeof(header));

    for(pkg_idx = 0; pkg_idx < dl_pkg_num; pkg_idx++)
    {
        //set the fragment number
        if(0 == pkg_idx) //first package
        {
            flag = 1;	
        }	
        else if((dl_pkg_num - 1) == pkg_idx) //last package
        {
            flag = 3;	
            flag = 3;
        }
        else
        {
            flag = 2;	
            flag = 2;
        }
		
        for(ant_idx = 1; ant_idx <= ant_num; ant_idx++)
        {
            dl_header_bulid(&header, ant_idx, pkg_idx, flag);
		
            memcpy(tx_buf, &header, DATA_HEADER_LEN);
            memcpy(tx_buf + DATA_HEADER_LEN, p_data_buf, PKG_LEN);
            
            if(verbose)
            {
                data_print(&header);
            }
           
           // payload_print(tx_buf + DATA_HEADER_LEN, PKG_LEN);

	    usleep(g_dl_send_delay);
            ret = send(sock, tx_buf, PKG_LEN + DATA_HEADER_LEN, 0);				
            if(ret != (PKG_LEN + DATA_HEADER_LEN))
            {
                fprintf(stderr, "Send packet Error: %s\n", strerror(errno));
                return -1;
            }
            g_dl_pkg_num ++;
			
            p_data_buf = p_data_buf + PKG_LEN;
        }

    }

    return 0;
}

int getaddr_by_index(int rawsockfd, int index, struct sockaddr_in * addr)
{
	int ret;
	struct ifreq ifr;
	struct sockaddr_in *addr_tmp;

	strcpy(ifr.ifr_name, "eth1");
	ioctl(rawsockfd, SIOCGIFINDEX, &ifr);
	if (ifr.ifr_ifindex != index) {
		strcpy(ifr.ifr_name, "eth2");
		ioctl(rawsockfd, SIOCGIFINDEX, &ifr);

		if (ifr.ifr_ifindex != index) {
			strcpy(ifr.ifr_name, "eth0");
			ioctl(rawsockfd, SIOCGIFINDEX, &ifr);
			if(ifr.ifr_ifindex != index){
				return -1;
			}
		}
	}

	ret = ioctl(rawsockfd, SIOCGIFADDR, &ifr);
	if (ret<0) {
		return ret;
	} else {
		addr_tmp = (struct sockaddr_in *)&ifr.ifr_addr;
		addr->sin_addr.s_addr = addr_tmp->sin_addr.s_addr;
		return 0;
	}
}

int connect_to_rru(void)
{
	int sockfd;
	unsigned int rruid;
	struct sockaddr_ll rawaddr={0}; 
	unsigned char buf[50]={0};
	unsigned char sndbuf[27]={0};
	struct {
		int rruid;
		char bbuid;
		char rrumac[6];
		int rruip;
		int rrumsk;
		int bbuip;
		short cmport;
		short iqport;
	}__attribute__ ((packed)) msg={0};

	struct udphdr *udphdr=NULL;
	struct iphdr *iphdr=NULL;
	struct sockaddr_in addr;
	int ret;
	int val=1,len ;

	printf("Waiting for RRU\n");
	//´´½¨Ô­Ê¼Ì×½Ó×Ö£¬ÓÃÓÚ½ÓÊÕ¹ã²¥ÏûÏ¢
	//ETH_P_IP 0x800 Ö»½ÓÊÕ·¢Íù±¾»úmacµÄipÀàĞÍµÄÊı¾İÖ¡ 
	sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
	if (sockfd<0) {
		printf("Fail to creat raw socket!\n");
		return -1;
	}

	//½ÓÊÕ¹ã²¥ÏûÏ¢²¢ÅĞ¶Ï
	len = sizeof(rawaddr);
	while (1) {
		ret = recvfrom(sockfd, (char *)buf, sizeof(buf), 0,
				(struct sockaddr *)&rawaddr, &len);
		udphdr = (struct udphdr *)(buf + sizeof(struct iphdr));
		iphdr = (struct iphdr *)buf;
		if (ret!=46 || htons(bbu_udp_port)!= udphdr->dest || htons(rru_udp_port)!=udphdr->source)
			continue;
		ret = getaddr_by_index(sockfd, rawaddr.sll_ifindex, &addr);
		if (ret<0) {
			printf("Î´ÕıÈ·ÉèÖÃÍø¿¨IPµØÖ·\n");
			exit(1);
		}

		rruid = 0;
		rruid |= buf[sizeof(struct udphdr)+sizeof(struct iphdr)+0] << 24;
		rruid |= buf[sizeof(struct udphdr)+sizeof(struct iphdr)+1] << 16;
		rruid |= buf[sizeof(struct udphdr)+sizeof(struct iphdr)+2] << 8;
		rruid |= buf[sizeof(struct udphdr)+sizeof(struct iphdr)+3];
		printf("Request from RRU 0x%08X:", rruid);
		if (rruid != g_rru_id) {
			printf(", need %08X, SKIP\n", g_rru_id);
			continue;
		} else {
			g_rru_id = rruid;
			printf("GOT\n");
		}

		//¸üĞÂBBU¡¢RRUµÄIPµØÖ·
		bbuip = addr.sin_addr.s_addr;
		rruip = htonl(htonl(bbuip) + 1);
		close(sockfd);
		break;
	}


	//¹¹Ôì²¢·¢ËÍÅäÖÃĞÅÏ¢
	memcpy(&msg.rruid,  buf+sizeof(struct udphdr)+sizeof(struct iphdr), 4);
	memcpy(&msg.rrumac, buf+sizeof(struct udphdr)+sizeof(struct iphdr)+4, 6);
	msg.rruip  = rruip;
	msg.rrumsk = inet_addr("255.255.255.0");
	msg.bbuip  = bbuip;
	msg.cmport = htons(bbu_cm_port);
	msg.iqport = htons(bbu_iq_port);
	//payload_print(&msg, sizeof(msg));

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		printf("%s:åˆ›å»ºæ™®é€šå¥—æ¥å­—å¤±è´¥\n", __FUNCTION__);
		return -1;
	}

	//ÎÒÃÇĞèÒªÏòÖ¸¶¨µÄ½Ó¿ÚÈ«Íø¹ã²¥£¬ËùÒÔ
	//1¡¢´ò¿ªÌ×½Ó×ÖµÄ¹ã²¥Ñ¡Ïî
	val = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&val, sizeof(int));

	//2¡¢°ó¶¨µ½Ö¸¶¨µÄ½Ó¿Ú
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = addr.sin_addr.s_addr;
	addr.sin_port = htons(bbu_udp_port);
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		fprintf(stderr,"Bind Error: %s\n", strerror(errno));
		return -1;
	}

	//3¡¢Ïò¹ã²¥µØÖ··¢Æğ½ø¹¥£¡
	addr.sin_addr.s_addr = INADDR_BROADCAST;
	addr.sin_port = htons(rru_udp_port);
	ret = sendto(sockfd, (char *)&msg, sizeof(msg), 0,
			(struct sockaddr *)&addr, sizeof(struct sockaddr_in));

	close(sockfd);
/*
	g_socket = data_link_creat();
	if(g_socket < 0) {
		fprintf(stderr, "data link creat error.\n");
		exit(1);
	}
*/

	printf("Config RRU %08X %s\n", g_rru_id, ret>0?"OK":"Fail");
	return 0;
}
