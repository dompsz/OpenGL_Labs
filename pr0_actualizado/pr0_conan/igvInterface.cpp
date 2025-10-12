#include <cstdlib>
#include "igvInterface.h"
#include <math.h>
#include <vector>

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
    float radius = 3.0f;      // distance from origin
    float azimuth = 45.0f;    // horizontal angle (degrees)
    float elevation = 20.0f;  // vertical angle (degrees)

    float nearPlane = 0.1f;   // front clipping plane
    float farPlane  = 200.0f; // rear clipping plane

    bool perspective = true;  // toggle projection type
};


static bool cameraMode = false; // true = move camera, false = move object
static Camera cam;

enum class TransformType {
    TRANSLATE,
    ROTATE,
    SCALE
};

// Stores a single transformation operation
struct Transformation {
    TransformType type;
    float x, y, z; // Vector for translation or axis for rotation
    float value;   // Angle for rotation or factor for scaling
};

// buffer using a vector to store the sequence of transformations
static std::vector<Transformation> transformBuffer;
static bool bufferMode = false;

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

// Apply object's local rotation to a translation vector (convert local -> world)
void applyLocalTranslation(const ObjectState& o, float tx, float ty, float tz, float& outX, float& outY, float& outZ) {
    float rx = o.rx * M_PI / 180.0f;
    float ry = o.ry * M_PI / 180.0f;
    float rz = o.rz * M_PI / 180.0f;

    float cx = cos(rx), sx = sin(rx);
    float cy = cos(ry), sy = sin(ry);
    float cz = cos(rz), sz = sin(rz);

    // Z * Y * X rotation order (same as OpenGL)
    outX = cy * cz * tx + (-cy * sz) * ty + sy * tz;
    outY = (sx * sy * cz + cx * sz) * tx +
           (-sx * sy * sz + cx * cz) * ty +
           (-sx * cy) * tz;
    outZ = (-cx * sy * cz + sx * sz) * tx +
           (cx * sy * sz + sx * cz) * ty +
           (cx * cy) * tz;
}


void igvInterface::keyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: exit(1); break; // Escape
        case '1': selected = 0; break;
        case '2': selected = 1; break;
        case '3': selected = 2; break;

        // Toggle buffering mode
        case 'm':
            bufferMode = !bufferMode;
            if (bufferMode) {
                transformBuffer.clear();
                printf("Buffer ON\n");
            } else {
                printf("Buffer OFF\n");
            }
            break;

        // Apply stored RST buffer
        case 'M': {
            if (bufferMode || transformBuffer.empty()) break; // Only apply if not recording and buffer is not empty

            // Create a temporary matrix to apply the buffered transformations
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            // Apply each transformation from the buffer in order
            for (const auto& op : transformBuffer) {
                if (op.type == TransformType::TRANSLATE) {
                    glTranslatef(op.x, op.y, op.z);
                } else if (op.type == TransformType::ROTATE) {
                    glRotatef(op.value, op.x, op.y, op.z);
                } else if (op.type == TransformType::SCALE) {
                    glScalef(op.value, op.value, op.value);
                }
            }

            glTranslatef(obj[selected].tx, obj[selected].ty, obj[selected].tz);
            glRotatef(obj[selected].rx, 1, 0, 0);
            glRotatef(obj[selected].ry, 0, 1, 0);
            glRotatef(obj[selected].rz, 0, 0, 1);
            glScalef(obj[selected].scale, obj[selected].scale, obj[selected].scale);

            // Get the resulting matrix
            float mat[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, mat);
            glPopMatrix();

            obj[selected].tx = mat[12];
            obj[selected].ty = mat[13];
            obj[selected].tz = mat[14];

            // Extract scale
            float scaleX = sqrt(mat[0]*mat[0] + mat[1]*mat[1] + mat[2]*mat[2]);
            obj[selected].scale = scaleX;

            // Extract rotation
            obj[selected].ry = asin(-mat[2]) * 180.0 / M_PI;
            float cosY = cos(obj[selected].ry * M_PI / 180.0);
            obj[selected].rx = atan2(mat[6] / cosY, mat[10] / cosY) * 180.0 / M_PI;
            obj[selected].rz = atan2(mat[1] / cosY, mat[0] / cosY) * 180.0 / M_PI;

            printf("Applied buffered transform sequence\n");
            break;
        }

        // Translation Y
        case 'U':
            if (bufferMode) transformBuffer.push_back({TransformType::TRANSLATE, 0.0f, 0.1f, 0.0f});
            else { float dx, dy, dz; applyLocalTranslation(obj[selected], 0.0f, 0.1f, 0.0f, dx, dy, dz); obj[selected].tx += dx; obj[selected].ty += dy; obj[selected].tz += dz; }
            break;
        case 'u':
            if (bufferMode) transformBuffer.push_back({TransformType::TRANSLATE, 0.0f, -0.1f, 0.0f});
            else { float dx, dy, dz; applyLocalTranslation(obj[selected], 0.0f, -0.1f, 0.0f, dx, dy, dz); obj[selected].tx += dx; obj[selected].ty += dy; obj[selected].tz += dz; }
            break;

        // Rotation X
        case 'X':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 1, 0, 0, 2.0f});
            else obj[selected].rx += 2.0f;
            break;
        case 'x':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 1, 0, 0, -2.0f});
            else obj[selected].rx -= 2.0f;
            break;

        // Rotation Y
        case 'Y':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 0, 1, 0, 2.0f});
            else obj[selected].ry += 2.0f;
            break;
        case 'y':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 0, 1, 0, -2.0f});
            else obj[selected].ry -= 2.0f;
            break;

        // Rotation Z
        case 'Z':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 0, 0, 1, 2.0f});
            else obj[selected].rz += 2.0f;
            break;
        case 'z':
            if (bufferMode) transformBuffer.push_back({TransformType::ROTATE, 0, 0, 1, -2.0f});
            else obj[selected].rz -= 2.0f;
            break;

        // Scaling
        case 'S':
            if (bufferMode) transformBuffer.push_back({TransformType::SCALE, 0,0,0, 1.1f});
            else obj[selected].scale *= 1.1f;
            break;
        case 's':
            if (bufferMode) transformBuffer.push_back({TransformType::SCALE, 0,0,0, 1.0f / 1.1f});
            else obj[selected].scale /= 1.1f;
            break;

        // Camera zoom
        case '=': // zoom in
        case '+': // zoom in
            cam.radius -= 0.2f;
            if (cam.radius < 0.5f) cam.radius = 0.5f;
            break;
        case '-': // zoom out
            cam.radius += 0.2f;
            break;
        case 'c':
            cameraMode = !cameraMode;
            if (cameraMode)
                printf("Camera mode ON (arrow keys move camera)\n");
            else
                printf("Object mode ON (arrow keys move object)\n");
            break;

        // Clipping planes control
        case 'f': // move near plane farther from viewer
            cam.nearPlane += 0.1f;
            if (cam.nearPlane >= cam.farPlane - 0.5f)
                cam.nearPlane = cam.farPlane - 0.5f;
            printf("Near plane: %.2f\n", cam.nearPlane);
            break;

        case 'F': // move near plane closer to viewer
            cam.nearPlane -= 0.1f;
            if (cam.nearPlane < 0.01f) cam.nearPlane = 0.01f;
            printf("Near plane: %.2f\n", cam.nearPlane);
            break;

        case 'b': // move far plane farther from viewer
            cam.farPlane += 0.5f;
            printf("Far plane: %.2f\n", cam.farPlane);
            break;

        case 'B': // move far plane closer to viewer
            cam.farPlane -= 0.5f;
            if (cam.farPlane <= cam.nearPlane + 0.5f)
                cam.farPlane = cam.nearPlane + 0.5f;
            printf("Far plane: %.2f\n", cam.farPlane);
            break;

        // Projection type toggle
        case 'p':
        case 'P':
            cam.perspective = !cam.perspective;
            printf("Projection: %s\n", cam.perspective ? "Perspective" : "Orthographic");
            break;
    }

    glutPostRedisplay(); // refresh display
}

