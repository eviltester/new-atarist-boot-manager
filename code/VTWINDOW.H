#ifndef _VTWINDOW_H
#define _VTWINDOW_H

#include <stdio.h>
#include "lists.h"

#define MEM_PRINT

extern short dirty_rectangle1[];
extern short dirty_rectangle2[];

typedef void (*VFP)();	/* pointer to function returning void */ 
typedef long (*LFP)(); 	/* pointer to function returning long */

typedef struct{
					int first_entry_on_screen;
					int logical_cursor;				/*position in list of entires*/
					int physical_cursor;				/* position on screen */
					int old_cursor;
					int on_top;				/* is window on top ? */					
					char *name;				/* window name */

					int height;				/* height in lines for window */
												/* does not include titles */
					int columns;			/* width in columns does not 
													include space between entries or
													window walls */
					int chars_per_col;	/* number of characters in each column */

					int titles_at_top;	/* boolean value for pos counts */
					int name_at_top;		/* boolean value for name pos */
					int titles_on_off;	/* boolean value for pos counts */
					int paging;				/* boolean value for page scrolling */
					
					int xpos;		/* xpos of top left corner */
					int ypos;		/* ypos of top left corner */

					int total_entries;	/* total_number of entries */
			
					int type;		/* window type - 1,PROG, 2,SETS, 3,OTHER */

					int can_resize_horizontal;
					int can_resize_vertical;
					int is_moveable;
					int is_scrollable_left;
					int is_scrollable_up;

					LinkedList *entries;	/* list of window entries */
	
					/* pointer to functions each window must implement */
					VFP help;		/* called when help key pressed */		
/*					VFP toggle;*/		/* called when toggle key pressed */
/*					VFP select;*/		/* called when select key pressed */
					VFP print_bottom; /* print text at bottom of screen */
					VFP part_print;	/* print part of the window */
					VFP print;		/* called when printing window contents */
					VFP focus;		/* called when window is in focus */
					VFP unfocus;	/* called when window goes out of focus */
					LFP other_keys;	/* called if unknown key is pressed */

					void *extra_info;	/* extra information depending upon */
											/* the window type */
					int max_selectable;	/* 0 means no limit */
					char *no_key_help1;
					char *no_key_help2;
					char *shift_key_help1;
					char *shift_key_help2;
					char *cntrl_key_help1;
					char *cntrl_key_help2;
					char *alt_key_help1;
					char *alt_key_help2;
					char *alt_shift_key_help1;
					char *alt_shift_key_help2;
					char *cntrl_shift_key_help1;
					char *cntrl_shift_key_help2;
				}VT52_WINDOW;


#ifdef MEM_PRINT
#include "screen.h"

extern VT52_WINDOW 	*create_vt52window( void );
extern void				delete_vt52window( VT52_WINDOW *);

extern void				print_window_numbers( VT52_WINDOW *, screen *, int *);
extern void				print_window_name( VT52_WINDOW *, screen *, int *);
extern void 			print_window_title_bar( VT52_WINDOW *, screen *, int *);
extern void 			print_window_bottom_bar( VT52_WINDOW *, screen *, int *);
extern void 			print_window_bar( VT52_WINDOW *, int, screen *, int *);
extern void 			print_window_sides( VT52_WINDOW * , int, screen *, int *);
extern void 			print_window_frame( VT52_WINDOW * , int, screen *, int *);
extern void 			print_window_contents( VT52_WINDOW *, screen *, int *);
extern void 			print_window_bottom_help( VT52_WINDOW *, screen *);

extern void				print_cursor( VT52_WINDOW *, char, char, int, screen *, int *);
extern void				remove_cursor(VT52_WINDOW *, int, screen *, int *);
extern void				draw_cursor(VT52_WINDOW *, int , screen *, int *);
extern void 			change_window_cols( VT52_WINDOW *, int, int);
extern void 			change_window_height( VT52_WINDOW *, int, int);
extern void 			move_window( VT52_WINDOW *, int, int, int, int);

extern void 			reset_cursors( VT52_WINDOW * );
extern void 			reset_cursors2( VT52_WINDOW * );

extern void				vt52window_up_arrow( VT52_WINDOW *, screen *);
extern void				vt52window_down_arrow( VT52_WINDOW *, screen *);
extern void				vt52window_right_arrow( VT52_WINDOW *, screen *);
extern void				vt52window_left_arrow( VT52_WINDOW *, screen *);
extern void				vt52window_paging_up( VT52_WINDOW *, screen *);
extern void				vt52window_paging(VT52_WINDOW *, screen *);


extern long 			window_key( VT52_WINDOW *, int, int, int, int, int, long , screen *);
extern long 			window_loop( VT52_WINDOW **, int, int, int, int, int , screen *);
extern void				print_window( VT52_WINDOW *);

#else

extern VT52_WINDOW 	*create_vt52window( void );
extern void				delete_vt52window( VT52_WINDOW *);

extern void				print_window_numbers( VT52_WINDOW *);
extern void				print_window_name( VT52_WINDOW *);
extern void 			print_window_title_bar( VT52_WINDOW *);
extern void 			print_window_bottom_bar( VT52_WINDOW *);
extern void 			print_window_bar( VT52_WINDOW *, int);
extern void 			print_window_sides( VT52_WINDOW * , int);
extern void 			print_window_frame( VT52_WINDOW * , int);
extern void 			print_window_contents( VT52_WINDOW *);
extern void 			print_window_bottom_help( VT52_WINDOW *);

extern void				print_cursor( VT52_WINDOW *, char, char, int);
extern void				remove_cursor(VT52_WINDOW *, int);
extern void				draw_cursor(VT52_WINDOW *, int );
extern void 			change_window_cols( VT52_WINDOW *, int, int);
extern void 			change_window_height( VT52_WINDOW *, int, int);
extern void 			move_window( VT52_WINDOW *, int, int, int, int);

extern void 			reset_cursors( VT52_WINDOW * );
extern void 			reset_cursors2( VT52_WINDOW * );

extern void				vt52window_up_arrow( VT52_WINDOW *);
extern void				vt52window_down_arrow( VT52_WINDOW *);
extern void				vt52window_right_arrow( VT52_WINDOW *);
extern void				vt52window_left_arrow( VT52_WINDOW *);
extern void				vt52window_paging_up( VT52_WINDOW *);
extern void				vt52window_paging(VT52_WINDOW *);


extern long 			window_key( VT52_WINDOW *, int, int, int, int, int, long, screen * );
extern long 			window_loop( VT52_WINDOW **, int, int, int, int, int, screen *);




extern void				set_total_entries( VT52_WINDOW *);
extern void				set_cursors( VT52_WINDOW *);
extern void				print_window( VT52_WINDOW *);

#endif

extern void set_up_window_name_frame(int *length, int *x, int *y, LinkedList *windows, screen *sp);
extern void print_window_name_frame(int x, int y, char *name, int length);
extern void print_area_under_frame(VT52_WINDOW *vtwin, screen *sp);
extern void print_dotted_frame(VT52_WINDOW *vtwin);

#endif
