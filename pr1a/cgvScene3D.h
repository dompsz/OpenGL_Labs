#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

/**
* Objects of this class represent 3D scenes for display
*/
class cgvScene3D {
public:
    const int SceneA = 1; ///< Internal identifier for scene A
    const int SceneB = 2; ///< Internal identifier for scene B
    const int SceneC = 3; ///< Internal identifier for scene C

    const char *Scene_NameA = "Scene A"; ///< Label for scene A
    const char *Scene_NameB = "Scene B"; ///< Label for scene B
    const char *Scene_NameC = "Scene C"; ///< Label for scene C
private:
    // Attributes
    bool axes = true; ///< Indicates whether or not to draw the coordinate axes
    int nStacksX=1;
    int nStacksY=1;
    int nStacksZ=1;

public:
    // Default constructors and destructor
    /// Default constructor
    cgvScene3D() = default;

    /// Destructor
    ~cgvScene3D() = default;

    // Methods
    // Method with OpenGL calls to display the scene
    void display(int scene);

    bool get_axes();

    void set_axes(bool _axes);

    void shoeBox();

    void incrStacksX();

    void decrStacksX();

    void incrStacksY();

    void decrStacksY();

    void incrStacksZ();

    void decrStacksZ();

private:
    void renderSceneA();

    void renderSceneB();

    void renderSceneC();

    void paint_axes();
};

#endif   // __IGVESCENA3D
