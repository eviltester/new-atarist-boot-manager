#include <vdi.h>
#include <aes.h>
#include <stdio.h>
#include <osbind.h>
#include "autofold.h"

/* a global to hold the auto environment details */

ALS_ENVIRONMENT auto_details;

/*******
*
*	start_auto_environment: 	void
*		sets up the auto_details and checks the workstation details
*		if it thinks it is running from an Auto folder
*		then it opens a physical workstation 
*
********/		
void start_auto_environment( void )
{

	short junk1,junk2,junk3,junk4;
	int aes_check;
	short width;
	short height;
	short testr[4];

	testr[0]=10;
	testr[1]=100;
	testr[2]=100;
	testr[3]=100;


	auto_details.work_in[0]=1;
	auto_details.work_in[1]=1;
	auto_details.work_in[2]=1;
	auto_details.work_in[3]=1;
	auto_details.work_in[4]=1;
	auto_details.work_in[5]=1;
	auto_details.work_in[6]=1;
	auto_details.work_in[7]=1;
	auto_details.work_in[8]=1;
	auto_details.work_in[9]=1;
	auto_details.work_in[10]=2;
	
	/* open an application */
	aes_check=appl_init();


	#ifdef PED
		printf("result of appl_init %d\n",aes_check);
	#endif

	/* get the handle for application */
	auto_details.handle=graf_handle(&junk1,&junk2,&junk3,&junk4);
	#ifdef PED
		printf("wchar %d hchar %d wbox %d hbos %d\n",junk1,junk2,junk3,junk4);
	#endif

	/* open a virtual workstation using handle */
	/*	v_opnvwk(	auto_details.work_in,
					&(auto_details.handle),
					auto_details.work_out);
	*/
	vq_extnd(auto_details.handle,0,auto_details.work_out);

	#ifdef PED
		printf("application handle = %d\n",auto_details.handle);
	#endif

	/* if we opened a virtual workstation, and we always seem to */
	/*	if(auto_details.handle)
	{*/
	#ifdef PED
			printf("opened virtual workstation\n");
			printf("width %d, height %d\n colours %d\n cols available %d\n",
						auto_details.work_out[0],auto_details.work_out[1],
						auto_details.work_out[13],auto_details.work_out[39]);
	#endif

		/* save these details for future use */
	width=auto_details.work_out[0];
	height=auto_details.work_out[1];

	#ifdef PED
		while(!Bconstat(2));
	#endif

	if(width==0 && height==0)	/* bollocks to the virtual */
	{
		/*v_clsvwk(auto_details.handle);*/	/* close that crap and */			

		/*	open a physical workstation */
		v_opnwk(	auto_details.work_in, 
					&(auto_details.handle), 
					auto_details.work_out);

		#ifdef PED
			printf(" physical handle = %d\n",auto_details.handle);
		#endif


		if(auto_details.handle)
		{

			auto_details.auto_on=1;

			#ifdef PED
				printf("width %d, height %d\n colours %d\n cols available %d\n",
						auto_details.work_out[0],auto_details.work_out[1],
						auto_details.work_out[13],auto_details.work_out[39]);
				printf("min char width %d, min char height %d\nmax char width %d, max char height %d\n",
						auto_details.work_out[45],auto_details.work_out[46],
						auto_details.work_out[47],auto_details.work_out[48]);
			#endif

			vq_extnd(auto_details.handle,1,auto_details.extnd_work_out);
			vq_chcells(auto_details.handle,&auto_details.rows,&auto_details.columns);

			auto_details.char_pixel_width=(auto_details.work_out[0]+1)/auto_details.columns;
			auto_details.char_pixel_height=(auto_details.work_out[1]+1)/auto_details.rows;
		
			printf("VDI TEST\n");
			while(!Bconstat(2));
			v_bar(auto_details.handle,testr);

			#ifdef PED
				printf(" rows %d, columns %d\n",auto_details.rows,auto_details.columns);
				printf(" planes %d\n",auto_details.extnd_work_out[4]);
				while(!Bconstat(2));
			#endif

		}
	}
	else		/* not an auto folder prog */
	{
		auto_details.auto_on=0;
		v_opnvwk(	auto_details.work_in,
					&(auto_details.handle),
					auto_details.work_out);
	
		vq_extnd(auto_details.handle,1,auto_details.extnd_work_out);
		vq_chcells(auto_details.handle,&auto_details.rows,&auto_details.columns);
		width=auto_details.work_out[0];
		height=auto_details.work_out[1];
		auto_details.char_pixel_width=(auto_details.work_out[0]+1)/auto_details.columns;
		auto_details.char_pixel_height=(auto_details.work_out[1]+1)/auto_details.rows;

			#ifdef PED
				printf(" rows %d, columns %d\n",auto_details.rows,auto_details.columns);
				printf(" planes %d\n",auto_details.extnd_work_out[4]);
				while(!Bconstat(2));
			#endif
	}

/*	}
	else
	{
		printf("INTERNAL ERROR: couldn't open application!!! sorry\n");
		printf("PRESS ANY KEY\n");
		while(!Bconstat(2));
	}*/

}

/*******
*
*	end_auto_environment: 	void
*		closes the virtual or physical workstation depending
*		on the value in auto_details.auto_on
*		and then closes the application
*
********/		
void end_auto_environment(	void )
{
	if(auto_details.auto_on==1)
	{
		v_clswk(auto_details.handle);
	}
	else
	{
		v_clsvwk(auto_details.handle);
	}

	appl_exit();
}



/* this is a test function to test the auto detection */

/*****************
main()
{


/*	appl_init();*/


	start_auto_environment();

	if(IS_AUTO)
		printf("we are in an auto folder\n");
	else
		printf("we are NOT in an autofolder\n");

	while(!Bconstat(2));

	end_auto_environment();


}
******************/