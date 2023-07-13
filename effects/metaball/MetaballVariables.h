#include "../../headers/Globals.h"
#include "../../headers/Timer.h"

/* 1. Calculate sphere positions stage variable data. */
//shader objects
GLint gSpheresUpdaterVertexShaderObject;
GLint gSpheresUpdaterFragmentShaderObject;
GLint gSpheresUpdaterShaderProgramObject;
/* 2. Scalar field generation stage variable data. */
//shader objects
GLint gScalerFieldVertexShaderObject;
GLint gScalerFieldFragmentShaderObject;
GLint gScalerFieldShaderProgramObject;
/* 3. Marching Cubes cell-splitting stage variable data. */
GLint gMarchingCubesCellsVertexShaderObject;
GLint gMarchingCubesCellsFragmentShaderObject;
GLint gMarchingCubesCellsShaderProgramObject;
/* 4. Marching Cubes algorithm triangle generation and rendering stage variable data. */
/** Program object id for marching cubes algorthim's for rendering stage. */
GLint gMarchingCubesTrianglesVertexShaderObject;
GLint gMarchingCubesTrianglesFragmentShaderObject;
GLint gMarchingCubesTrianglesShaderProgramObject;

/*Stage 1 variables */
GLuint spheres_updater_sphere_positions_buffer_object_id = 0; /** Buffer object id to store calculated sphere positions. */
GLuint spheres_updater_transform_feedback_object_id = 0; /** Id of transform feedback object to keep sphere update stage buffer bindings. */
const GLchar* spheres_updater_uniform_time_name = "time"; /** Name of time uniform for sphere positions update stage. */
GLuint spheres_updater_uniform_time_id = 0; /** Location of time uniform for sphere positions update stage. */
const GLchar* sphere_position_varying_name = "sphere_position"; /** Sphere position output variable's name. */
/*Stage 2 variables*/
GLuint        scalar_field_buffer_object_id = 0; /** Buffer object id to store calculated values of scalar field. */
GLuint        scalar_field_transform_feedback_object_id = 0; /** Id of transform feedback object to keep scalar field buffer binding. */
const GLchar* scalar_field_uniform_samples_per_axis_name = "samples_per_axis"; /** Name of samples_per_axis uniform. */
GLuint        scalar_field_uniform_samples_per_axis_id = 0; /** Location of samples_per_axis uniform. */
const GLchar* scalar_field_uniform_spheres_name = "spheres_uniform_block"; /** Name of uniform block storing sphere data. */
GLuint        scalar_field_uniform_spheres_id = 0; /** Index of uniform block storing sphere data. */
const GLchar* scalar_field_value_varying_name = "scalar_field_value"; /** Scalar_field_value output variable's name. */
GLuint        scalar_field_texture_object_id = 0; /** Id of a 3D texture object storing scalar field data. */
/*Stage 3 variables*/
const GLchar* marching_cubes_cells_uniform_cells_per_axis_name = "cells_per_axis"; /** Name of cells_per_axis uniform. */
GLuint        marching_cubes_cells_uniform_cells_per_axis_id = 0; /** Location of cells_per_axis uniform. */
const GLchar* marching_cubes_cells_uniform_isolevel_name = "iso_level"; /** Name of iso_level uniform. */
GLuint        marching_cubes_cells_uniform_isolevel_id = 0; /** Location of iso_level uniform. */
const GLchar* marching_cubes_cells_uniform_scalar_field_sampler_name = "scalar_field"; /** Name of scalar_field uniform. */
GLuint        marching_cubes_cells_uniform_scalar_field_sampler_id = 0; /** Location of scalar_field uniform. */
const GLchar* marching_cubes_cells_varying_name = "cell_type_index"; /** Cell_type_index output variable's name. */
GLuint        marching_cubes_cells_transform_feedback_object_id = 0; /** Id of transform feedback object to keep cell types buffer binding. */
GLuint        marching_cubes_cells_types_buffer_id = 0; /** Id of a buffer object to hold result cell type data. */
GLuint        marching_cubes_cells_types_texture_object_id = 0; /** Id of a texture object to hold result cell type data. */
/*stage 4 variables*/
const GLchar* marching_cubes_triangles_uniform_samples_per_axis_name = "samples_per_axis"; /** Name of samples_per_axis uniform. */
GLuint        marching_cubes_triangles_uniform_samples_per_axis_id = 0; /** Location of samples_per_axis uniform. */
const GLchar* marching_cubes_triangles_uniform_isolevel_name = "iso_level"; /** Name of iso_level uniform. */
GLuint        marching_cubes_triangles_uniform_isolevel_id = 0; /** Location of iso_level uniform. */
const GLchar* marching_cubes_triangles_uniform_time_name = "time"; /** Name of time uniform. */
GLuint        marching_cubes_triangles_uniform_time_id = 0; /** Location of time uniform. */
const GLchar* marching_cubes_triangles_uniform_mvp_name = "mvp"; /** Name of mvp uniform. */
GLuint        marching_cubes_triangles_uniform_mvp_id = 0; /** Location of mvp uniform. */
const GLchar* marching_cubes_triangles_uniform_cell_types_sampler_name = "cell_types"; /** Name of cell_types uniform. */
GLuint        marching_cubes_triangles_uniform_cell_types_sampler_id = 0; /** Location of cell_types uniform. */
const GLchar* marching_cubes_triangles_uniform_scalar_field_sampler_name = "scalar_field"; /** Name of scalar_field uniform. */
GLuint        marching_cubes_triangles_uniform_scalar_field_sampler_id = 0; /** Location of scalar_field uniform. */
const GLchar* marching_cubes_triangles_uniform_sphere_positions_name = "sphere_positions_uniform_block"; /** Name of sphere_positions_uniform_block uniform block. */
GLuint        marching_cubes_triangles_uniform_sphere_positions_id = 0;/** Index of sphere_positions_uniform_block uniform block. */
const GLchar* marching_cubes_triangles_uniform_tri_table_sampler_name = "tri_table"; /** Name of tri_table uniform. */
GLuint        marching_cubes_triangles_uniform_tri_table_sampler_id = 0; /** Location of tri_table uniform. */
GLuint        marching_cubes_triangles_lookup_table_texture_id = 0; /** Id of a texture object to hold triangle look-up table data. */

