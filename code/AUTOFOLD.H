#ifndef _AUTO_FOLDER_H
#define _AUTO_FOLDER_H
#include <stdio.h>

#define PED

typedef struct{
					int auto_on;
					short work_in[11];			/* sets workstation */
					short work_out[57];			/* from open workstation */
					short extnd_work_out[57];	/* from workstation inquire */
					short handle;
					short rows;
					short columns;
					short char_pixel_width;
					short char_pixel_height;
					}ALS_ENVIRONMENT;

extern ALS_ENVIRONMENT auto_details;

#define IS_AUTO	auto_details.auto_on

extern void start_auto_environment( void );
extern void end_auto_environment( void );

#endif