/*
 * vnf_plug.h
 *
 *  Created on: 16 avr. 2015
 *      Author: root
 */

#ifndef OFPROTO_VNF_PLUG_H_
#define OFPROTO_VNF_PLUG_H_

#include <stdint.h>
#include <config.h>
#include "ofproto.h"
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "ofproto-dpif.h"
/*
#include "bitmap.h"
#include "byte-order.h"
#include "classifier.h"
#include "connectivity.h"
#include "connmgr.h"
#include "coverage.h"
#include "dynamic-string.h"
#include "hash.h"
#include "hmap.h"
#include "meta-flow.h"
#include "netdev.h"
#include "nx-match.h"
#include "ofp-actions.h"
#include "ofp-errors.h"
#include "ofp-msgs.h"
#include "ofp-print.h"
#include "ofp-util.h"
#include "ofpbuf.h"
#include "ofproto-provider.h"
#include "openflow/nicira-ext.h"
#include "openflow/openflow.h"
#include "ovs-rcu.h"
#include "packets.h"
#include "pinsched.h"
#include "pktbuf.h"
#include "poll-loop.h"
#include "random.h"
#include "seq.h"
#include "shash.h"
#include "simap.h"
#include "smap.h"
#include "sset.h"
#include "timeval.h"
#include "unaligned.h"
#include "unixctl.h"
*/
#include "bundles.h"
#include <flow.h>
#include <dpif.h>





//START KAHINA

#define ENTRY_NBR  100
#define EXPERIMENTER_ID 99999


/* si envoi */



enum vnferror
{ NO_ERROR,
  ENTRERR_EXIST,
  ENTRERR_NOT_EXIST,
  TBLERR_EMPTY,
  VNFIDERR,
  POINTER_NULL
  } ;


enum vnf_experim_type
{ ADD_VNFENTRY,
  MODIF_VNFENTRY,
  DEL_VNFENTRY,
  CLEAN_VNFTABLE,
  GETTABLE_REQUEST,
  ENTRY_REQUEST,
  GETTABLE_REPLY,
  ENTRY_REPLY,
  VNF_ACK,
  VNF_ERROR,

};


typedef struct vnfentry
{
	struct ofproto* ofproto;
	bool used;
	char extern_library[32];
	char fnct_name[32];
	struct ofpbuf *internal_parameters;
	struct ofpbuf *external_parameters;
	int process_packet;
} vnfentry;


typedef vnfentry table[ENTRY_NBR];

struct vnftable
 { struct ovs_mutex rwlock;
   int nb_entry;
   table vnf_table;
} ;


// test pour execution action

extern struct vnftable *global_vnf_table;


// utilisé pour extraire le payload des message experimenter

struct vnf_data_type1
 {  uint32_t vnf_id;
 	char extern_library [32];
	char fnct_name [32] ;
	uint16_t param_length;
	char param [2] ;
	uint16_t external_param_length;
    //
  };


// utilisé pour extraire le payload des message experimenter

struct vnf_data_type2
{ uint32_t  id;
	};

//utilisé pr extraire le nom de la fonction
//voir si on garde ou non ??

struct vnf_data_type3
{ uint8_t name[32];
	};

// utilisé pour décoder le header de l'experimenter

struct experim_headers
{ struct ofp_header oh;
  struct ofp12_experimenter_stats_header exph;
	};


//struct vnftable *vnf_tbl(const struct ofproto_dpif *dpif); a supprimer !


/*entry handling */
enum vnferror vnfentry_insert(struct ofproto *ofproto, struct vnf_data_type1 *content, struct ofpbuf *data);
enum vnferror vnfentry_delet(struct ofproto *ofproto, uint32_t vnf_id);
enum vnferror vnfentry_modif(struct ofproto *ofproto, struct vnf_data_type1 *content, struct ofpbuf *data);

/*table  handling */
void vnftable_destroy(struct ofproto* ofproto);
enum vnferror vnftable_clear( struct ofproto *ofproto );
void vnftable_init( struct ofproto *ofproto );


