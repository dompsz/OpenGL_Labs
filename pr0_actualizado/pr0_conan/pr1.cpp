#include <cstdlib>

#include "igvInterface.h"


int main (int argc, char** argv) {
	// initializes the display window
    igvInterface::getInstance().configure_environment( argc, argv
                                                    , 500, 500 // window size
                                                    , 100, 100 // window position
                                                    , "CGIV: Practice 0" // window title
   );

	// sets the callback functions for event management
    igvInterface::getInstance().initialize_callbacks();

	// start the GLUT display loop
    igvInterface::getInstance().start_display_loop();

	return(0);
}