/** Instance of a timer to measure time moments. */
MaliSDK::Timer timer;

/** Amount of spheres defining scalar field. This value should be synchronized between all files. */
const int n_spheres = 2;

/** Amount of components in sphere position varying. */
const int n_sphere_position_components = 4;

/** Matrix that transforms vertices from model space to perspective projected world space. */
mat4 mvp;


/* General metaballs example properties. */
GLfloat      model_time = 0.0f;  /**< Time (in seconds), increased each rendering iteration.                                         */
const GLuint tesselation_level = 70;    /**< Level of details you would like to split model into. Please use values from th range [8..256]. */
GLfloat      isosurface_level = 12.0f; /**< Scalar field's isosurface level.                                                               */
unsigned int window_width = 256;   /**< Window width resolution (pixels).                                                              */
unsigned int window_height = 256;   /**< Window height resolution (pixels).                                                             */

/* Marching Cubes algorithm-specific constants. */
const GLuint samples_per_axis = tesselation_level;                                      /**< Amount of samples we break scalar space into (per each axis). */
const GLuint samples_in_3d_space = samples_per_axis * samples_per_axis * samples_per_axis; /**< Amount of samples in 3D space. */
const GLuint cells_per_axis = samples_per_axis - 1;                                   /**< Amount of cells per each axis. */
const GLuint cells_in_3d_space = cells_per_axis * cells_per_axis * cells_per_axis;       /**< Amount of cells in 3D space. */
const GLuint vertices_per_triangle = 3;                                                      /**< Amount of vertices that defines one triangle. */
const GLuint triangles_per_cell = 5;                                                      /**< Amount of triangles that can be generated for a single cell by the Marching Cubes algorithm. */
const GLuint mc_vertices_per_cell = vertices_per_triangle * triangles_per_cell;             /**< Amount of vertices in tri_table representing triangles by vertices for one cell. */
const GLuint mc_cells_types_count = 256;