void igvInterface::specialFunc(int key, int x, int y) {
    if (cameraMode) {
        // Camera orbit movement
        switch (key) {
            case GLUT_KEY_UP:    cam.elevation += 2.0f; break;
            case GLUT_KEY_DOWN:  cam.elevation -= 2.0f; break;
            case GLUT_KEY_LEFT:  cam.azimuth -= 2.0f; break;
            case GLUT_KEY_RIGHT: cam.azimuth += 2.0f; break;
        }
    } else {
        // Object movement
        float local_dx = 0.0f, local_dz = 0.0f;

        switch (key) {
            case GLUT_KEY_UP:    local_dz = -0.1f; break;
            case GLUT_KEY_DOWN:  local_dz =  0.1f; break;
            case GLUT_KEY_LEFT:  local_dx = -0.1f; break;
            case GLUT_KEY_RIGHT: local_dx =  0.1f; break;
        }

        if (bufferMode) {
            // Record translation into buffer
            if (local_dx != 0.0f || local_dz != 0.0f) {
                transformBuffer.push_back({TransformType::TRANSLATE, local_dx, 0.0f, local_dz});
            }
        } else {
            // Apply translation
            float dx, dy, dz;
            applyLocalTranslation(obj[selected], local_dx, 0.0f, local_dz, dx, dy, dz);
            obj[selected].tx += dx;
            obj[selected].ty += dy;
            obj[selected].tz += dz;
        }
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

    // Projection setup
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;

    if (cam.perspective) {
        gluPerspective(60.0, aspect, cam.nearPlane, cam.farPlane);
    } else {
        float orthoSize = cam.radius; // roughly match zoom level
        glOrtho(-orthoSize * aspect, orthoSize * aspect,
                -orthoSize, orthoSize,
                cam.nearPlane, cam.farPlane);
    }


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

    // Section A: paint the axes
    glBegin(GL_LINES);

    // X axis - red
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glVertex3f( 20.0, 0.0, 0.0);

    // Y axis - green
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, -20.0, 0.0);
    glVertex3f(0.0,  20.0, 0.0);

    // Z axis - blue
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, -20.0);
    glVertex3f(0.0, 0.0,  20.0);

    glEnd();

    // Section C: object drawing
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
        glColor3f(1.0, 0.0, 0.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCube(1.0);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidCube(1.0);
    }
    else if (selected == 1) {
        // cone
        glColor3f(0.0, 1.0, 0.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCone(0.5, 1.0, 32, 32);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidCone(0.5, 1.0, 32, 32);
    }
    else if (selected == 2) {
        // sphere
        glColor3f(0.0, 0.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidSphere(0.5, 32, 32);
        // outlines
        prepare_outlines(1.0, 1.0, 1.0, 2.0f);
        glutSolidSphere(0.5, 32, 32);
    }

    // reset
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);

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
