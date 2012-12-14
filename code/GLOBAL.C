#include "vt52.h"
#include <osbind.h>
#include "lists.h"

int top_line_size;				/*title line*/
int top_gap;				   	/* gap between title and windows */
int mid_window_titles;			/* titles of windows */
int mid_window_bottom;			/* bottom */
int middle_gap;				/* gap between that and the other window*/
int bot_window_title;
int bot_window_bottom;
int bottom_gap;
int bottom_line_size;			/* size of bottom lines */
int extra_lines;

int left_border;
int right_border;
int auto_accs_gap;
int accs_sets_gap;

int title_row;
int mid_window_title_row;
int mid_window_first_item_row;
int other_window_title_row;
int other_window_first_item_row;
int bottom_title_row;

int auto_menu_col;
int accs_menu_col;
int sets_menu_col;


int total_accs;
int total_sets;
int total_progs;
int total_others;

int logical_progs_cursor;
int logical_accs_cursor;
int logical_sets_cursor;
int logical_other_cursor;

int progs_cursor;
int accs_cursor;
int sets_cursor;
int other_cursor;

int old_progs_cursor;
int old_accs_cursor;
int old_sets_cursor;
int old_other_cursor;

int current_window;
int old_window;

int first_prog_on_screen;
int first_acc_on_screen;
int first_set_on_screen;
int first_other_on_screen;

/* Date */
/* current time */
char altime[10];
char aldate[10];

DEFAULTS prog_defaults;
RES_DEF	resolutions[10];

char *line_buffer;

LinkedList *prgs;
LinkedList *accs;

LinkedList *sets;
SET *curr_set;

OTHER_FILES *others;
OTHER_FILES *curr_other;

int oconterm;

int conset(void)
{
	oconterm=*(char *)0x484;
	*(char *)0x484|=1<<3;
}

int conunset(void)
{
	*(char *)0x484=oconterm;
}

void get_key( int *shift, int *upper, int *lower)
{

	long k;
	

	k=Bconin(2);
	*upper=(int)(k>>16)&0xff;
	*lower=(int)(k%256);
	*shift=(int)(k>>24)&0xff;
}