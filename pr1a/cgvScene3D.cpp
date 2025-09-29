#include <cstdlib>
#include <stdio.h>

#include "cgvScene3D.h"

/**
* Method for painting the coordinate axes by calling OpenGL functions
*/
void cgvScene3D::paint_axes ()
{
    GLfloat red[] = { 1,0,0,1.0 };
    GLfloat green[] = { 0,1,0,1.0 };
    GLfloat blue[] = { 0,0,1,1.0 };

    glBegin(GL_LINES);
    glMaterialfv(GL_FRONT, GL_EMISSION, red);
    glVertex3f(1000, 0, 0);
    glVertex3f(-1000, 0, 0);

    glMaterialfv(GL_FRONT, GL_EMISSION, green);
    glVertex3f(0, 1000, 0);
    glVertex3f(0, -1000, 0);

    glMaterialfv(GL_FRONT, GL_EMISSION, blue);
    glVertex3f(0, 0, 1000);
    glVertex3f(0, 0, -1000);
    glEnd();
}

void cgvScene3D::shoeBox() {
    GLfloat part_color[] = { 0,0.25,0 };
    GLfloat part_color2[] = { 0,0.3,0 };

    glMaterialfv(GL_FRONT, GL_EMISSION, part_color);

    glPushMatrix();
    glScalef(1, 1, 2);
    glutSolidCube(1);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, part_color2);
    glPushMatrix();
    glTranslatef(0, 0.4, 0);
    glScalef(1.1, 0.2, 2.1);
    glutSolidCube(1);
    glPopMatrix();
}

void cgvScene3D::incrStacksX() {
    nStacksX++;
};

void cgvScene3D::decrStacksX() {
    if (nStacksX > 1)
        nStacksX--;
};

void cgvScene3D::incrStacksY() {
    nStacksY++;
};

void cgvScene3D::decrStacksY() {
    if (nStacksY > 1)
        nStacksY--;
};

void cgvScene3D::incrStacksZ() {
    nStacksZ++;
};

void cgvScene3D::decrStacksZ() {
    if (nStacksZ > 1)
        nStacksZ--;
};

/**
* Method with OpenGL calls to display the scene
* @param scene Identifier of the scene type to draw
* @pre Assumes the parameter value is correct
*/
void cgvScene3D::display(int scene)
{
    // clear the window and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Lights
    GLfloat light0[] = { 10, 8, 9, 1 }; // point light source
    glLightfv(GL_LIGHT0, GL_POSITION, light0);
    glEnable(GL_LIGHT0);

    glPushMatrix(); // save the modeling matrix

    // paint the axes
    if(axes)
    { paint_axes();
    }

    // Scene selected via the menu (right-click)
    if(scene == SceneA)
    { renderSceneA();
    }
    else
    { if ( scene == SceneB )
        { renderSceneB();
        }
        else
        { if ( scene == SceneC )
            { renderSceneC();
            }
        }
    }

    glPopMatrix(); // restores the modeling matrix
    glutSwapBuffers(); // used instead of glFlush() to prevent flickering
}
/**
* Renders scene A by calling OpenGL functions
*/
void cgvScene3D::renderSceneA()
{
    shoeBox();
}

/**
* Paints scene B by calling OpenGL functions
*/
void cgvScene3D::renderSceneB ()
{
    GLfloat piece_color[] = { 0, 0, 0.5 };

    glMaterialfv(GL_FRONT, GL_EMISSION, piece_color);
    for (int yStack = 0; yStack < nStacksY; yStack++) {
        glPushMatrix();
        glTranslatef(0, yStack, 0);
        shoeBox();
        glPopMatrix();
    }
}

/**
* Paints scene C by calling OpenGL functions
*/
void cgvScene3D::renderSceneC ()
{
    GLfloat part_color[] = { 0,0,0.5 };
    GLfloat xSeparation = 1.5;
    GLfloat zSeparation = 2.5;

    glMaterialfv(GL_FRONT, GL_EMISSION, part_color);
    for (int yStacks = 0; yStacks < nStacksY; yStacks++) {
        for (int xStacks = 0; xStacks < nStacksX; xStacks++) {
            for (int zStacks = 0; zStacks < nStacksZ; zStacks++) {
                glPushMatrix();
                glTranslatef(xStacks * xSeparation, yStacks, zStacks * zSeparation);
                shoeBox();
                glPopMatrix();
            }
        }
    }
}

/**
* Method to check whether the axes should be drawn or not
* @retval true If the axes should be drawn
* @retval false If the axes should not be drawn
*/
bool cgvScene3D::get_axes()
{ return axes;
}

/**
* Method to enable or disable axes drawing
* @param _ejes Indicates whether the axes should be drawn (true) or not (false)
* @post The state of the object changes with respect to axes drawing,
* according to the value passed as a parameter
*/
void cgvScene3D::set_axes(bool _axes )
{ axes = _axes;
}