/** The array that is used for cell triangularization.
 *  Each row in table represents one cell type. Each cell type contains up to 5 triangles.
 *  Each triangle is defined by 3 sequential vertices.
 *  These vertices are "middle" points of the cell edges specified in this table.
 *  For example cell type 0 (see first line) does not define any triangles,
 *  while cell type 1 (see second line) defines one triangle consisting of "middle" points of edges 0,8 and 3 of a cell.
 *  "Middle" points are base points and can be moved closer to edge beginning point or edge ending point.
 *  Edge numeration is according to the Marching Cubes algorithm.
 *  There are exactly 256 cell types due to each vertex having only 2 states: it can be below isosurface or above.
 *  Thus (having 8 corners for each cubic cell) we have 2^8 = 256 cell types.
 *
 *  Table data taken from http://paulbourke.net/geometry/polygonise/
 */
 /* [tri_table chosen part for documentation] */
const GLint tri_table[mc_cells_types_count * mc_vertices_per_cell] =
{
  -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
   0,  8,  3,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
   0,  1,  9,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
   1,  8,  3,     9,  8,  1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
   /* [tri_table chosen part for documentation] */
	  1,  2, 10,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  3,     1,  2, 10,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9,  2, 10,     0,  2,  9,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  2,  8,  3,     2, 10,  8,    10,  9,  8,    -1, -1, -1,    -1, -1, -1,
	  3, 11,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0, 11,  2,     8, 11,  0,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  9,  0,     2,  3, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1, 11,  2,     1,  9, 11,     9,  8, 11,    -1, -1, -1,    -1, -1, -1,
	  3, 10,  1,    11, 10,  3,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0, 10,  1,     0,  8, 10,     8, 11, 10,    -1, -1, -1,    -1, -1, -1,
	  3,  9,  0,     3, 11,  9,    11, 10,  9,    -1, -1, -1,    -1, -1, -1,
	  9,  8, 10,    10,  8, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  7,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  3,  0,     7,  3,  4,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  1,  9,     8,  4,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  1,  9,     4,  7,  1,     7,  3,  1,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 10,     8,  4,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  4,  7,     3,  0,  4,     1,  2, 10,    -1, -1, -1,    -1, -1, -1,
	  9,  2, 10,     9,  0,  2,     8,  4,  7,    -1, -1, -1,    -1, -1, -1,
	  2, 10,  9,     2,  9,  7,     2,  7,  3,     7,  9,  4,    -1, -1, -1,
	  8,  4,  7,     3, 11,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 11,  4,  7,    11,  2,  4,     2,  0,  4,    -1, -1, -1,    -1, -1, -1,
	  9,  0,  1,     8,  4,  7,     2,  3, 11,    -1, -1, -1,    -1, -1, -1,
	  4,  7, 11,     9,  4, 11,     9, 11,  2,     9,  2,  1,    -1, -1, -1,
	  3, 10,  1,     3, 11, 10,     7,  8,  4,    -1, -1, -1,    -1, -1, -1,
	  1, 11, 10,     1,  4, 11,     1,  0,  4,     7, 11,  4,    -1, -1, -1,
	  4,  7,  8,     9,  0, 11,     9, 11, 10,    11,  0,  3,    -1, -1, -1,
	  4,  7, 11,     4, 11,  9,     9, 11, 10,    -1, -1, -1,    -1, -1, -1,
	  9,  5,  4,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9,  5,  4,     0,  8,  3,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  5,  4,     1,  5,  0,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  8,  5,  4,     8,  3,  5,     3,  1,  5,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 10,     9,  5,  4,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  0,  8,     1,  2, 10,     4,  9,  5,    -1, -1, -1,    -1, -1, -1,
	  5,  2, 10,     5,  4,  2,     4,  0,  2,    -1, -1, -1,    -1, -1, -1,
	  2, 10,  5,     3,  2,  5,     3,  5,  4,     3,  4,  8,    -1, -1, -1,
	  9,  5,  4,     2,  3, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0, 11,  2,     0,  8, 11,     4,  9,  5,    -1, -1, -1,    -1, -1, -1,
	  0,  5,  4,     0,  1,  5,     2,  3, 11,    -1, -1, -1,    -1, -1, -1,
	  2,  1,  5,     2,  5,  8,     2,  8, 11,     4,  8,  5,    -1, -1, -1,
	 10,  3, 11,    10,  1,  3,     9,  5,  4,    -1, -1, -1,    -1, -1, -1,
	  4,  9,  5,     0,  8,  1,     8, 10,  1,     8, 11, 10,    -1, -1, -1,
	  5,  4,  0,     5,  0, 11,     5, 11, 10,    11,  0,  3,    -1, -1, -1,
	  5,  4,  8,     5,  8, 10,    10,  8, 11,    -1, -1, -1,    -1, -1, -1,
	  9,  7,  8,     5,  7,  9,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9,  3,  0,     9,  5,  3,     5,  7,  3,    -1, -1, -1,    -1, -1, -1,
	  0,  7,  8,     0,  1,  7,     1,  5,  7,    -1, -1, -1,    -1, -1, -1,
	  1,  5,  3,     3,  5,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9,  7,  8,     9,  5,  7,    10,  1,  2,    -1, -1, -1,    -1, -1, -1,
	 10,  1,  2,     9,  5,  0,     5,  3,  0,     5,  7,  3,    -1, -1, -1,
	  8,  0,  2,     8,  2,  5,     8,  5,  7,    10,  5,  2,    -1, -1, -1,
	  2, 10,  5,     2,  5,  3,     3,  5,  7,    -1, -1, -1,    -1, -1, -1,
	  7,  9,  5,     7,  8,  9,     3, 11,  2,    -1, -1, -1,    -1, -1, -1,
	  9,  5,  7,     9,  7,  2,     9,  2,  0,     2,  7, 11,    -1, -1, -1,
	  2,  3, 11,     0,  1,  8,     1,  7,  8,     1,  5,  7,    -1, -1, -1,
	 11,  2,  1,    11,  1,  7,     7,  1,  5,    -1, -1, -1,    -1, -1, -1,
	  9,  5,  8,     8,  5,  7,    10,  1,  3,    10,  3, 11,    -1, -1, -1,
	  5,  7,  0,     5,  0,  9,     7, 11,  0,     1,  0, 10,    11, 10,  0,
	 11, 10,  0,    11,  0,  3,    10,  5,  0,     8,  0,  7,     5,  7,  0,
	 11, 10,  5,     7, 11,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 10,  6,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  3,     5, 10,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9,  0,  1,     5, 10,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  8,  3,     1,  9,  8,     5, 10,  6,    -1, -1, -1,    -1, -1, -1,
	  1,  6,  5,     2,  6,  1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  6,  5,     1,  2,  6,     3,  0,  8,    -1, -1, -1,    -1, -1, -1,
	  9,  6,  5,     9,  0,  6,     0,  2,  6,    -1, -1, -1,    -1, -1, -1,
	  5,  9,  8,     5,  8,  2,     5,  2,  6,     3,  2,  8,    -1, -1, -1,
	  2,  3, 11,    10,  6,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 11,  0,  8,    11,  2,  0,    10,  6,  5,    -1, -1, -1,    -1, -1, -1,
	  0,  1,  9,     2,  3, 11,     5, 10,  6,    -1, -1, -1,    -1, -1, -1,
	  5, 10,  6,     1,  9,  2,     9, 11,  2,     9,  8, 11,    -1, -1, -1,
	  6,  3, 11,     6,  5,  3,     5,  1,  3,    -1, -1, -1,    -1, -1, -1,
	  0,  8, 11,     0, 11,  5,     0,  5,  1,     5, 11,  6,    -1, -1, -1,
	  3, 11,  6,     0,  3,  6,     0,  6,  5,     0,  5,  9,    -1, -1, -1,
	  6,  5,  9,     6,  9, 11,    11,  9,  8,    -1, -1, -1,    -1, -1, -1,
	  5, 10,  6,     4,  7,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  3,  0,     4,  7,  3,     6,  5, 10,    -1, -1, -1,    -1, -1, -1,
	  1,  9,  0,     5, 10,  6,     8,  4,  7,    -1, -1, -1,    -1, -1, -1,
	 10,  6,  5,     1,  9,  7,     1,  7,  3,     7,  9,  4,    -1, -1, -1,
	  6,  1,  2,     6,  5,  1,     4,  7,  8,    -1, -1, -1,    -1, -1, -1,
	  1,  2,  5,     5,  2,  6,     3,  0,  4,     3,  4,  7,    -1, -1, -1,
	  8,  4,  7,     9,  0,  5,     0,  6,  5,     0,  2,  6,    -1, -1, -1,
	  7,  3,  9,     7,  9,  4,     3,  2,  9,     5,  9,  6,     2,  6,  9,
	  3, 11,  2,     7,  8,  4,    10,  6,  5,    -1, -1, -1,    -1, -1, -1,
	  5, 10,  6,     4,  7,  2,     4,  2,  0,     2,  7, 11,    -1, -1, -1,
	  0,  1,  9,     4,  7,  8,     2,  3, 11,     5, 10,  6,    -1, -1, -1,
	  9,  2,  1,     9, 11,  2,     9,  4, 11,     7, 11,  4,     5, 10,  6,
	  8,  4,  7,     3, 11,  5,     3,  5,  1,     5, 11,  6,    -1, -1, -1,
	  5,  1, 11,     5, 11,  6,     1,  0, 11,     7, 11,  4,     0,  4, 11,
	  0,  5,  9,     0,  6,  5,     0,  3,  6,    11,  6,  3,     8,  4,  7,
	  6,  5,  9,     6,  9, 11,     4,  7,  9,     7, 11,  9,    -1, -1, -1,
	 10,  4,  9,     6,  4, 10,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4, 10,  6,     4,  9, 10,     0,  8,  3,    -1, -1, -1,    -1, -1, -1,
	 10,  0,  1,    10,  6,  0,     6,  4,  0,    -1, -1, -1,    -1, -1, -1,
	  8,  3,  1,     8,  1,  6,     8,  6,  4,     6,  1, 10,    -1, -1, -1,
	  1,  4,  9,     1,  2,  4,     2,  6,  4,    -1, -1, -1,    -1, -1, -1,
	  3,  0,  8,     1,  2,  9,     2,  4,  9,     2,  6,  4,    -1, -1, -1,
	  0,  2,  4,     4,  2,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  8,  3,  2,     8,  2,  4,     4,  2,  6,    -1, -1, -1,    -1, -1, -1,
	 10,  4,  9,    10,  6,  4,    11,  2,  3,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  2,     2,  8, 11,     4,  9, 10,     4, 10,  6,    -1, -1, -1,
	  3, 11,  2,     0,  1,  6,     0,  6,  4,     6,  1, 10,    -1, -1, -1,
	  6,  4,  1,     6,  1, 10,     4,  8,  1,     2,  1, 11,     8, 11,  1,
	  9,  6,  4,     9,  3,  6,     9,  1,  3,    11,  6,  3,    -1, -1, -1,
	  8, 11,  1,     8,  1,  0,    11,  6,  1,     9,  1,  4,     6,  4,  1,
	  3, 11,  6,     3,  6,  0,     0,  6,  4,    -1, -1, -1,    -1, -1, -1,
	  6,  4,  8,    11,  6,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  7, 10,  6,     7,  8, 10,     8,  9, 10,    -1, -1, -1,    -1, -1, -1,
	  0,  7,  3,     0, 10,  7,     0,  9, 10,     6,  7, 10,    -1, -1, -1,
	 10,  6,  7,     1, 10,  7,     1,  7,  8,     1,  8,  0,    -1, -1, -1,
	 10,  6,  7,    10,  7,  1,     1,  7,  3,    -1, -1, -1,    -1, -1, -1,
	  1,  2,  6,     1,  6,  8,     1,  8,  9,     8,  6,  7,    -1, -1, -1,
	  2,  6,  9,     2,  9,  1,     6,  7,  9,     0,  9,  3,     7,  3,  9,
	  7,  8,  0,     7,  0,  6,     6,  0,  2,    -1, -1, -1,    -1, -1, -1,
	  7,  3,  2,     6,  7,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  2,  3, 11,    10,  6,  8,    10,  8,  9,     8,  6,  7,    -1, -1, -1,
	  2,  0,  7,     2,  7, 11,     0,  9,  7,     6,  7, 10,     9, 10,  7,
	  1,  8,  0,     1,  7,  8,     1, 10,  7,     6,  7, 10,     2,  3, 11,
	 11,  2,  1,    11,  1,  7,    10,  6,  1,     6,  7,  1,    -1, -1, -1,
	  8,  9,  6,     8,  6,  7,     9,  1,  6,    11,  6,  3,     1,  3,  6,
	  0,  9,  1,    11,  6,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  7,  8,  0,     7,  0,  6,     3, 11,  0,    11,  6,  0,    -1, -1, -1,
	  7, 11,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  7,  6, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  0,  8,    11,  7,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  1,  9,    11,  7,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  8,  1,  9,     8,  3,  1,    11,  7,  6,    -1, -1, -1,    -1, -1, -1,
	 10,  1,  2,     6, 11,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 10,     3,  0,  8,     6, 11,  7,    -1, -1, -1,    -1, -1, -1,
	  2,  9,  0,     2, 10,  9,     6, 11,  7,    -1, -1, -1,    -1, -1, -1,
	  6, 11,  7,     2, 10,  3,    10,  8,  3,    10,  9,  8,    -1, -1, -1,
	  7,  2,  3,     6,  2,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  7,  0,  8,     7,  6,  0,     6,  2,  0,    -1, -1, -1,    -1, -1, -1,
	  2,  7,  6,     2,  3,  7,     0,  1,  9,    -1, -1, -1,    -1, -1, -1,
	  1,  6,  2,     1,  8,  6,     1,  9,  8,     8,  7,  6,    -1, -1, -1,
	 10,  7,  6,    10,  1,  7,     1,  3,  7,    -1, -1, -1,    -1, -1, -1,
	 10,  7,  6,     1,  7, 10,     1,  8,  7,     1,  0,  8,    -1, -1, -1,
	  0,  3,  7,     0,  7, 10,     0, 10,  9,     6, 10,  7,    -1, -1, -1,
	  7,  6, 10,     7, 10,  8,     8, 10,  9,    -1, -1, -1,    -1, -1, -1,
	  6,  8,  4,    11,  8,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  6, 11,     3,  0,  6,     0,  4,  6,    -1, -1, -1,    -1, -1, -1,
	  8,  6, 11,     8,  4,  6,     9,  0,  1,    -1, -1, -1,    -1, -1, -1,
	  9,  4,  6,     9,  6,  3,     9,  3,  1,    11,  3,  6,    -1, -1, -1,
	  6,  8,  4,     6, 11,  8,     2, 10,  1,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 10,     3,  0, 11,     0,  6, 11,     0,  4,  6,    -1, -1, -1,
	  4, 11,  8,     4,  6, 11,     0,  2,  9,     2, 10,  9,    -1, -1, -1,
	 10,  9,  3,    10,  3,  2,     9,  4,  3,    11,  3,  6,     4,  6,  3,
	  8,  2,  3,     8,  4,  2,     4,  6,  2,    -1, -1, -1,    -1, -1, -1,
	  0,  4,  2,     4,  6,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  9,  0,     2,  3,  4,     2,  4,  6,     4,  3,  8,    -1, -1, -1,
	  1,  9,  4,     1,  4,  2,     2,  4,  6,    -1, -1, -1,    -1, -1, -1,
	  8,  1,  3,     8,  6,  1,     8,  4,  6,     6, 10,  1,    -1, -1, -1,
	 10,  1,  0,    10,  0,  6,     6,  0,  4,    -1, -1, -1,    -1, -1, -1,
	  4,  6,  3,     4,  3,  8,     6, 10,  3,     0,  3,  9,    10,  9,  3,
	 10,  9,  4,     6, 10,  4,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  9,  5,     7,  6, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  3,     4,  9,  5,    11,  7,  6,    -1, -1, -1,    -1, -1, -1,
	  5,  0,  1,     5,  4,  0,     7,  6, 11,    -1, -1, -1,    -1, -1, -1,
	 11,  7,  6,     8,  3,  4,     3,  5,  4,     3,  1,  5,    -1, -1, -1,
	  9,  5,  4,    10,  1,  2,     7,  6, 11,    -1, -1, -1,    -1, -1, -1,
	  6, 11,  7,     1,  2, 10,     0,  8,  3,     4,  9,  5,    -1, -1, -1,
	  7,  6, 11,     5,  4, 10,     4,  2, 10,     4,  0,  2,    -1, -1, -1,
	  3,  4,  8,     3,  5,  4,     3,  2,  5,    10,  5,  2,    11,  7,  6,
	  7,  2,  3,     7,  6,  2,     5,  4,  9,    -1, -1, -1,    -1, -1, -1,
	  9,  5,  4,     0,  8,  6,     0,  6,  2,     6,  8,  7,    -1, -1, -1,
	  3,  6,  2,     3,  7,  6,     1,  5,  0,     5,  4,  0,    -1, -1, -1,
	  6,  2,  8,     6,  8,  7,     2,  1,  8,     4,  8,  5,     1,  5,  8,
	  9,  5,  4,    10,  1,  6,     1,  7,  6,     1,  3,  7,    -1, -1, -1,
	  1,  6, 10,     1,  7,  6,     1,  0,  7,     8,  7,  0,     9,  5,  4,
	  4,  0, 10,     4, 10,  5,     0,  3, 10,     6, 10,  7,     3,  7, 10,
	  7,  6, 10,     7, 10,  8,     5,  4, 10,     4,  8, 10,    -1, -1, -1,
	  6,  9,  5,     6, 11,  9,    11,  8,  9,    -1, -1, -1,    -1, -1, -1,
	  3,  6, 11,     0,  6,  3,     0,  5,  6,     0,  9,  5,    -1, -1, -1,
	  0, 11,  8,     0,  5, 11,     0,  1,  5,     5,  6, 11,    -1, -1, -1,
	  6, 11,  3,     6,  3,  5,     5,  3,  1,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 10,     9,  5, 11,     9, 11,  8,    11,  5,  6,    -1, -1, -1,
	  0, 11,  3,     0,  6, 11,     0,  9,  6,     5,  6,  9,     1,  2, 10,
	 11,  8,  5,    11,  5,  6,     8,  0,  5,    10,  5,  2,     0,  2,  5,
	  6, 11,  3,     6,  3,  5,     2, 10,  3,    10,  5,  3,    -1, -1, -1,
	  5,  8,  9,     5,  2,  8,     5,  6,  2,     3,  8,  2,    -1, -1, -1,
	  9,  5,  6,     9,  6,  0,     0,  6,  2,    -1, -1, -1,    -1, -1, -1,
	  1,  5,  8,     1,  8,  0,     5,  6,  8,     3,  8,  2,     6,  2,  8,
	  1,  5,  6,     2,  1,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  3,  6,     1,  6, 10,     3,  8,  6,     5,  6,  9,     8,  9,  6,
	 10,  1,  0,    10,  0,  6,     9,  5,  0,     5,  6,  0,    -1, -1, -1,
	  0,  3,  8,     5,  6, 10,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 10,  5,  6,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 11,  5, 10,     7,  5, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 11,  5, 10,    11,  7,  5,     8,  3,  0,    -1, -1, -1,    -1, -1, -1,
	  5, 11,  7,     5, 10, 11,     1,  9,  0,    -1, -1, -1,    -1, -1, -1,
	 10,  7,  5,    10, 11,  7,     9,  8,  1,     8,  3,  1,    -1, -1, -1,
	 11,  1,  2,    11,  7,  1,     7,  5,  1,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  3,     1,  2,  7,     1,  7,  5,     7,  2, 11,    -1, -1, -1,
	  9,  7,  5,     9,  2,  7,     9,  0,  2,     2, 11,  7,    -1, -1, -1,
	  7,  5,  2,     7,  2, 11,     5,  9,  2,     3,  2,  8,     9,  8,  2,
	  2,  5, 10,     2,  3,  5,     3,  7,  5,    -1, -1, -1,    -1, -1, -1,
	  8,  2,  0,     8,  5,  2,     8,  7,  5,    10,  2,  5,    -1, -1, -1,
	  9,  0,  1,     5, 10,  3,     5,  3,  7,     3, 10,  2,    -1, -1, -1,
	  9,  8,  2,     9,  2,  1,     8,  7,  2,    10,  2,  5,     7,  5,  2,
	  1,  3,  5,     3,  7,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  7,     0,  7,  1,     1,  7,  5,    -1, -1, -1,    -1, -1, -1,
	  9,  0,  3,     9,  3,  5,     5,  3,  7,    -1, -1, -1,    -1, -1, -1,
	  9,  8,  7,     5,  9,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  5,  8,  4,     5, 10,  8,    10, 11,  8,    -1, -1, -1,    -1, -1, -1,
	  5,  0,  4,     5, 11,  0,     5, 10, 11,    11,  3,  0,    -1, -1, -1,
	  0,  1,  9,     8,  4, 10,     8, 10, 11,    10,  4,  5,    -1, -1, -1,
	 10, 11,  4,    10,  4,  5,    11,  3,  4,     9,  4,  1,     3,  1,  4,
	  2,  5,  1,     2,  8,  5,     2, 11,  8,     4,  5,  8,    -1, -1, -1,
	  0,  4, 11,     0, 11,  3,     4,  5, 11,     2, 11,  1,     5,  1, 11,
	  0,  2,  5,     0,  5,  9,     2, 11,  5,     4,  5,  8,    11,  8,  5,
	  9,  4,  5,     2, 11,  3,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  2,  5, 10,     3,  5,  2,     3,  4,  5,     3,  8,  4,    -1, -1, -1,
	  5, 10,  2,     5,  2,  4,     4,  2,  0,    -1, -1, -1,    -1, -1, -1,
	  3, 10,  2,     3,  5, 10,     3,  8,  5,     4,  5,  8,     0,  1,  9,
	  5, 10,  2,     5,  2,  4,     1,  9,  2,     9,  4,  2,    -1, -1, -1,
	  8,  4,  5,     8,  5,  3,     3,  5,  1,    -1, -1, -1,    -1, -1, -1,
	  0,  4,  5,     1,  0,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  8,  4,  5,     8,  5,  3,     9,  0,  5,     0,  3,  5,    -1, -1, -1,
	  9,  4,  5,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4, 11,  7,     4,  9, 11,     9, 10, 11,    -1, -1, -1,    -1, -1, -1,
	  0,  8,  3,     4,  9,  7,     9, 11,  7,     9, 10, 11,    -1, -1, -1,
	  1, 10, 11,     1, 11,  4,     1,  4,  0,     7,  4, 11,    -1, -1, -1,
	  3,  1,  4,     3,  4,  8,     1, 10,  4,     7,  4, 11,    10, 11,  4,
	  4, 11,  7,     9, 11,  4,     9,  2, 11,     9,  1,  2,    -1, -1, -1,
	  9,  7,  4,     9, 11,  7,     9,  1, 11,     2, 11,  1,     0,  8,  3,
	 11,  7,  4,    11,  4,  2,     2,  4,  0,    -1, -1, -1,    -1, -1, -1,
	 11,  7,  4,    11,  4,  2,     8,  3,  4,     3,  2,  4,    -1, -1, -1,
	  2,  9, 10,     2,  7,  9,     2,  3,  7,     7,  4,  9,    -1, -1, -1,
	  9, 10,  7,     9,  7,  4,    10,  2,  7,     8,  7,  0,     2,  0,  7,
	  3,  7, 10,     3, 10,  2,     7,  4, 10,     1, 10,  0,     4,  0, 10,
	  1, 10,  2,     8,  7,  4,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  9,  1,     4,  1,  7,     7,  1,  3,    -1, -1, -1,    -1, -1, -1,
	  4,  9,  1,     4,  1,  7,     0,  8,  1,     8,  7,  1,    -1, -1, -1,
	  4,  0,  3,     7,  4,  3,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  4,  8,  7,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  9, 10,  8,    10, 11,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  0,  9,     3,  9, 11,    11,  9, 10,    -1, -1, -1,    -1, -1, -1,
	  0,  1, 10,     0, 10,  8,     8, 10, 11,    -1, -1, -1,    -1, -1, -1,
	  3,  1, 10,    11,  3, 10,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  2, 11,     1, 11,  9,     9, 11,  8,    -1, -1, -1,    -1, -1, -1,
	  3,  0,  9,     3,  9, 11,     1,  2,  9,     2, 11,  9,    -1, -1, -1,
	  0,  2, 11,     8,  0, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  3,  2, 11,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  2,  3,  8,     2,  8, 10,    10,  8,  9,    -1, -1, -1,    -1, -1, -1,
	  9, 10,  2,     0,  9,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  2,  3,  8,     2,  8, 10,     0,  1,  8,     1, 10,  8,    -1, -1, -1,
	  1, 10,  2,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  1,  3,  8,     9,  1,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  9,  1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	  0,  3,  8,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,
	 -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1,    -1, -1, -1
};

