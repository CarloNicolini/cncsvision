#include "glm.h"

GLfloat    scale;     /* original scale factor */
GLfloat    smoothing_angle = 90.0;  /* smoothing angle */
GLboolean  facet_normal = GL_FALSE; /* draw with facet normal? */
GLuint     material_mode = 2;   /* 0=none, 1=color, 2=material */

GLuint load_obj(char* mdl_name, GLMmodel* mdl)
{
    /* read in the model */
    mdl = glmReadOBJ(mdl_name);
    //scale = glmUnitize(model);
    glmVertexNormals(mdl, smoothing_angle, GL_TRUE);

    if (mdl->nummaterials > 0)
      material_mode = 2;

	/*
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat shininess = 65.0;
    */
    GLuint mode = 0;
/*
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    if (model_list)
  glDeleteLists(model_list, 1);

    /* generate a list */
    if (facet_normal)
  mode = GLM_FLAT;
    else
  mode = GLM_SMOOTH;
    if (material_mode == 1) {
  mode |= GLM_COLOR;
    } else if (material_mode == 2) {
  mode |= GLM_MATERIAL | GLM_TEXTURE;
    }
    //mdl_list = glmList(mdl, mode);

    return glmList(mdl, mode);
}

void draw_obj (GLuint obj)
{
	// display the texture
	glCallList(obj);
	// disable texture to avoid changing the drawing color based on the first pixel drawn
	// http://stackoverflow.com/questions/3405873/opengl-loading-a-texture-changes-the-current-color
	glDisable(GL_TEXTURE_2D);
}