#include <cstdlib>

#include "igvInterface.h"

#include <math.h>


// Application of the Singleton pattern
igvInterface* igvInterface::_instance = nullptr;

struct ObjectState {
    float tx = 0.0f, ty = 0.0f, tz = 0.0f; // translation
    float rx = 0.0f, ry = 0.0f, rz = 0.0f; // rotation
    float scale = 1.0f;                    // scale
};

static ObjectState obj[3]; // 3 objects to choose from
static int selected = 0;   // current object


struct Camera {
    float radius = 3.0f;    // distance from origin
    float azimuth = 45.0f;  // left-right angle in degrees
    float elevation = 20.0f; // up-down angle in degrees
};

static Camera cam;

// Public methods ----------------------------------------

/**
 * Method for accessing the single object of the class, in application of the Singleton design pattern.
 * @return A reference to the single object of the class.
 */
igvInterface& igvInterface::getInstance()
{  if ( !_instance )
   {  _instance = new igvInterface;
   }

   return *_instance;
}

/**
 * Initializes all parameters to create a display window.
 * @param argc Number of command line parameters when running the application.
 *             application.
 * @param argv Command line parameters when running the application.
 * @param _window_width Initial width of the display window.
 * @param _window_height Initial height of the display window.
 * @param _pos_X X coordinate of the initial position of the display window.
 *               display window
 * @param _pos_Y Y coordinate of the initial position of the
 *               display window
 * @param _title Title of the display window
 * @pre It is assumed that all parameters have valid values
 * @post Changes the height and width of the window stored in the object
 */
void igvInterface::configure_environment(int argc, char **argv, int _window_width, int _window_height, int _pos_X,
                                         int _pos_Y, std::string _title)
{
    // initialization of interface attributes
    window_width = _window_width;
    window_height = _window_height;

    // initialization of the display window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(_window_width,_window_height);
    glutInitWindowPosition(_pos_X,_pos_Y);
    glutCreateWindow(_title.c_str());

    glEnable(GL_DEPTH_TEST); // activates Z-buffer face culling
    glClearColor(0.0,0.0,0.0,0.0); // sets the window background color
}

/**
 * Method to display the scene and wait for events on the interface
 */
void igvInterface::start_display_loop()
{
    glutMainLoop(); // starts the GLUT display loop
}

/**
 * Method for controlling keyboard events
 * @param key Code of the key pressed
 * @param x X coordinate of the mouse cursor position at the time of the
 *          keyboard event
 * @param y Y coordinate of the mouse cursor position at the time of the
 *          keyboard event
 * @pre It is assumed that all parameters have valid values
 * @post The attribute that indicates whether the axes should be drawn or not can
 *       change
 */
void igvInterface::keyboardFunc(unsigned char key, int x, int y)
{  switch (key)
   {
    case 27: exit(1); break; //Escape
    case '1': selected = 0; break;
    case '2': selected = 1; break;
    case '3': selected = 2; break;
        // Translations
    case 'U': obj[selected].ty += 0.1f; break;
    case 'u': obj[selected].ty -= 0.1f; break;
    case 'X': obj[selected].rx += 2.0f; break;
    case 'x': obj[selected].rx -= 2.0f; break;
    case 'Y': obj[selected].ry += 2.0f; break;
    case 'y': obj[selected].ry -= 2.0f; break;
    case 'Z': obj[selected].rz += 2.0f; break;
    case 'z': obj[selected].rz -= 2.0f; break;
    case 'S': obj[selected].scale *= 1.1f; break;
    case 's': obj[selected].scale /= 1.1f; break;
        // Camera
    case '=': // zoom in
        cam.radius -= 0.2f;
        if (cam.radius < 0.5f) cam.radius = 0.5f; // prevent flipping through origin
        break;
    case '-': // zoom out
        cam.radius += 0.2f;
        break;

   }
   glutPostRedisplay(); // refreshes the contents of the view window
}

void igvInterface::specialFunc(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    cam.elevation += 2.0f; break;
        case GLUT_KEY_DOWN:  cam.elevation -= 2.0f; break;
        case GLUT_KEY_LEFT:  cam.azimuth -= 2.0f; break;
        case GLUT_KEY_RIGHT: cam.azimuth += 2.0f; break;
    }

    glutPostRedisplay();
}


