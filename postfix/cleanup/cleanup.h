/*++
/* NAME
/*	cleanup 3h
/* SUMMARY
/*	canonicalize and enqueue message
/* SYNOPSIS
/*	#include "cleanup.h"
/* DESCRIPTION
/* .nf

 /*
  * Utility library.
  */
#include <vstring.h>
#include <vstream.h>
#include <argv.h>

 /*
  * Global library.
  */
#include <maps.h>
#include <tok822.h>
#include <been_here.h>
#include <mail_stream.h>

 /*
  * These state variables are accessed by many functions, and there is only
  * one instance of each per message.
  */
typedef struct CLEANUP_STATE {
    VSTRING *temp1;			/* scratch buffer, local use only */
    VSTRING *temp2;			/* scratch buffer, local use only */
    VSTREAM *src;			/* current input stream */
    VSTREAM *dst;			/* current output stream */
    MAIL_STREAM *handle;		/* mail stream handle */
    char   *queue_id;			/* queue file basename */
    time_t  time;			/* posting time */
    char   *fullname;			/* envelope sender full name */
    char   *sender;			/* envelope sender address */
    char   *from;			/* From: address */
    char   *resent_from;		/* Resent-From: address */
    char   *recip;			/* envelope recipient address */
    char   *return_receipt;		/* return-receipt address */
    char   *errors_to;			/* errors-to address */
    int     flags;			/* processing options */
    int     errs;			/* any badness experienced */
    int     err_mask;			/* allowed badness */
    VSTRING *header_buf;		/* multi-record header */
    int     headers_seen;		/* which headers were seen */
    int     hop_count;			/* count of received: headers */
    ARGV   *recipients;			/* recipients from regular headers */
    ARGV   *resent_recip;		/* recipients from resent headers */
    char   *resent;			/* any resent- header seen */
    BH_TABLE *dups;			/* recipient dup filter */
    long    warn_time;			/* cleanup_envelope.c */
    void    (*action) (struct CLEANUP_STATE *, int, char *, int);
    long    mesg_offset;		/* start of message segment */
    long    data_offset;		/* start of message content */
} CLEANUP_STATE;

 /*
  * Mappings.
  */
extern MAPS *cleanup_comm_canon_maps;
extern MAPS *cleanup_send_canon_maps;
extern MAPS *cleanup_rcpt_canon_maps;
extern MAPS *cleanup_header_checks;
extern MAPS *cleanup_virtual_maps;
extern ARGV *cleanup_masq_domains;

 /*
  * Restrictions on extension propagation.
  */
extern int cleanup_ext_prop_mask;

 /*
  * Saved queue file name, so the file can be removed in case of a fatal
  * run-time error.
  */
extern char *cleanup_path;

 /*
  * cleanup_state.c
  */
extern CLEANUP_STATE *cleanup_state_alloc(void);
extern void cleanup_state_free(CLEANUP_STATE *);

 /*
  * cleanup_api.c
  */
extern CLEANUP_STATE *cleanup_open(void);
extern void cleanup_control(CLEANUP_STATE *, int);
extern int cleanup_close(CLEANUP_STATE *);
extern void cleanup_all(void);

#define CLEANUP_RECORD(s, t, b, l)	((s)->action((s), (t), (b), (l)))

 /*
  * cleanup_out.c
  */
extern void cleanup_out(CLEANUP_STATE *, int, char *, int);
extern void cleanup_out_string(CLEANUP_STATE *, int, char *);
extern void cleanup_out_format(CLEANUP_STATE *, int, char *,...);

#define CLEANUP_OUT_BUF(s, t, b) \
	cleanup_out((s), (t), vstring_str((b)), VSTRING_LEN((b)))

#define CLEANUP_OUT_OK(s)	(((s)->errs & (s)->err_mask) == 0)

 /*
  * cleanup_envelope.c
  */
extern void cleanup_envelope_init(CLEANUP_STATE *, int, char *, int);
extern void cleanup_envelope_process(CLEANUP_STATE *, int, char *, int);

 /*
  * cleanup_message.c
  */
extern void cleanup_message_init(CLEANUP_STATE *, int, char *, int);
extern void cleanup_message_header(CLEANUP_STATE *, int, char *, int);
extern void cleanup_message_body(CLEANUP_STATE *, int, char *, int);

 /*
  * cleanup_extracted.c
  */
extern void cleanup_extracted_init(CLEANUP_STATE *, int, char *, int);
extern void cleanup_extracted_process(CLEANUP_STATE *, int, char *, int);

 /*
  * cleanup_rewrite.c
  */
extern void cleanup_rewrite_external(VSTRING *, const char *);
extern void cleanup_rewrite_internal(VSTRING *, const char *);
extern void cleanup_rewrite_tree(TOK822 *);

 /*
  * cleanup_map11.c
  */
extern void cleanup_map11_external(CLEANUP_STATE *, VSTRING *, MAPS *, int);
extern void cleanup_map11_internal(CLEANUP_STATE *, VSTRING *, MAPS *, int);
extern void cleanup_map11_tree(CLEANUP_STATE *, TOK822 *, MAPS *, int);

 /*
  * cleanup_map1n.c
  */
ARGV   *cleanup_map1n_internal(CLEANUP_STATE *, char *, MAPS *, int);

 /*
  * cleanup_masquerade.c
  */
extern void cleanup_masquerade_external(VSTRING *, ARGV *);
extern void cleanup_masquerade_internal(VSTRING *, ARGV *);
extern void cleanup_masquerade_tree(TOK822 *, ARGV *);

 /*
  * Cleanup_recipient.c
  */
extern void cleanup_out_recipient(CLEANUP_STATE *, char *);

/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/
