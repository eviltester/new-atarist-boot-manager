#ifndef _BOOTWINS_H_
#define _BOOTWINS_H_

#include <stdio.h>
#include "vtwindow.h"
#include "other.h"
#include "sets.h"

typedef struct{
					char good_extension[4];
					char bad_extension[4];
					char search_extension[4];
					char from_path[FILENAME_MAX];
					char to_path[FILENAME_MAX];
					int action_type;	/* 1 - copy, 0 rename */
					/* 
						action type 0 will rename all highlit files in
						from_path to name.good_extension and all unlit
						to from_path\name.bad_extension.

						action type 1 will copy any highlit files to
						to_path.good_extension. for this reason it is 
						best that only one file be highlit so
						if window action type is 0 then only one file 
						may be highlit at any one time 
					*/
				}prog_win_extra_info;

#ifdef MEM_PRINT
extern void progs_win_bottom_title( VT52_WINDOW *, screen *);
extern prog_win_extra_info *create_prog_win_extra_info( void );
extern VT52_WINDOW *create_prog_win( void );
extern void print_prog_win_contents( VT52_WINDOW *vtwin, screen *, int * );
extern void print_specific_win_contents( VT52_WINDOW *, screen * );
extern void prog_win_toggle_key( VT52_WINDOW *, screen * );
extern long prog_win_other_keys( VT52_WINDOW *, long, screen * );
#else
extern void progs_win_bottom_title( VT52_WINDOW *);
extern prog_win_extra_info *create_prog_win_extra_info( void );
extern VT52_WINDOW *create_prog_win( void );
extern void print_prog_win_contents( VT52_WINDOW *vtwin, int * );
extern void print_specific_win_contents( VT52_WINDOW * );
extern void prog_win_toggle_key( VT52_WINDOW * );
extern long prog_win_other_keys( VT52_WINDOW *, long );
#endif

extern void sets_win_bottom_title( VT52_WINDOW *, screen *);
/*extern sets_win_extra_info *create_prog_win_extra_info( void );*/
extern VT52_WINDOW *create_sets_win( void );
extern void print_sets_win_contents( VT52_WINDOW *vtwin, screen *, int * );
extern void print_specific_set_win_contents( VT52_WINDOW *, screen * );
extern void sets_win_toggle_key( VT52_WINDOW *, screen * );
extern long sets_win_other_keys( VT52_WINDOW *, long, screen * );
extern void prog_win_in_focus( VT52_WINDOW *);
extern void prog_win_out_focus( VT52_WINDOW *);
extern void set_win_in_focus( VT52_WINDOW *);
extern void set_win_out_focus( VT52_WINDOW *);
extern void print_ordered_windows(LinkedList *, VT52_WINDOW *, screen *);
extern void set_curr_set_from_window( VT52_WINDOW *);
extern LinkedList *set_windows_from_set(VT52_WINDOW *);
extern void save_windows_to_set( VT52_WINDOW *);

extern void other_win_bottom_title( VT52_WINDOW *, screen *);
extern VT52_WINDOW *create_other_win( void );
extern void print_other_win_contents( VT52_WINDOW *vtwin, screen *, int * );
/*extern void print_specific_win_contents( VT52_WINDOW *, screen * );
extern void prog_win_toggle_key( VT52_WINDOW *, screen * );
*/
extern void other_win_in_focus( VT52_WINDOW *);
extern void other_win_out_focus( VT52_WINDOW *);
extern long other_win_other_keys( VT52_WINDOW *, long, screen * );
/*extern void new_other_window_key( VT52_WINDOW *, screen *);*/

extern LinkedList *set_default_windows(char *curr_path, SET *curr_set, LinkedList *sets);
extern LinkedList *circular_windows_list;	

#endif