/**
 * Method that defines the camera and viewport. It is called automatically
 * when the window size is changed.
 * @param w New window width
 * @param h New window height
 * @pre All parameters are assumed to have valid values
 */
void igvInterface::reshapeFunc(int w, int h)
{
    // resize the viewport to the new window width and height
   glViewport(0,0,(GLsizei) w,(GLsizei) h);

   // we save new values from the display window
   _instance->set_window_width( w );
   _instance->set_window_height( h );

   // sets the type of projection to use
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

    float aspect = (float)w / (float)h;
    gluPerspective(60.0, aspect, 0.1, 200.0); // perspective projection

    // back to modelview, ale BEZ gluLookAt
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Method for displaying the scene
 */

void prepare_outlines(float r, float g, float b, float width) {
    glColor3f(r,g,b);
    glLineWidth(width);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void igvInterface::displayFunc()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window and the Z-buffer
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // reset modelview
    float radA = cam.azimuth * M_PI / 180.0f;
    float radE = cam.elevation * M_PI / 180.0f;

    float eyeX = cam.radius * cos(radE) * cos(radA);
    float eyeY = cam.radius * sin(radE);
    float eyeZ = cam.radius * cos(radE) * sin(radA);

    gluLookAt(eyeX, eyeY, eyeZ,
              0.0, 0.0, 0.0,  // always look at origin
              0.0, 1.0, 0.0);

    glPushMatrix(); // saves the modeling matrix

    // Section A: paint the axes
    glBegin(GL_LINES);

    // X axis - red
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-2.0, 0.0, 0.0);
    glVertex3f( 2.0, 0.0, 0.0);

    // Y axis - green
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, -2.0, 0.0);
    glVertex3f(0.0,  2.0, 0.0);

    // Z axis - blue
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, -2.0);
    glVertex3f(0.0, 0.0,  2.0);

    glEnd();

    // // Section B: primitive cube in wireframe mode
    // glColor3f(0.0, 0.0, 0.0); // black cube
    // glutWireCube(1.0);        // cube centered at origin with size 1

    // Section C
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(obj[selected].tx, obj[selected].ty, obj[selected].tz);
    glRotatef(obj[selected].rx, 1, 0, 0);
    glRotatef(obj[selected].ry, 0, 1, 0);
    glRotatef(obj[selected].rz, 0, 0, 1);
    glScalef(obj[selected].scale, obj[selected].scale, obj[selected].scale);

    // object selection execution
    if (selected == 0) {
        // cube
        glColor3f(1.0, 0.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCube(1.0);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidCube(1.0);
    }
    else if (selected == 1) {
        // cone
        glColor3f(1.0, 0.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCone(0.5, 1.0, 32, 32);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidCone(0.5, 1.0, 32, 32);
    }
    else if (selected == 2) {
        // sphere
        glColor3f(1.0, 0.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidSphere(0.5, 32, 32);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidSphere(0.5, 32, 32);
    }

    // reset
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);

    // TODO: Section D: upper face with triangles


    glPopMatrix(); // restores the modeling matrix
    glutSwapBuffers(); // used instead of glFlush() to prevent flickering
}

/**
 * Method to initialize callbacks
 */
void igvInterface::initialize_callbacks()
{  glutKeyboardFunc(keyboardFunc);
   glutReshapeFunc(reshapeFunc);
   glutDisplayFunc(displayFunc);
   glutSpecialFunc(specialFunc); // register arrow keys
}

/**
 * Method to query the width of the display window
 * @return The value stored as the width of the display window
 */
int igvInterface::get_window_width()
{  return window_width;
}

/**
 * Method to query the height of the display window
 * @return The value stored as the height of the display window
 */
int igvInterface::get_window_height()
{  return window_height;
}

/**
 * Method to change the width of the display window
 * @param _window_width New value for the width of the display window
 * @pre It is assumed that the parameter has a valid value
 * @post The window width stored in the application changes to the new value
 */
void igvInterface::set_window_width(int _window_width)
{  window_width = _window_width;
}

/**
 * Method to change the height of the display window
 * @param _window_height New value for the height of the display window
 * @pre It is assumed that the parameter has a valid value
 * @post The window height stored in the application changes to the new value
 */
void igvInterface::set_window_height(int _window_height)
{
    window_height = _window_height;
}
