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

#define MAX_ADDR_LEN 100
#define IP_length 4
#define MAX 256

void copy_array();

int main()
{

    char o_ip[MAX_ADDR_LEN] = "172.32.29.200";
    int m_ipAddr[IP_length];

    int i_ip[IP_length] = {172,32,29,0};
    int temp_ip[IP_length];

    char *p;
    int i = 0;
    int cnt = 0;
    int rand_v;

    char result[MAX_ADDR_LEN];

    srand((unsigned int)time(NULL));

    p = strtok(o_ip,".");
    m_ipAddr[i]=atoi(p);

    while(p!=NULL)
    {

        m_ipAddr[i]=atoi(p);
        i++;

        p = strtok(NULL,".");


    }

    printf("original ip is : %d\n",m_ipAddr[0]);
    printf("original ip is : %d\n",m_ipAddr[1]);
    printf("original ip is : %d\n",m_ipAddr[2]);
    printf("original ip is : %d\n",m_ipAddr[3]);


    copy_array(m_ipAddr,i_ip,IP_length);
    i_ip[3] = 0;

    // working on & bit
    for (cnt=0;cnt<=3;cnt++)
    {
        printf("Check i_ip // result is : %d\n",i_ip[cnt]);
    }


    while(1)
    {
        rand_v = rand()%MAX;

        printf("rand_v : %d\n",rand_v);

        if(m_ipAddr[3] != rand_v)
            break;
    }
    // private ip address가 여러개일 가능성...

    copy_array(i_ip,temp_ip,IP_length);
    temp_ip[3] = rand_v;

    // working on AND
    for (cnt=0;cnt<=3;cnt++)
    {
        printf("Check temp_ip // result is : %d\n" ,temp_ip[cnt]);
    }

    result[0] = '\0';

    sprintf(result,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);

    printf("Changed Ip is : %s\n",result);
    return 0;
}

void copy_array(int from[], int to[], int n)
{
    int i = 0;
    for (i=0; i<n; i++)
    {
        to[i] = from[i];
    };
}




/* end of ip_change.c */
