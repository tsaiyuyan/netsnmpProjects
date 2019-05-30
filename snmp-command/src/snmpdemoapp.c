#include "snmp_tools.h"
#include  <arpa/inet.h>
// === snmp const variblae ===
const char *our_query_oid_basic = ".1.3.6.1.2.1.1.1.0";  // This oid is used for broadcast.
const char *our_query_oid = ".1.3.6.1.4.1.16536.1.1.0";  // This is my custom oid.
const char *our_query_oid2 = ".1.3.6.1.4.1.16535.1.1.0"; // This is my custom oid.
const char *our_target_address = "172.16.28.118:1611";
const char *our_broadcast_address = "255.255.255.255:1611";
const char *our_rwcommunity = "wezio";
const char *our_rocommunity = "ezio";
/* change the word "define" to "undef" to try the (insecure) SNMPv1 version */
#undef DEMO_USE_SNMP_VERSION_3

#ifdef DEMO_USE_SNMP_VERSION_3
const char *our_v3_rwuser = "wyuyan";
const char *our_v3_passphrase = "99310362";
#endif
// ===========================

int print_result(int status, struct snmp_pdu *pdu)
{
    /* Device information variables */
    char ip_addr[1024];
    /* remote IP detection variables */
    netsnmp_indexed_addr_pair *responder = (netsnmp_indexed_addr_pair *)pdu->transport_data;
    struct sockaddr_in *remote = NULL;

    printf("%s: Handling SNMP response \n", __func__);

    if (responder == NULL || pdu->transport_data_length != sizeof(netsnmp_indexed_addr_pair))
    {
        printf("%s: Unable to extract IP address from SNMP response.\n",__func__);
        return 0;
    }
    remote = (struct sockaddr_in *)&(responder->remote_addr);
    if (remote == NULL)
    {
        printf("%s: Unable to extract IP address from SNMP response.\n",__func__);
        return 0;
    }
    //snprintf(ip_addr, sizeof(ip_addr), "%s", inet_ntoa(remote->sin_addr));
    sprintf(ip_addr, "%s", inet_ntoa(remote->sin_addr));
    printf("%s: IP Address of responder is %s\n", __func__, ip_addr);

    char buf[1024];
    struct variable_list *vp;
    int ix;
    struct timeval now;
    struct timezone tz;
    struct tm *tm;

    gettimeofday(&now, &tz);
    tm = localtime(&now.tv_sec);
    fprintf(stdout, "%.2d:%.2d:%.2d.%.6d ", tm->tm_hour, tm->tm_min, tm->tm_sec,
            now.tv_usec);
    switch (status)
    {
    case STAT_SUCCESS:
        vp = pdu->variables;
        if (pdu->errstat == SNMP_ERR_NOERROR)
        {
            while (vp)
            {
                snprint_variable(buf, sizeof(buf), vp->name, vp->name_length, vp);
                fprintf(stdout, " %s\n", buf);
                vp = vp->next_variable;
            }
        }
        else
        {
            for (ix = 1; vp && ix != pdu->errindex; vp = vp->next_variable, ix++)
                ;
            if (vp)
                snprint_objid(buf, sizeof(buf), vp->name, vp->name_length);
            else
                strcpy(buf, "(none)");
            fprintf(stdout, " %s: %s\n", buf, snmp_errstring(pdu->errstat));
        }
        return 1;
    case STAT_TIMEOUT:
        fprintf(stdout, " Timeout\n");
        return 0;
    case STAT_ERROR:
        // snmp_perror(sp->peername);
        return 0;
    }
    return 0;
}

int asynch_response(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic)
{
    //netsnmp_session *host = (netsnmp_session *)magic;
    struct snmp_pdu *req;
    printf("peer %s:\n", sp->peername);
    printf("local %s:\n", sp->localname);
    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {
        print_result(STAT_SUCCESS, pdu);
    }
    else
        print_result(STAT_TIMEOUT, pdu);

    return 0; //return 0 means don't close session for snmp_read
    return 1;
}

int main(int argc, char **argv)
{
    netsnmp_session session, *ss;
    netsnmp_set_mib_directory("/usr/local/net-snmp/share/snmp/mibs");

    /* Initialize the SNMP library */
    init_snmp("snmpdemoapp");

    /* Initialize a "session" that defines who we're going to talk to */
    snmp_sess_init(&session); /* set up defaults */

    /* set up the authentication parameters for talking to the server */

#ifdef DEMO_USE_SNMP_VERSION_3

    /* Use SNMPv3 to talk to the experimental server */

    /* set the SNMP version number */
    session.version = SNMP_VERSION_3;

    /* set the SNMPv3 user name */
    session.securityName = strdup(our_v3_rwuser);
    session.securityNameLen = strlen(session.securityName);

    /* set the security level to authenticated, but not encrypted */
    session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;

    /* set the authentication method to MD5 */
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol) / sizeof(oid);
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

    /* set the authentication key to a MD5 hashed version of our
       passphrase "The Net-SNMP Demo Password" (which must be at least 8
       characters long) */
    if (generate_Ku(session.securityAuthProto,
                    session.securityAuthProtoLen,
                    (u_char *)our_v3_passphrase, strlen(our_v3_passphrase),
                    session.securityAuthKey,
                    &session.securityAuthKeyLen) != SNMPERR_SUCCESS)
    {
        snmp_perror(argv[0]);
        snmp_log(LOG_ERR, "Error generating Ku from authentication pass phrase. \n");
        exit(1);
    }

#else  /* we'll use the insecure (but simplier) SNMPv1 */

    /* set the SNMP version number */
    session.version = SNMP_VERSION_2c;

    /* set the SNMPv1 community name used for authentication */
    session.community = "wezio";
    session.community_len = strlen(session.community);
#endif /* SNMPv1 */

    session.flags = SNMP_FLAGS_UDP_BROADCAST;
    session.callback = asynch_response;
    session.callback_magic = ss;

    //session.peername = strdup(our_target_address);
    session.peername = strdup(our_broadcast_address);
    /* Open the session */
    ss = snmp_open(&session); /* establish the session */

    //iint on=1;
    //  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR | SO_BROADCAST,&on,sizeof(on));
    if (!ss)
    {
        snmp_sess_perror("ack", &session);
        exit(1);
    }

    // === yuyan start ===
    srand(time(0));
    char tmp[1024] = "";

    ysnmp_broadcast(ss, our_query_oid_basic, tmp);
    //while(1)
    /*{
        sprintf(tmp,"%d",rand()%100);
        ysnmp_setInt(ss,our_query_oid,tmp);
        printf("tmp=%s\n",tmp);
        sleep(5);
    }
    ysnmp_getInt(ss,our_query_oid,tmp);
    */
    snmp_close(ss);

    return (0);
}