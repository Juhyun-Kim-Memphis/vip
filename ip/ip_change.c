/**
 * @file    ip_change.c
 * @brief   TODO brief documentation here.
 *
 * @author
 * @version $Id$
 */

/*{{{ Headers ----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*---------------------------------------------------------------- Headers }}}*/

#define NUM_OF_IP_SLOT 4
#define RAND_MAX_IP_SLOT 256

void copy_array();

int main()
{
    char primary_priv_ip[128] = "172.32.29.200";
    int m_ipAddr[NUM_OF_IP_SLOT];

    int i_ip[NUM_OF_IP_SLOT] = {172,32,29,0};
    int temp_ip[NUM_OF_IP_SLOT];

    char *p;
    int i = 0;
    int cnt = 0;
    int rand_v;

    char result[MAX_ADDR_LEN];

    srand((unsigned int)time(NULL));

    p = strtok(primary_priv_ip,".");
    m_ipAddr[i]=atoi(p);

    while(p!=NULL)
    {
        m_ipAddr[i]=atoi(p);
        i++;
        p = strtok(NULL,".");
    }

    copy_array(m_ipAddr,i_ip,NUM_OF_IP_SLOT);
    i_ip[3] = 0;

    while(1)
    {
        rand_v = rand()%RAND_MAX_IP_SLOT;

        printf("rand_v : %d\n",rand_v);

        if(m_ipAddr[3] != rand_v)
            break;
    }
    // private ip address가 여러개일 가능성...

    temp_ip[3] = rand_v;

//    result[0] = '\0';
    sprintf(result,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);

    printf("Changed Ip is : %s\n",result);
    return 0;
}

void copy_array(int from[], int to[], int n)
{
    int i = 0;
    for (i=0; i<n; i++)
        to[i] = from[i];
}




/* end of ip_change.c */
