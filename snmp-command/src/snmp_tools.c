
#include <string.h>
#include <errno.h>
#include <time.h>
#include "snmp_tools.h"

#define YSNMP_ERROR    -1
#define YSNMP_OK        0

unsigned int GetTickCount()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
        return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int ysnmp_setInt(struct snmp_session *ss, const char *intoid, char *val)
{
    bool SnmpDebug = false;
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    char type;

    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;

    int status;

    if (SnmpDebug)
        fprintf(stdout, "Set Int OID: %s\n", intoid);

    type = 'i';
    /*
        type - one of i, u, t, a, o, s, x, d, n
        i: INTEGER, u: unsigned INTEGER, t: TIMETICKS, a: IPADDRESS
        o: OBJID, s: STRING, x: HEX STRING, d: DECIMAL STRING
        U: unsigned int64, I: signed int64, F: float, D: double
    */
    pdu = snmp_pdu_create(SNMP_MSG_SET);

    // create set request and add object names and values

    if (snmp_parse_oid(intoid, anOID, &anOID_len) == NULL)
    {
        snmp_perror(intoid);
        return YSNMP_ERROR;
    }

    if (snmp_add_var(pdu, anOID, anOID_len, type, val))
    {
        snmp_perror(intoid);
        return YSNMP_ERROR;
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status == STAT_SUCCESS)
    {
        if (response->errstat != SNMP_ERR_NOERROR)
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n",
                    snmp_errstring(response->errstat));
            return YSNMP_ERROR;
        }
    }
    else if (status == STAT_TIMEOUT)
    {
        fprintf(stderr, "SNMP SetTimeout: No Response from %s\n",
                ss->peername);
        return YSNMP_ERROR;
    }
    else
    { /* status == STAT_ERROR */
        snmp_sess_perror("snmpset", ss);
        return YSNMP_ERROR;
    }

    if (response)
    {
        snmp_free_pdu(response);
        return YSNMP_OK;
    }
    return YSNMP_ERROR;
}

int ysnmp_getInt(struct snmp_session *ss, const char *intoid, char *val)
{
    netsnmp_pdu *pdu;
    netsnmp_variable_list *vars;
    netsnmp_pdu *response;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;
    int status;
    int count = 1;

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    if (!snmp_parse_oid(intoid, anOID, &anOID_len))
    {
        snmp_perror(intoid);
        return YSNMP_ERROR;
    }

    snmp_add_null_var(pdu, anOID, anOID_len);

    /* Send the Request out. */
    status = snmp_synch_response(ss, pdu, &response);

    /* Process the response. */
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR)
    {
        /* SUCCESS: Print the result variables */
        for (vars = response->variables; vars; vars = vars->next_variable)
            print_variable(vars->name, vars->name_length, vars);

        /* manipuate the information ourselves */
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
            if (vars->type == ASN_OCTET_STR)
            {
                char *sp = (char *)malloc(1 + vars->val_len);
                memcpy(sp, vars->val.string, vars->val_len);
                sp[vars->val_len] = '\0';
                printf("value #%d is a string: %s\n", count++, sp);
                free(sp);
            }
            else if (vars->type == ASN_INTEGER)
            {
                int nValue = 0;
                nValue = *vars->val.integer;
                //memcpy(nValue,vars->val.integer,vars->val_len);
                printf("value #%d is a integer: %d\n", count++, nValue);
            }
            else
                printf("value #%d is NOT a string! Ack!\n", count++);
        }
    }
    else
    {
        if (status == STAT_SUCCESS)
            fprintf(stderr, "Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
        else if (status == STAT_TIMEOUT)
            fprintf(stderr, "Timeout: No response from %s.\n", ss->peername);
        else
            snmp_sess_perror("snmpdemoapp", ss);
    }

    if (response)
        snmp_free_pdu(response);

    return 0;
}

int ysnmp_broadcast(struct snmp_session *ss, const char *intoid, char *host_ip)
{
    printf("snmp_broadcast() start\n");

    struct timeval timeout;
    fd_set fdset;
    int fds = 0, block = 1;
    long lasttime;
    int nsnmpok = YSNMP_ERROR;

    netsnmp_pdu *pdu;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;


    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (!snmp_parse_oid(intoid, anOID, &anOID_len))
    {
        snmp_perror(intoid);
        exit(1);
    }
    snmp_add_null_var(pdu, anOID, anOID_len);

    /* int status = 0;
       netsnmp_pdu *response;
       status = snmp_synch_response(Session, req, &response);

       if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
       snmp_ipaddr *pipaddress = (snmp_ipaddr*)response->transport_data;
       long u_tmp = pipaddress->sa_align;
 
       char tmp[20]={0};

       sprintf(tmp, "%d.%d.%d.%d\n" , (u_tmp&0xFF), (u_tmp>>8)&0xFF,(u_tmp>>16)&0xFF,(u_tmp>>24)&0xFF);
       TRACE0(tmp);
       }*/

    if (!(snmp_send(ss, pdu)))
    {
        //snmp_perror("snmp_send");
        snmp_free_pdu(pdu);
        return 1;
    }

    /* struct timeval  *tvp;
       int             numfds, count;*/
    //Read response
    lasttime = GetTickCount();
    do
    {
        FD_ZERO(&fdset);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        snmp_select_info(&fds, &fdset, &timeout, &block);
        fds = select(fds, &fdset, NULL, NULL, &timeout);
        if (fds)
        {
            printf("xxxxx\n");
            snmp_read(&fdset);
            char *err = 0;
            snmp_error(ss, NULL, NULL, &err);
            // TRACE0(err);
            nsnmpok = YSNMP_OK;
        } 
        else if(fds == 0)
        {
            //snmp_timeout();
            printf("yyyyyy\n");
            //break;
            usleep(100000); //100ms
        }else 
        {
            snmp_perror("fds < 0");
            printf("zzzzzz\n");
            break;
        }

    } while ((GetTickCount() - lasttime) < 10000);

    printf("snmp_broadcast() end\n");
    return nsnmpok;
}