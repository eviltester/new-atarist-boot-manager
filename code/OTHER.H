#ifndef _OTHER_H
#define _OTHER_H

#include <stdio.h>
#include "vt52.h"
#include "vtwindow.h"

extern OTHER_FILES *create_other( void );
extern int valid_to_char( char ch );
extern int valid_from_char( char ch );
extern void new_other_key( VT52_WINDOW *,screen * );
extern void delete_other( VT52_WINDOW *, screen * );
extern LinkedList *dlte_other( LinkedList *, LinkedList *);
extern void edit_other( VT52_WINDOW *, screen * );

/*
extern void other_window( void );
extern int other_main_loop( int * );
extern void other_bottom_title( void );
extern OTHER_FILES *append_other_to_list( OTHER_FILES *, OTHER_FILES *);
extern void print_others_in_window( OTHER_FILES * );
extern void print_others_title( int );
extern void other_up_arrow( void );
extern void other_down_arrow( void );
extern void other_paging( void );
extern void other_paging_up( void );
extern void set_other_counts( void );
*/



#endif



