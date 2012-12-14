#ifndef _SETS_H
#define _SETS_H

#include <stdio.h>
#include "vt52.h"
#include "lists.h"
#include "vtwindow.h"
#include "screen.h"

extern void sets_window( void );
extern void print_sets_in_window( SET * );
extern void print_sets_title( int );
extern void sets_up_arrow( void );
extern void sets_down_arrow( void );
extern int	sets_main_loop( int * );
extern void  sets_bottom_title( void );
extern void set_set_from_accs( SET *, FILELIST *);
extern SMALLFILE *append_smallfile( SMALLFILE *, SMALLFILE *);
extern void set_accs_on_off_from_set( SET *, FILELIST *);
extern int acc_in_set( char *, SET *);
extern SET *append_set_to_list( SET *, SET *);
extern void sets_paging( void );
extern void sets_paging_up( void );

extern void set_window_on_off_from_set( SET *, VT52_WINDOW *);
extern void set_progs_on_off_from_set( LinkedList *, LinkedList *);
extern void set_set_details_from_window( SET *, VT52_WINDOW *);
extern LinkedList *set_set_from_progs( LinkedList *, LinkedList *);
extern int prog_in_set( char *, LinkedList *);
extern SET *create_set( void );
extern SET *copy_set(SET *);

extern void new_set_key( VT52_WINDOW *, screen *, SET * );
extern void edit_set( VT52_WINDOW *, screen * );
extern void delete_set( VT52_WINDOW *, screen *  );
extern LinkedList *dlte_set( LinkedList *, LinkedList * );
extern void delete_window_list(WindowList *wlp);
extern void delete_smallfile(SMALLFILE *sfp);





#endif







