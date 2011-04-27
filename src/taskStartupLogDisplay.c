/* -*- mode: c++; tab-width: 4; c-basic-offset: 4 -*- */
/*
 * taskStartupLogDisplay.c
 *
 * \brief This task displays the startup log line by line
 * with a specified delay after each line, and a 5x delay
 * at the end of the log.  This allows the startup code to
 * run without additional delays but permits the user to
 * see the sequence of startup event reports at a readable
 * rate.
 *
 *  Created on: 2011-04-26
 *      Author: Roger E Critchlow Jr, AD5DZ
 */


#include "widget.h"
#include "features.h"

/*! \brief scroll through a line of startup and go to sleep
 *
 * \retval none
 */
static void vtaskStartupLogDisplay( void * pcParameters )
{
  int current_line = -1;
  int startup_log_delay;

  if ( ! FEATURE_LOG_NONE ) {

    if ( FEATURE_LOG_1SEC )
      startup_log_delay = 1 * configTSK_LOGDISPLAY_PERIOD;
    else
      startup_log_delay = configTSK_LOGDISPLAY_PERIOD;

    widget_display_grab();
    widget_display_clear();

    while( 1 ) {
      char **buffer_lines;
      int lines;
    
      current_line += 1;
      // refetch the startup log each time, since it may get
      // longer while we're scrolling through it
      widget_get_startup_buffer_lines(&buffer_lines, lines);
      // if we're still working our way through the log,
      // display the next line, otherwise pause for 5 extra
      // delay periods at the end of the log
      if (current_line < lines) {
	widget_display_string_and_scroll(buffer_lines[current_line]);
      } else if (current_line > lines + 5) {
	break;
      }

      // Delay as defined in FreeRTOSConfig.h, initially 10ms
      vTaskDelay( startup_log_delay );
    }
  
    widget_display_drop();

    // delete ourself from the RTOS scheduler
    vTaskDelete(NULL);

    // just loop when we're done, we're not allowed to return
    // but it's not clear what we can do after deleting ourself
    while( 1 ) {
      vTaskDelay( 100 * startup_log_delay );
    }
      
  }
  
}


/*! \brief RTOS initialisation of the StartupLogDisplay task
 *
 * \retval none
 */
void vStartTaskStartupLogDisplay(void)
{
  xStatus = xTaskCreate( vtaskStartupLogDisplay,
			 configTSK_LOGDISPLAY_NAME,
			 configTSK_LOGDISPLAY_STACK_SIZE,
			 NULL, 
			 configTSK_LOGDISPLAY_PRIORITY,
                         (xTaskHandle *)NULL );
}

#endif
