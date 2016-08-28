/*
 * vnfplug.c
 *
 *  Created on: 10 juin 2015
 *      Author: root
 */




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
#include "openvswitch/vlog.h"
#include "bundles.h"
#include "vnf_plug.h"
#include "ofproto.h"
#include "ofproto-dpif.h"
#include "ofpbuf.h"
#include <dlfcn.h>   // for extern library


#include <unistd.h>
#include <syslog.h>


bool vnfsend_ack =1;
void *vnf_lib;
// parametre pour la fonction d'affichage vlog
VLOG_DEFINE_THIS_MODULE(vnf_plug);



//! Byte swap unsigned int
inline uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}


inline uint64_t swap_uint64( uint64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}


void swap_info(struct vnf_data_type1 *data)
{
	//amrane
	VLOG_WARN("Amrane   le 1er version de id est : %d",data->vnf_id );
	data->vnf_id= ntohl(data->vnf_id);
	VLOG_WARN("Amrane param_length : %d ntohs %d",data->param_length,  ntohs(data->param_length));
	data->param_length=ntohs(data->param_length);
	VLOG_WARN("Amrane external_param_length : %d ntohs %d",
			data->external_param_length, ntohs(data->external_param_length));
	data->external_param_length=ntohs(data->external_param_length);
    //VLOG_WARN("Amrane   le 2ème version de id est : %d",data->vnf_id );

}



void write_info(struct vnf_data_type1 *data)
{
    VLOG_WARN("kahina   le contenu de experimenter est : %d",0);
	VLOG_WARN("kahina    vnf_id  = : %d",data->vnf_id);
	VLOG_WARN("kahina    extern_library  : %s",data->extern_library);
	VLOG_WARN("kahina    external_param_length  : %d",data->external_param_length);
	VLOG_WARN("kahina   fonction name    : %s",data->fnct_name);
	VLOG_WARN("kahina   param length   : %d",data->param_length);

}



enum vnferror vnfentry_modif(struct ofproto *ofproto, struct vnf_data_type1 *content, struct ofpbuf *data)
OVS_REQUIRES(ofproto_mutex)
{  struct vnftable *table;
   enum vnferror error;
   int taille_parametres,id;
   struct ofpbuf *parametres;

   if (ofproto==NULL)
    {error =POINTER_NULL;
    goto exit;}


     table=ofproto->vnf_table;

     if(table==NULL)
     {error =POINTER_NULL;
      goto exit;}

    ovs_mutex_lock(&table->rwlock);

    id =content->vnf_id;


    if (table->vnf_table[id].used==true)
    {
    	VLOG_WARN("kahina.vnfentry_modif :  modification entrée  -->   %d  ",id);
        strncpy(table->vnf_table[id].fnct_name,content->fnct_name,32);
        taille_parametres = content->param_length;


    	//recuperer parametre
        parametres = ofpbuf_clone_data(data,taille_parametres);
        table->vnf_table[id].internal_parameters=parametres;

    	// a completer
        error=0;
    }
    else
    	error= ENTRERR_NOT_EXIST;

    ovs_mutex_unlock(&table->rwlock);



exit: return error;

    }


enum vnferror vnfentry_insert(struct ofproto *ofproto, struct vnf_data_type1 *content, struct ofpbuf *data)
OVS_REQUIRES(ofproto_mutex)
{  struct vnftable *table;
   enum vnferror error;
   int taille_parametres,id, taille_parametres_extern;
   struct ofpbuf *parametres, *external_param;

   if (ofproto==NULL)
    {error =POINTER_NULL;
    goto exit;}


     table=ofproto->vnf_table;

     if(table==NULL)
     {error =POINTER_NULL;
      goto exit;}

    ovs_mutex_lock(&table->rwlock);

    id =content->vnf_id;

    if ((id>ENTRY_NBR)||(id<0))
    {   ovs_mutex_unlock(&table->rwlock);
    	return  VNFIDERR; }

    if (table->vnf_table[id].used==false)
    {
      VLOG_WARN("kahina.vnfentry_insert :  insertion entrée  -->   %d  ",id);
      strncpy(table->vnf_table[id].extern_library,content->extern_library,32);
      strncpy(table->vnf_table[id].fnct_name,content->fnct_name,32);
      taille_parametres = content->param_length;
      taille_parametres_extern = content->param_length;
      //recuperer parametre
      external_param = ofpbuf_clone_data(data,taille_parametres_extern);
      table->vnf_table[id].external_parameters = taille_parametres_extern;

    	//recuperer parametre
        parametres = ofpbuf_clone_data(data,taille_parametres);
        table->vnf_table[id].internal_parameters=parametres;

    	// a completervnftable
        table->vnf_table[id].used=true;
    	table->nb_entry++;
    	/*VLOG_WARN("kahina.  extern_library %s  ",table->vnf_table.extern_library);
    	VLOG_WARN("kahina.  external_parameters %p  ",table->vnf_table.external_parameters);
    	VLOG_WARN("kahina.  fnct_name %s  ",table->vnf_table.fnct_name);
    	VLOG_WARN("kahina.  internal_parameters %p ",table->vnf_table.internal_parameters);*/
    	error=0;
    }
    else
    	error=ENTRERR_EXIST;

    ovs_mutex_unlock(&table->rwlock);



exit: return error;

    }


