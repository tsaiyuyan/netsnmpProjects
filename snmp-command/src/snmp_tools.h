#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdbool.h>

struct snmpSessini
{

};

int ysnmp_broadcast(struct snmp_session *ss, const char *intoid, char *host_ip);
int ysnmp_setInt(struct snmp_session *ss, const char *intoid, char *val);
int ysnmp_getInt(struct snmp_session *ss, const char *intoid, char *val);
