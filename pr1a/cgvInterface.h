#ifndef __IGVINTERFAZ
#define __IGVINTERFAZ

#if defined(__APPLE__) && defined(__MACH__)

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#endif   // defined(__APPLE__) && defined(__MACH__)

#include <string>
#include "cgvScene3D.h"

/**
* Objects of this class encapsulate the interface and state of the application.
*/
class cgvInterface
{ private:
    // Attributes
    int window_width = 0; ///< Width of the display window
    int window_height = 0; ///< Height of the display window

    cgvScene3D scene; ///< Scene to display in the window defined by cgvInterface

    int menuSelection = 0; ///< Last selected menu item

    // Implementing the Singleton pattern
    static cgvInterface* _instance; ///< Pointer to the singleton object of the class
    cgvInterface();

public:
    static cgvInterface& getInstance();

    /// Destroyer
    ~cgvInterface() = default;
    // Static methods
    // Event callbacks
    static void keyboardFunc ( unsigned char key, int x, int y ); // Method for handling keyboard events
    static void reshapeFunc ( int w, int h ); // Method that defines the camera and viewport
    // Automatically called when the window is resized
    static void displayFunc (); // Method for displaying the scene
    static void menuHandle(int value); // method to manage menu option selection

    // Methods
    // initializes all parameters to create a display window
    void configure_environment ( int argc, char** argv // main parameters
            , int _window_width, int _window_height // width and height of the display window
            , int _pos_X, int _pos_Y // initial position of the display window
            , std::string _title // title of the display window
    );
    void create_menu(); // Creates a menu that is handled with the right mouse button.

    void initialize_callbacks(); // Initializes all callbacks

    void start_display_loop(); // Displays the scene and waits for events on the interface

    // Get_ and set_ methods for accessing attributes
    int get_window_width();
    int get_window_height();

    void set_window_width(int _window_width);
    void set_window_height(int _window_height);
};

#endif   // __IGVINTERFAZ