void vnftable_init( struct ofproto *ofproto )
{    struct vnftable *table;
     int indice;

     table=xmalloc(sizeof *table);

     ovs_mutex_init_adaptive(&table->rwlock);

     ovs_mutex_lock(&table->rwlock);

     for(indice=1;indice<=ENTRY_NBR ;indice ++)
     {
     table->vnf_table[indice].used=false;
     }
     table->nb_entry=0;
     ovs_mutex_unlock(&table->rwlock);

     ofproto->vnf_table=table;
     VLOG_WARN("kahina . initialisation de la table_Vnf : %s",ofproto->name);

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

   ovs_mutex_lock(&table->rwlock);

   for(indice=1;indice<=ENTRY_NBR ;indice ++)
   table->vnf_table[indice].used =false;

   table->nb_entry=0;
   ovs_mutex_unlock(&table->rwlock);

   error=NO_ERROR;

ext: return error;

}

enum vnferror vnfentry_delet(struct ofproto *ofproto, uint32_t vnf_id)
{
	enum vnferror error;
	struct vnftable *table;

	// check if rule deletion need lock(mutex)

   if ((vnf_id>ENTRY_NBR)||(vnf_id<0))
      return VNFIDERR;


	table=ofproto->vnf_table;

    ovs_mutex_lock(&table->rwlock);


    if (table->vnf_table[vnf_id].used==false)
                   error=ENTRERR_NOT_EXIST;

    else
    {   table->vnf_table[vnf_id].used =false;
    	table->nb_entry--;
    	error=0;
     }

    ovs_mutex_unlock(&table->rwlock);


	return error;
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
     int vnferror;
	 struct vnf_data_type1 *info;
	 struct vnf_data_type2 *vnf_id;
	 struct vnf_data_type3 *name_fn;   // peut etre suppr
	 uint32_t id;

    error = reject_slave_controller(ofconn);
    if (error)  return error;


    ofpbuf_use_stub(&b,vnf_content,sizeof vnf_content);
    type= swap_uint32(decode_experimenter_type (oh,&b ));
    

      switch(type)
	      {
        case ADD_VNFENTRY:
          VLOG_WARN("kahina.handle_experimenter reception  msg experimenter de  type: ADD_VNFENTRY %d",0);
          VLOG_WARN("kahina.handle_experimenter : sizeof *info %d",sizeof *info);
          info= ofpbuf_pull(&b, sizeof *info);

	       	swap_info(info);
	       	write_info(info);
            vnferror=vnfentry_insert(ofproto,info,&b);
            VLOG_WARN("Amrane vnferror  %d",vnferror);
            break;

	       case MODIF_VNFENTRY:
          VLOG_WARN("kahina.handle_experimenter reception  msg experimenter de  type: MODIF_VNFENTRY %d",0);
	       	info= ofpbuf_pull(&b, sizeof *info);
	       	swap_info(info);
	       	vnferror=vnfentry_modif(ofproto,info,&b);

	       	break;

	       case DEL_VNFENTRY:
         VLOG_WARN("kahina.handle_experimenter reception  msg experimenter de  type:  DEL_VNFENTRY %d",0);
	       	  vnf_id= ofpbuf_pull(&b, sizeof *vnf_id);
	       	  id =swap_uint32(vnf_id->id);
	    	  vnferror=vnfentry_delet(ofproto,id);

	       	 break;

	       case CLEAN_VNFTABLE:
         VLOG_WARN("kahina.handle_experimenter reception  msg experimenter de  type CLEAN_VNFTABLE %d",0);
	    	vnferror= vnftable_clear(ofproto);

	        break;

	       case GETTABLE_REQUEST:
         VLOG_WARN("kahina.handle_experimenter reception  msg experimenter de  type GETTABLE_REQUEST %d",0);
	    	 get_table_content(ofconn ,oh->xid);

          break;

	       case ENTRY_REQUEST:
	          vnf_id= ofpbuf_pull(&b, sizeof *vnf_id);
	    	  id =swap_uint32(vnf_id->id);
	    	  vnferror=get_entry_content(ofconn ,oh->xid,id);



            break;

	       default:
	       	//error =1;
	     VLOG_WARN("kahina.handle_experimenter erreur : type experimenter non supporté   %d",0);
	       	;}

         if ((vnfsend_ack= true) || (vnferror!=0))
    	     VLOG_WARN("Amrane inside IF vnferror = %d",vnferror);
        	 send_reply(ofconn ,oh->xid,vnferror);

	return 0;

}



void vnftable_destroy(struct ofproto *ofproto)
{

    struct vnftable* table= ofproto->vnf_table;
    ofproto->vnf_table=NULL;
    ovs_mutex_destroy(&table->rwlock);
    free(table);


}



