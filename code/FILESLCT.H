#ifndef _VT52_FILE_SELECTOR_
#define _VT52_FILE_SELECTOR_

#include <stdio.h>
#include "lists.h"
#include <osbind.h>
#include "vt52.h"

extern char *drive_paths_array[];


extern int file_selector( 	int screen_width, int screen_height, 
									char *curr_drive, char *purpose,
									char *return_file_path);

extern LinkedList *read_files(	char *file_mask, 
											char *curr_path, short attr, 
											LinkedList *files,int *total_entries);

extern LinkedList * sort_files_by_type(LinkedList *files);

extern void display_files( 	int first_entry, short things_to_print,
							int cols, int col_width, LinkedList *files,
							int screen_height);

extern void col_calculation( int screen_width, short things_to_print, 
							 int *col_width, int *cols);

extern void remove_flst_cursor(int num_cols, int col_width, int cursor);
extern void draw_flst_cursor(int num_cols, int col_width, int cursor);
extern void print_flst_cursor(int num_cols, int col_width,char l, char r, int cursor);




#endif
