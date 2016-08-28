/*
 * vnf_plug.c
 *
 *  Created on: 17 avr. 2015
 *      Author: root
 */
#include <config.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "ofproto.h"
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
#include "vlog.h"
#include "bundles.h"
#include "vnf_plug.h"
#include "ofproto.h"



enum vnferror vnfentry_insert(struct ofproto *ofproto, struct vnf_data *content)
OVS_REQUIRES(ofproto_mutex)
{  struct vnftable *table;
   enum vnferror error;
    if (ofproto!=NULL)
    {

     table=ofproto->vnf_table;

     if(table==NULL)
     {error =POINTER_NULL;
      goto exit;}

    fat_rwlock_wrlock(&table->rwlock);

    int id =content->vnf_ID;


    if (table->vnf_table[id].vnf_ID==EMPTY)
    {
    	// a voir pr mutex
    	table->vnf_table[id].p_vnfPacketParam =content->p_vnfPacketParam;
    	table->vnf_table[id].vnfLocation=content->vnfLocation;
    	table->vnf_table[id].vnfOutParam=content->vnfOutParam;
    	// a completer

    	table->nb_entry++;
    	error=NO_ERROR;
    }
    else
    	error=ENTRERR_EXIST;

    fat_rwlock_unlock(&table->rwlock);



exit: return error;

    }

enum vnferror vnfentry_delet(struct ofproto *ofproto, int vnf_id)
{
	enum vnferror error;
	struct vnftable *table;

	table=ofproto->vnf_table;

    // check if rule deletion need lock(mutex)
    fat_rwlock_wrlock(&table->rwlock);
    if (table->vnf_table[vnf_id].vnf_ID==EMPTY)
    	error=ENTRERR_NOT_EXIST;

    else
    {
    	table->vnf_table[vnf_id].vnf_ID=EMPTY;
    	table->nb_entry--;
    	error= NO_ERROR;
    }
	fat_rwlock_unlock(&table->rwlock);


	return error;
}



void vnftable_init( struct ofproto *ofproto )
{    struct vnftable *table;

     int indice;



     fat_rwlock_init(&table->rwlock);

     fat_rwlock_wrlock(&table->rwlock);

     for(indice=1;indice<=ENTRY_NBR ;indice ++)
     {
     table->vnf_table[indice].vnf_ID=EMPTY;
     }
     table->nb_entry=0;
     fat_rwlock_unlock(&table->rwlock);

     ofproto->vnf_table=table;

}



enum vnferror vnftable_clear( struct ofproto *ofproto )
{  struct vnftable *table;
   enum vnferror error;
   int indice;


   if (ofproto==NULL )
   {error=POINTER_NULL;
   goto ext; }

    table=ofproto->vnf_table;
    if(table==NULL)
    {error=POINTER_NULL;
     goto ext;
    }

   fat_rwlock_wrlock(&table->rwlock);

   for(indice=1;indice<=ENTRY_NBR ;indice ++)
   table->vnf_table[indice].vnf_ID=EMPTY;

   table->nb_entry=0;
   fat_rwlock_unlock(&table->rwlock);

   error=NO_ERROR;

ext: return error;

}


enum vnf_experim_type decode_experimenter_type(const struct ofp_header *oh, struct ofpbuf *b)
{
	const struct ofp_header *oph;
	const struct ofp12_experimenter_stats_header  *exph;



	ofpbuf_use_const(b, oh, ntohs(oh->length));

    oph= ofpbuf_pull(b, sizeof *oph);
    exph = ofpbuf_pull(b, sizeof *exph);

    return (exph->exp_type);

}




enum ofperr handle_experimenter(struct ofconn *ofconn, const struct ofp_header *oh)
{
	 struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
	 struct ofpbuf b;
	 uint64_t vnf_content[1024 / 8];
	 enum ofperr error;
	 enum vnf_experim_type type;

	 struct vnf_data *info;
	 uint32_t vnf_id;

    error = reject_slave_controller(ofconn);
    if (error)  return error;


     ofpbuf_use_stub(&b,vnf_content,sizeof vnf_content);
	 type = decode_experimenter_type (oh,&b );



	    switch(type)
	       { case ADD_VNFENTRY:
	       	info= ofpbuf_pull(&b, sizeof *info);
	       	 vnfentry_insert(ofproto,info);
	         	   break;
	       case MODIF_VNFENTRY:
	       	info= ofpbuf_pull(&b, sizeof *info);
	       	   //a modifier
	       	//vnfentry_insert(ofproto,info);  // voir si il faut remplacer par modifi
	       	 break;

	       case DEL_VNFENTRY:
	       	//vnf_id= ofpbuf_pull(&b, sizeof *vnf_id);
            // vnfentry_delet()  faut remplacer entry par id_entry
	       	 break;
	       case CLEAN_VNFTABLE:
	    	   // vnf_clean a ajouter
	            break;
	       case GET_VNFTABLE_CONTENT:
	    	   //ajouter fnct
	         break;
	       default:
	       	//error =1;
	       	;}


error=0;
	return error;

}

void vnftable_destroy(struct ofproto *ofproto)
{


     ovs_mutex_lock(&ofproto_mutex);

     ofproto->vnf_table=NULL;

 	 ovs_mutex_unlock(&ofproto_mutex);
}


