#include <cstdlib>

#include "cgvInterface.h"

// Singleton Pattern Application
cgvInterface* cgvInterface::_instance = nullptr;

// Constructor Methods -----------------------------------

/**
* Default constructor
*/
cgvInterface::cgvInterface(): menuSelection(scene.SceneA)
{}

// Public methods ----------------------------------------

/**
* Method to access the class's singleton object, applying the Singleton
* design pattern
* @return A reference to the class's singleton object
*/
cgvInterface& cgvInterface::getInstance()
{ if ( !_instance )
    { _instance = new cgvInterface;
    }

    return *_instance;
}

/**
* Initializes all parameters to create a display window
* @param argc Number of parameters per command line when running the
* application
* @param argv Parameters per command line when running the application
* @param _window_width Initial width of the display window
* @param _window_height Initial height of the display window
* @param _pos_X X coordinate of the initial position of the display * window
* @param _pos_Y Y coordinate of the initial position of the display * window
* @param _title Title of the display window
* @pre All parameters are assumed to be Parameters have valid values
* @post Changes the height and width of the window stored in the object
*/
void cgvInterface::configure_environment (int argc, char** argv
        , int _window_width, int _window_height
        , int _x_pos, int _y_pos
        , std::string _title )
{ // initialize interface attributes
    window_width = _window_width;
    window_height = _window_height;

// initialize the display window
    glutInit ( &argc, argv );
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize ( _window_width, _window_height );
    glutInitWindowPosition ( _x_pos, _y_pos );
    glutCreateWindow( _title.c_str() );

    create_menu();

    glEnable( GL_DEPTH_TEST ); // enable z-buffer surface hiding
    glClearColor( 1.0, 1.0, 1.0, 0.0 ); // set the window background color

    glEnable( GL_LIGHTING ); // enable scene lighting
    glEnable( GL_NORMALIZE ); // normalize normal vectors for lighting calculations
}

/**
* Creates a menu bound to the right mouse button
*/
void cgvInterface::create_menu ()
{ int menu_id = glutCreateMenu ( menuHandle );
    glutAddMenuEntry ( _instance->scene.Scene_NameA
            , _instance->scene.SceneA );
    glutAddMenuEntry ( _instance->scene.Scene_NameB
            , _instance->scene.SceneB );
    glutAddMenuEntry ( _instance->scene.Scene_NameC
            , _instance->scene.SceneC );

    glutAttachMenu ( GLUT_RIGHT_BUTTON );
}

/**
* Method to display the scene and wait for events on the interface
*/
void cgvInterface::start_display_loop()
{ glutMainLoop(); // starts the GLUT display loop
}

/**
* Method for handling keyboard events
* @param key Code of the key pressed
* @param x X coordinate of the mouse cursor position at the time of the
* keyboard event
* @param y Y coordinate of the mouse cursor position at the time of the
* keyboard event
* @pre All parameters are assumed to have valid values
* @post The scene may change depending on the key pressed
*/
void cgvInterface::keyboardFunc (unsigned char key, int x, int y)
{ switch ( key )
    { case 'e': // toggle the display of the axes
            _instance->scene.set_axes(_instance->scene.get_axes() ? false : true);
            break;
        case 'E': // toggle the display of the axes
            _instance->scene.set_axes(_instance->scene.get_axes() ? false : true);
            break;
        case 'X':
            _instance->scene.incrStacksX();
            break;
        case 'x':
            _instance->scene.decrStacksX();
            break;
        case 'Y':
            _instance->scene.incrStacksY();
            break;
        case 'y':
            _instance->scene.decrStacksY();
            break;
        case 'Z':
            _instance->scene.incrStacksZ();
            break;
        case 'z':
            _instance->scene.decrStacksZ();
            break;
        case 27: // escape key to EXIT
            exit ( 1 );
            break;
    }
    glutPostRedisplay (); // refresh the contents of the viewport
}

/**
* Method that defines the camera and viewport. Called automatically
* when the window is resized.
* @param w New window width
* @param h New window height
* @pre All parameters are assumed to have valid values
*/
void cgvInterface::reshapeFunc (int w, int h)
{ // reshape the viewport to the new window width and height
    glViewport ( 0, 0, (GLsizei) w, (GLsizei) h );

// save the new viewport values
    _instance->set_window_width ( w );
    _instance->set_window_height ( h );

// sets the projection type to use
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-1*5, 1*5, -1*5, 1*5, -1*5, 200);

// define the view camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(1.5, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // perspective view
// gluLookAt(1.5, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // plan view from the positive X axis
}

/**
* Method for displaying the scene
*/
void cgvInterface::displayFunc ()
{ _instance->scene.display( _instance->menuSelection );
}

/**
* Method for managing menu option selection
* @param value New selected option
* @pre Assumes the parameter value is correct
* @post Stores the selected option in the object
*/
void cgvInterface::menuHandle (int value )
{ _instance->menuSelection = value;
    glutPostRedisplay (); // renew the contents of the window
}

/**
* Method to initialize callbacks
*/
void cgvInterface::initialize_callbacks()
{ glutKeyboardFunc ( keyboardFunc );
    glutReshapeFunc ( reshapeFunc );
    glutDisplayFunc ( displayFunc );
}

/**
* Method to query the display window width
* @return The value stored as the display window width
*/
int cgvInterface::get_window_width ()
{ return window_width;
}

/**
* Method to query the height of the display window
* @return The value stored as the height of the display window
*/
int cgvInterface::get_window_height ()
{ return window_height;
}

/**
* Method to change the width of the display window
* @param _window_width New value for the width of the display window
* @pre Assumes the parameter has a valid value
* @post The window width stored in the application is changed to the new value
*/
void cgvInterface::set_window_width (int _window_width )
{ window_width = _window_width;
}

/**
* Method to change the display window height
* @param _window_height New value for the display window height
* @pre Assumes the parameter has a valid value
* @post The window height stored in the application is changed to the new value
*/
void cgvInterface::set_window_height (int _window_height )
{ window_height = _window_height;
}