/*message handling */
enum ofperr handle_experimenter(struct ofconn *ofconn, const struct ofp_header *oh);
enum vnf_experim_type decode_experimenter_type(const struct ofp_header *oh, struct ofpbuf *b);


/*utils */
void write_info(struct vnf_data_type1 *data);
void swap_data1(struct vnf_data_type1 *data);
void swap_data2(struct vnf_data_type2 *data);
inline uint32_t swap_uint32( uint32_t val );
inline uint64_t swap_uint64( uint64_t val );

/*send message to controller */
void get_table_content(struct ofconn *ofconn ,ovs_be32 xid);
void send_reply(struct ofconn *ofconn ,ovs_be32 xid,enum vnferror error);
enum vnferror get_entry_content(struct ofconn *ofconn ,ovs_be32 xid,uint32_t vnf_id);








/*openlog("slog", LOG_PID|LOG_CONS, LOG_USER);
syslog(LOG_INFO, "aberkane action.c  . execute action vnf   ");
closelog();*/






/*

//! Byte swap unsigned short
uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
int16_t swap_int16( int16_t val )
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap int
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

// 64bit byte swapping

int64_t swap_int64( int64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}


*/

//void vnftable_print(struct ofproto *ofproto);

//void vnfentry_insert(struct vnfentry *entry);

//void test_vnftable(struct ofproto *ofproto);



// void *vnf_action(void*); a corriger et completer

  /***ajouter
  int vnfindex; // a voir

  int* vnflocation; // pointeur vers location de la vnf -->remplace int pas struct adequate


  pkt_metadata** p_vnfinputparam;
  pkt_metadata** p_vnfOutputparam;

*/

/*START KAHINA */
//    const char *argum;
//    argum="ofprotod_pif//alloc ";
//    VLOG_WARN("kahina test module %s",argum);

/*END KAHINA */




/*
void vnftable_print(struct ofproto *ofproto)
{

	FILE *f;
	struct vnftable *table;

	f = fopen("/home/air/vnftable.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}


    table=ofproto->vnf_table;
	char *contenu_table = "";
    char *str="";
	struct vnfentry *entry;
	int i=0;

	fat_rwlock_rdlock(&table->rwlock);
	fprintf(f, "VNF TABLE nombre entre =   %d \n", table->nb_entry);

	HMAP_FOR_EACH(entry, node, &table->vnfentry_set)
	{ovs_mutex_lock(&entry->mutex);
	  sprintf(str,"\n index entry = %d ---- nombre packets = %d ",i, entry->nb_packet);
	  contenu_table=strcat(contenu_table,str);
      ovs_mutex_unlock(&entry->mutex);
	 i++;
	 }
	fat_rwlock_unlock(&table->rwlock);
	fprintf(f, "Contenu de la table vnf \n   %s \n", contenu_table);

	 fclose(f);
	}

// fonction a mettre en commentaire


void test_vnftable(struct ofproto *ofproto)
 {

  struct vnftable * table;
  struct vnf_data *info;

  table=vnftable_init(ofproto);
  memset(info,0,sizeof *info );
 int i;
 for(i=1;i<=20;i++)
  {
   info->index=i;
   info->packet_number=50+i;
   vnfentry_insert(table,info);

  }
  vnftable_print(ofproto);
 }


*/




/*
openlog("slog", LOG_PID|LOG_CONS, LOG_USER);
syslog(LOG_INFO, "aberkane test syslog    ");
closelog();

        ///test

    	strncpy(table->vnf_table[id].fnct_name,content->fnct_name,32);
    	char *str2,*str1 ;
    	str2=content->fnct_name;
    	while (*str2==' ') str2++;
    	if (str2!=content->fnct_name) memmove(str1,str2,strlen(str2)+1);
        VLOG_WARN("kahina.vnfentry_insert :  insertion entré  -->   %s  ",str1);

        //test







*/



//START KAHINA



/*kahina
    char buffer[100];

    strncpy(buffer,"debut \n",30);

    int fd2 = open("/tmp/kahina.svg", O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if(fd2!=-1){
    write(fd2,buffer,30);
    	//close(fd2);
    }
    //kahina */



#endif /* OFPROTO_VNF_PLUG_H_ */