void get_table_content(struct ofconn *ofconn ,ovs_be32 xid)
{

 struct vnf_data_type1 *data;
 struct ofp12_experimenter_stats_header  *exph;
 uint64_t *pad;


 struct ofpbuf *b;
 enum ofpraw raw;
 enum ofp_version version;

 int limit,i,nb_entry;


  struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
 enum ofputil_protocol protocol = ofconn_get_protocol(ofconn);
 struct vnftable *table; table =ofproto->vnf_table;

 VLOG_WARN("recuperation table dans ofproto  : %s",ofproto->name);
 /**************************************************/

 version = ofputil_protocol_to_ofp_version(protocol);
 raw= OFPRAW_OFPT_EXPERIMENTER ;


 b = ofpraw_alloc_xid(raw, version, xid, 0);


 struct experim_headers *hdrs =b->base;
 hdrs->exph.experimenter= EXPERIMENTER_ID;
 hdrs->exph.exp_type=GETTABLE_REPLY;

 ovs_mutex_lock(&table->rwlock);

 if (table ==NULL)  return;


 limit=table->nb_entry;
 if (limit != 0)
 {
  nb_entry=1;i=1;
  VLOG_WARN("kahina.get_table_content lecture table, nombre entrée  -->  %d",limit);

  while ((nb_entry<=limit) & (i<=ENTRY_NBR) )
  {
   if (table->vnf_table[i].used ==true )    //// tab[0] /tab[1] ??
    {  nb_entry ++;
       // voir s'il faut utiliser htonl() ??

      VLOG_WARN("kahina.get_table_content   entrée --->   %d",i);

	   data = ofpbuf_put_zeros(b, sizeof *data);

	   data->vnf_id= i;
	   VLOG_WARN("kahina : fn_name : get table centent:%s",table->vnf_table[i].fnct_name);
      strncpy(data->fnct_name,table->vnf_table[i].fnct_name,32);


	   data->param_length= sizeof(table->vnf_table[i].internal_parameters); // a corriger

     }
    i++;
   }

 }
 else
 {   // si table vide => ajouter padding
   pad = ofpbuf_put_zeros(b, sizeof *pad);  }
   ovs_mutex_unlock(&table->rwlock);
   ofconn_send_reply(ofconn, b);


}

/**** send ack or error messages to controller  *******/

void send_reply(struct ofconn *ofconn ,ovs_be32 xid,enum vnferror error)
{
	struct ofp12_experimenter_stats_header  *exph;
	struct ofp_header oh;
	struct ofpbuf *b;
	enum ofpraw raw;
	enum ofp_version version;
	uint64_t *description;

  enum ofputil_protocol protocol = ofconn_get_protocol(ofconn);
    version = ofputil_protocol_to_ofp_version(protocol);

    raw= OFPRAW_OFPT_EXPERIMENTER ;
    b = ofpraw_alloc_xid(raw, version, xid, 0);


	struct experim_headers *hdrs =b->base;



   //exph= ofpbuf_put_zeros(b, sizeof *exph);

   hdrs->exph.experimenter= EXPERIMENTER_ID;


	 if (!error)
	 hdrs->exph.exp_type = VNF_ACK ;
	 else hdrs->exph.exp_type =  VNF_ERROR ;

     description= ofpbuf_put_zeros(b, sizeof *description);

     description= (uint64_t)error;
	 ofconn_send_reply(ofconn, b);

}


/****************************************************/

enum vnferror get_entry_content(struct ofconn *ofconn ,ovs_be32 xid,uint32_t vnf_id)
{

 struct vnf_data_type1 *data;
 struct ofp12_experimenter_stats_header  *exph;
 uint64_t *pad;


 struct ofpbuf *b;
 enum ofpraw raw;
 enum ofp_version version;


struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
enum ofputil_protocol protocol = ofconn_get_protocol(ofconn);
struct vnftable *table; table =ofproto->vnf_table;

/**************************************************/


 if (vnf_id >ENTRY_NBR )return  VNFIDERR;


 version = ofputil_protocol_to_ofp_version(protocol);

  raw= OFPRAW_OFPT_EXPERIMENTER ;


 b = ofpraw_alloc_xid(raw, version, xid, 0);


 struct experim_headers *hdrs =b->base;
 hdrs->exph.experimenter= EXPERIMENTER_ID;
 hdrs->exph.exp_type=ENTRY_REPLY;


 if (table ==NULL)  return  POINTER_NULL;
 ovs_mutex_lock(&table->rwlock);

 if (table->vnf_table[vnf_id].used==false)
 { ovs_mutex_unlock(&table->rwlock);
   return ENTRERR_NOT_EXIST;
 }

 else
 {   ovs_mutex_lock(&table->rwlock);
	 data = ofpbuf_put_zeros(b, sizeof *data);
	 data->vnf_id= vnf_id;
	 strncpy(data->fnct_name,table->vnf_table[vnf_id].fnct_name,32);
     data->param_length= sizeof(&table->vnf_table[vnf_id].internal_parameters); // a corriger
     ovs_mutex_unlock(&table->rwlock);
     ofconn_send_reply(ofconn, b);
 }
return 0;
}

