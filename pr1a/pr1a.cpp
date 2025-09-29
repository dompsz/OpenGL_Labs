#include <cstdlib>

#include "cgvInterface.h"

int main (int argc, char** argv)
{
    // initializes the display window
    cgvInterface::getInstance().configure_environment(argc, argv
            , 500, 500 // window size
            , 100, 100 // window position
            , "CG&V. Practice 2A" // window title
    );

// sets the callback functions for event handling
    cgvInterface::getInstance().initialize_callbacks();

// starts the OpenGL display loop
    cgvInterface::getInstance().start_display_loop();

    return(0);
}
