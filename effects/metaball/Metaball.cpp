#pragma once
#include "../../headers/Globals.h"
#include "../../headers/Timer.h"
#include "MetaballVariables.h"
#include "ShaderSource.h"
//Reference
/***
* https://arm-software.github.io/opengl-es-sdk-for-android/metaballs.html
* https://github.com/ARM-software/opengl-es-sdk-for-android/blob/master/samples/advanced_samples/Metaballs/jni/Native.cpp
***/

/** Id of vertex array object. */
GLuint  marching_cubes_triangles_vao_id = 0;

void calc_mvp(mat4& perspectiveProjectionMatrix);


void FirstSteppheresUpdaterShader() {
	//1st step initialize sphere updater shader 
	//shaders\Metaballs\Step1_metaball_sphere_updater_vertex_shader.s
	gSpheresUpdaterVertexShaderObject = CompileShader("E:\\RTR\\GITHUB\\OpenGLEffectsLibrary\\effects\\metaball\shaders\\Metaballs\\step1_metaball_sphere_updater_vertex_shader.s", GL_VERTEX_SHADER, spheres_updater_vert_shader);
	if (gSpheresUpdaterVertexShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball Sphere updater vertex shader\n");
	}
	gSpheresUpdaterFragmentShaderObject = CompileShader("shaders\\\Metaballs\\step1_sphere_updater_fragment_shader.s", GL_FRAGMENT_SHADER, spheres_updater_frag_shader);
	if (gSpheresUpdaterFragmentShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball Sphere updater fragment shader\n");
		return;
	}
	//provide source object to shader
	gSpheresUpdaterShaderProgramObject = glCreateProgram();
	glTransformFeedbackVaryings(gSpheresUpdaterShaderProgramObject, 1, &sphere_position_varying_name, GL_SEPARATE_ATTRIBS);
	//attach vertext shader to shader program
	glAttachShader(gSpheresUpdaterShaderProgramObject, gSpheresUpdaterVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(gSpheresUpdaterShaderProgramObject, gSpheresUpdaterFragmentShaderObject);

	LinkShader(gSpheresUpdaterShaderProgramObject);
	/* [Stage 1 Specifying input variables] */
	/* Get input uniform location. */
	spheres_updater_uniform_time_id = glGetUniformLocation(gSpheresUpdaterShaderProgramObject, spheres_updater_uniform_time_name);
	glUseProgram(gSpheresUpdaterShaderProgramObject);
	/* [Stage 1 Allocate buffer for output values] */
	/* Generate buffer object id. Define required storage space sufficient to hold sphere positions data. */
	glGenBuffers(1, &spheres_updater_sphere_positions_buffer_object_id);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, spheres_updater_sphere_positions_buffer_object_id);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, n_spheres * n_sphere_position_components * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	/* [Stage 1 Allocate buffer for output values] */
	/* [Stage 1 Transform feedback object initialization] */
	/* Generate and bind transform feedback object. */
	glGenTransformFeedbacks(1, &spheres_updater_transform_feedback_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id);
	/* Bind buffers to store calculated sphere positions. */
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, spheres_updater_sphere_positions_buffer_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	/* [Stage 1 Transform feedback object initialization] */

}



void InitializeMetaBallVertexUpdater() {
	void FirstSteppheresUpdaterShader();
	void LoadScalerFieldGenereratorShaders();
	void LoadMarchingCubesCellsShaders();
	void LoadMarchingCubesTriangleShaders();
	//Vertex shader
	//create shader
	 /* Store window width and height. */
//	window_width = WIN_WIDTH;
//	window_height = WIN_HEIGHT;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	FirstSteppheresUpdaterShader();
	LoadScalerFieldGenereratorShaders();
	LoadMarchingCubesCellsShaders();
	LoadMarchingCubesTriangleShaders();

	/* Enable facet culling, depth testing and specify front face for polygons. */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	timer.reset();

}


void LoadScalerFieldGenereratorShaders() {
	//2nd step Load scaler field generation shader 
	gScalerFieldVertexShaderObject = CompileShader("Shaders\\Metaballs\\step2_scalar_field_vertex_shader", GL_VERTEX_SHADER, scalar_field_vert_shader);
	if (gScalerFieldVertexShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball Scaler field vertex shader\n");
		return;
	}
	gScalerFieldFragmentShaderObject = CompileShader("Shaders\\Metaballs\\step2_scalar_field_fragment_shader", GL_FRAGMENT_SHADER, scalar_field_frag_shader);
	if (gScalerFieldFragmentShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball  Scaler field  fragment shader\n");
		return;
	}
	//provide source object to shader
	gScalerFieldShaderProgramObject = glCreateProgram();
	glTransformFeedbackVaryings(gScalerFieldShaderProgramObject, 1, &scalar_field_value_varying_name, GL_SEPARATE_ATTRIBS);
	//attach vertext shader to shader program
	glAttachShader(gScalerFieldShaderProgramObject, gScalerFieldVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(gScalerFieldShaderProgramObject, gScalerFieldFragmentShaderObject);

	LinkShader(gScalerFieldShaderProgramObject);

	/* Get input uniform locations. */
	scalar_field_uniform_samples_per_axis_id = glGetUniformLocation(gScalerFieldShaderProgramObject, scalar_field_uniform_samples_per_axis_name);
	scalar_field_uniform_spheres_id = glGetUniformBlockIndex(gScalerFieldShaderProgramObject, scalar_field_uniform_spheres_name);

	/* Activate scalar field generating program. */
	glUseProgram(gScalerFieldShaderProgramObject);

	/* Initialize uniforms constant throughout rendering loop. */
	glUniform1i(scalar_field_uniform_samples_per_axis_id, samples_per_axis);

	/* Set binding point for uniform block. */
	glUniformBlockBinding(gScalerFieldShaderProgramObject, scalar_field_uniform_spheres_id, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, spheres_updater_sphere_positions_buffer_object_id);

	/* Generate buffer object id. Define required storage space sufficient to hold scalar field data. */
	glGenBuffers(1, &scalar_field_buffer_object_id);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, scalar_field_buffer_object_id);

	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, samples_in_3d_space * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

	/* Generate and bind transform feedback object. */
	glGenTransformFeedbacks(1, &scalar_field_transform_feedback_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);

	/* Bind buffer to store calculated scalar field values. */
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, scalar_field_buffer_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	/* [Stage 2 Creating texture] */
	/* Generate texture object to hold scalar field data. */
	glGenTextures(1, &scalar_field_texture_object_id);

	/* Scalar field uses GL_TEXTURE_3D target of texture unit 1. */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, scalar_field_texture_object_id);

	/* Prepare texture storage for scalar field values. */
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, samples_per_axis, samples_per_axis, samples_per_axis);
	/* [Stage 2 Creating texture] */

	/* Tune texture settings to use it as a data source. */
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


void LoadMarchingCubesCellsShaders() {
	//3rd step initialize marching cubes cells shader shader 
	gMarchingCubesCellsVertexShaderObject = CompileShader("Shaders\\Metaballs\\step3_scalar_field_vertex_shader", GL_VERTEX_SHADER, marching_cubes_cells_vert_shader);
	if (gMarchingCubesCellsVertexShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball Scaler field vertex shader\n");
		return;
	}
	gMarchingCubesCellsFragmentShaderObject = CompileShader("Shaders\\Metaballs\\step3_scalar_field_fragment_shader", GL_FRAGMENT_SHADER, marching_cubes_cells_frag_shader);
	if (gMarchingCubesCellsFragmentShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball  Scaler field  fragment shader\n");
		return;
	}
	//provide source object to shader
	gMarchingCubesCellsShaderProgramObject = glCreateProgram();
	//attach vertext shader to shader program
	glAttachShader(gMarchingCubesCellsShaderProgramObject, gMarchingCubesCellsVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(gMarchingCubesCellsShaderProgramObject, gMarchingCubesCellsFragmentShaderObject);
	glTransformFeedbackVaryings(gMarchingCubesCellsShaderProgramObject, 1, &marching_cubes_cells_varying_name, GL_SEPARATE_ATTRIBS);
	LinkShader(gMarchingCubesCellsShaderProgramObject);

	/* Get input uniform locations. */
	marching_cubes_cells_uniform_cells_per_axis_id = glGetUniformLocation(gMarchingCubesCellsShaderProgramObject, marching_cubes_cells_uniform_cells_per_axis_name);
	marching_cubes_cells_uniform_scalar_field_sampler_id = glGetUniformLocation(gMarchingCubesCellsShaderProgramObject, marching_cubes_cells_uniform_scalar_field_sampler_name);
	marching_cubes_cells_uniform_isolevel_id = glGetUniformLocation(gMarchingCubesCellsShaderProgramObject, marching_cubes_cells_uniform_isolevel_name);

	/* Activate cell-splitting program. */
	glUseProgram(gMarchingCubesCellsShaderProgramObject);

	/* Initialize uniforms constant throughout rendering loop. */
	glUniform1i(marching_cubes_cells_uniform_cells_per_axis_id, cells_per_axis);
	glUniform1f(marching_cubes_cells_uniform_isolevel_id, isosurface_level);
	glUniform1i(marching_cubes_cells_uniform_scalar_field_sampler_id, 1);

	/* Generate buffer object id and allocate memory to store scalar field values. */
	glGenBuffers(1, &marching_cubes_cells_types_buffer_id);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, marching_cubes_cells_types_buffer_id);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, cells_in_3d_space * sizeof(GLint), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

	/* Generate and bind transform feedback object. */
	glGenTransformFeedbacks(1, &marching_cubes_cells_transform_feedback_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);

	/* Bind buffer to store calculated cell type data. */
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, marching_cubes_cells_types_buffer_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	/* [Stage 3 Creating texture] */
	/* Generate a texture object to hold cell type data. (We will explain why the texture later). */
	glGenTextures(1, &marching_cubes_cells_types_texture_object_id);

	/* Marching cubes cell type data uses GL_TEXTURE_3D target of texture unit 2. */
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, marching_cubes_cells_types_texture_object_id);

	/* Prepare texture storage for marching cube cell type data. */
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32I, cells_per_axis, cells_per_axis, cells_per_axis);
	/* [Stage 3 Creating texture] */

	/* Tune texture settings to use it as a data source. */
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void LoadMarchingCubesTriangleShaders() {
	//4th step initialize sphere updater shader 
	gMarchingCubesTrianglesVertexShaderObject = CompileShader("Shaders\\Metaballs\\step4_scalar_field_vertex_shader", GL_VERTEX_SHADER, marching_cubes_triangles_vert_shader);
	if (gMarchingCubesTrianglesVertexShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball Scaler field vertex shader\n");
		return;
	}
	gMarchingCubesTrianglesFragmentShaderObject = CompileShader("Shaders\\Metaballs\\step4_scalar_field_fragment_shader", GL_FRAGMENT_SHADER, marching_cubes_triangles_frag_shader);
	if (gMarchingCubesTrianglesFragmentShaderObject == 0)
	{
		fprintf(gpFile, "Failed to compile Metaball  Scaler field  fragment shader\n");
		return;
	}
	//provide source object to shader
	gMarchingCubesTrianglesShaderProgramObject = glCreateProgram();
	//attach vertext shader to shader program
	glAttachShader(gMarchingCubesTrianglesShaderProgramObject, gMarchingCubesTrianglesVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(gMarchingCubesTrianglesShaderProgramObject, gMarchingCubesTrianglesFragmentShaderObject);

	LinkShader(gMarchingCubesTrianglesShaderProgramObject);

	/* Get input uniform locations. */
	marching_cubes_triangles_uniform_time_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_time_name);
	marching_cubes_triangles_uniform_samples_per_axis_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_samples_per_axis_name);
	marching_cubes_triangles_uniform_isolevel_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_isolevel_name);
	marching_cubes_triangles_uniform_mvp_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_mvp_name);
	marching_cubes_triangles_uniform_cell_types_sampler_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_cell_types_sampler_name);
	marching_cubes_triangles_uniform_tri_table_sampler_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_tri_table_sampler_name);
	marching_cubes_triangles_uniform_scalar_field_sampler_id = glGetUniformLocation(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_scalar_field_sampler_name);
	marching_cubes_triangles_uniform_sphere_positions_id = glGetUniformBlockIndex(gMarchingCubesTrianglesShaderProgramObject, marching_cubes_triangles_uniform_sphere_positions_name);

	/* Activate triangle generating and rendering program. */
	glUseProgram(gMarchingCubesTrianglesShaderProgramObject);

	/* Initialize model view projection matrix. */
	calc_mvp(perspectiveProjectionMatrix);

	/* Initialize uniforms constant throughout rendering loop. */
	glUniform1f(marching_cubes_triangles_uniform_isolevel_id, isosurface_level);
	glUniform1i(marching_cubes_triangles_uniform_samples_per_axis_id, samples_per_axis);
	glUniform1i(marching_cubes_triangles_uniform_tri_table_sampler_id, 4);
	glUniform1i(marching_cubes_triangles_uniform_cell_types_sampler_id, 2);
	glUniform1i(marching_cubes_triangles_uniform_scalar_field_sampler_id, 1);
	glUniformMatrix4fv(marching_cubes_triangles_uniform_mvp_id, 1, GL_FALSE, perspectiveProjectionMatrix);

	/* Allocate memory for buffer */
	glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);

	/* Generate an Id for a texture object to hold look-up array data (tri_table). */
	glGenTextures(1, &marching_cubes_triangles_lookup_table_texture_id);

	/* Lookup array (tri_table) uses GL_TEXTURE_2D target of texture unit 4. */
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, marching_cubes_triangles_lookup_table_texture_id);

	/* Tune texture settings to use it as a data source. */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Load lookup table (tri_table) into texture. */
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, mc_vertices_per_cell, mc_cells_types_count);
	glTexSubImage2D(GL_TEXTURE_2D,        /* Use texture bound to GL_TEXTURE_2D               */
		0,                    /* Base image level                                 */
		0,                    /* From the texture origin                          */
		0,                    /* From the texture origin                          */
		mc_vertices_per_cell, /* Width will represent vertices in all 5 triangles */
		mc_cells_types_count, /* Height will represent cell type                  */
		GL_RED_INTEGER,       /* Texture will have only one component             */
		GL_INT,               /* ... of type int                                  */
		tri_table             /* Data will be copied directly from tri_table      */
	);

	/* Generate a vertex array object. We'll go with the explanation later. */
	glGenVertexArrays(1, &marching_cubes_triangles_vao_id);

	/* In OpenGL ES, draw calls require a bound vertex array object.
	 * Even though we're not using any per-vertex attribute data, we still need to bind a vertex array object.
	 */
	glBindVertexArray(marching_cubes_triangles_vao_id);

}

static int i = 0;
void RenderMetaballVertexUpdater() {
	if (i < 10000)
		i++;
	else
		i = 0;
	//model_time = i;
	model_time = timer.getTime();

	/* [Stage 1] */
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id);
	glEnable(GL_RASTERIZER_DISCARD);
	glUseProgram(gSpheresUpdaterShaderProgramObject);
	glUniform1f(spheres_updater_uniform_time_id, model_time);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, n_spheres);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


	// [Stage 2 Scalar field generation stage] 
	glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);
	glEnable(GL_RASTERIZER_DISCARD);
	{
		glUseProgram(gScalerFieldShaderProgramObject);
		glBeginTransformFeedback(GL_POINTS);
		{
			glDrawArrays(GL_POINTS, 0, samples_in_3d_space);
		}
		glEndTransformFeedback();
	}
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, scalar_field_buffer_object_id);
	glTexSubImage3D(GL_TEXTURE_3D,    // Use texture bound to GL_TEXTURE_3D                                    
		0,                // Base image level                                                       
		0,                // From the texture origin                                                
		0,                // From the texture origin                                                
		0,                // From the texture origin                                                
		samples_per_axis, // Texture have same width as scalar field in buffer                      
		samples_per_axis, // Texture have same height as scalar field in buffer                     
		samples_per_axis, // Texture have same depth as scalar field in buffer                      
		GL_RED,           // Scalar field gathered in buffer has only one component                 
		GL_FLOAT,         // Scalar field gathered in buffer is of float type                       
		NULL              // Scalar field gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target 
	);


	// 3. Stage 3: Marching cube algorithm cell splitting stage.
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);
	glEnable(GL_RASTERIZER_DISCARD);
	{
		glUseProgram(gMarchingCubesCellsShaderProgramObject);
		glBeginTransformFeedback(GL_POINTS);
		{
			glDrawArrays(GL_POINTS, 0, cells_in_3d_space);
		}
		glEndTransformFeedback();
	}
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, marching_cubes_cells_types_buffer_id);
	glTexSubImage3D(GL_TEXTURE_3D,  // Use texture bound to GL_TEXTURE_3D                                  
		0,              // Base image level                                                     
		0,              // From the texture origin                                              
		0,              // From the texture origin                                              
		0,              // From the texture origin                                              
		cells_per_axis, // Texture have same width as cells by width in buffer                  
		cells_per_axis, // Texture have same height as cells by height in buffer                
		cells_per_axis, // Texture have same depth as cells by depth in buffer                  
		GL_RED_INTEGER, // Cell types gathered in buffer have only one component                
		GL_INT,         // Cell types gathered in buffer are of int type                        
		NULL            // Cell types gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target 
	);

	
	// 4. Marching Cubes algorithm triangle generation stage.
	glActiveTexture(GL_TEXTURE0);

	glUseProgram(gMarchingCubesTrianglesShaderProgramObject);

	glUniform1f(marching_cubes_triangles_uniform_time_id, model_time);

	glDrawArrays(GL_TRIANGLES, 0, cells_in_3d_space * triangles_per_cell * vertices_per_triangle);

	/****
	
	// [Stage 2 Scalar field generation stage] 
	glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);
	glEnable(GL_RASTERIZER_DISCARD);
	{
		glUseProgram(gScalerFieldShaderProgramObject);
		glBeginTransformFeedback(GL_POINTS);
		{
			glDrawArrays(GL_POINTS, 0, samples_in_3d_space);
		}
		glEndTransformFeedback();
	}
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, scalar_field_buffer_object_id);
	glTexSubImage3D(GL_TEXTURE_3D,    // Use texture bound to GL_TEXTURE_3D                                    
		0,                // Base image level                                                       
		0,                // From the texture origin                                                
		0,                // From the texture origin                                                
		0,                // From the texture origin                                                
		samples_per_axis, // Texture have same width as scalar field in buffer                      
		samples_per_axis, // Texture have same height as scalar field in buffer                     
		samples_per_axis, // Texture have same depth as scalar field in buffer                      
		GL_RED,           // Scalar field gathered in buffer has only one component                 
		GL_FLOAT,         // Scalar field gathered in buffer is of float type                       
		NULL              // Scalar field gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target 
	);

	// 3. Stage 3: Marching cube algorithm cell splitting stage.
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);
	glEnable(GL_RASTERIZER_DISCARD);
	{
		glUseProgram(gMarchingCubesCellsShaderProgramObject);
		glBeginTransformFeedback(GL_POINTS);
		{
			glDrawArrays(GL_POINTS, 0, cells_in_3d_space);
		}
		glEndTransformFeedback();
	}
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, marching_cubes_cells_types_buffer_id);
	glTexSubImage3D(GL_TEXTURE_3D,  // Use texture bound to GL_TEXTURE_3D                                  
		0,              // Base image level                                                     
		0,              // From the texture origin                                              
		0,              // From the texture origin                                              
		0,              // From the texture origin                                              
		cells_per_axis, // Texture have same width as cells by width in buffer                  
		cells_per_axis, // Texture have same height as cells by height in buffer                
		cells_per_axis, // Texture have same depth as cells by depth in buffer                  
		GL_RED_INTEGER, // Cell types gathered in buffer have only one component                
		GL_INT,         // Cell types gathered in buffer are of int type                        
		NULL            // Cell types gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target 
	);

	glActiveTexture(GL_TEXTURE0);
	glUseProgram(gMarchingCubesTrianglesShaderProgramObject);
	glUniform1f(marching_cubes_triangles_uniform_time_id, model_time);
	// Run triangle generating and rendering program. 
	glDrawArrays(GL_TRIANGLES, 0, cells_in_3d_space * triangles_per_cell * vertices_per_triangle);
	
	*/
	
	// [Stage 4 Run triangle generating and rendering program] 

	// Start counting time. 
	//timer.reset();
}

void UninitializeMetaballVertexUpdater() {
	glDeleteVertexArrays(1, &marching_cubes_triangles_vao_id);
	glDeleteShader(gMarchingCubesTrianglesFragmentShaderObject);
	glDeleteShader(gMarchingCubesTrianglesVertexShaderObject);
	glDeleteProgram(gMarchingCubesTrianglesShaderProgramObject);
	glDeleteTextures(1, &marching_cubes_triangles_lookup_table_texture_id);
	glDeleteTextures(1, &marching_cubes_cells_types_texture_object_id);
	glDeleteTransformFeedbacks(1, &marching_cubes_cells_transform_feedback_object_id);
	glDeleteBuffers(1, &marching_cubes_cells_types_buffer_id);
	glDeleteShader(gMarchingCubesCellsFragmentShaderObject);
	glDeleteShader(gMarchingCubesCellsVertexShaderObject);
	glDeleteProgram(gMarchingCubesCellsShaderProgramObject);
	glDeleteTextures(1, &scalar_field_texture_object_id);
	glDeleteTransformFeedbacks(1, &scalar_field_transform_feedback_object_id);
	glDeleteBuffers(1, &scalar_field_buffer_object_id);
	glDeleteShader(gScalerFieldFragmentShaderObject);
	glDeleteShader(gScalerFieldVertexShaderObject);
	glDeleteProgram(gScalerFieldShaderProgramObject);
	glDeleteBuffers(1, &spheres_updater_sphere_positions_buffer_object_id);
	glDeleteShader(gSpheresUpdaterFragmentShaderObject);
	glDeleteShader(gSpheresUpdaterVertexShaderObject);
	glDeleteProgram(gSpheresUpdaterShaderProgramObject);
	if(spheres_updater_transform_feedback_object_id)
	glDeleteTransformFeedbacks(1, &spheres_updater_transform_feedback_object_id);
	//safe release changes
	/*if (gSpheresUpdaterShaderProgramObject) {
		glUseProgram(gSpheresUpdaterShaderProgramObject);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(gSpheresUpdaterShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(gSpheresUpdaterShaderProgramObject, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(gSpheresUpdaterShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(gSpheresUpdaterShaderProgramObject);
		gSpheresUpdaterShaderProgramObject = 0;
		glUseProgram(0);
	}*/
}


void calc_mvp(mat4& perspectiveProjectionMatrix)
{
	/* Define projection properties. */
	float degreesToRadiansCoefficient = atanf(1) / 45;                            /* Coefficient to recalculate degrees to radians.      */
	float frustum_fovy = 45.0f;                                    /* 45 degrees field of view in the y direction.        */
	float frustum_aspect = (float)window_width / (float)window_height; /* Aspect ratio.                                       */
	float frustum_z_near = 0.01f;                                    /* How close the viewer is to the near clipping plane. */
	float frustum_z_far = 100.0f;                                    /* How far the viewer is from the far clipping plane.  */
	float camera_distance = 6.5f;                                     /* Distance from camera to scene center.               */
	mat4 mat4_translate = vmath::translate(-0.5f, -0.5f, -50.95f);
	mat4 mat4_scale = vmath::scale(0.5f, 0.5f, 0.5f);
	/* Matrix that transforms the vertices from model space to world space. */
	/* Translate and scale coordinates from [0..1] to [-1..1] range for full visibility. */
	mat4 mat4_model_view = mat4_scale * mat4_translate  ;
	/* Pull the camera back from the scene center. */
	mat4_model_view[14] -= float(camera_distance);
	/* Create the perspective matrix from frustum parameters. */
	mat4 mat4_perspective = vmath::perspective(degreesToRadiansCoefficient * frustum_fovy, (GLfloat)window_width / (GLfloat)window_height, frustum_z_near, frustum_z_far);
	/* MVP (Model View Perspective) matrix is a result of multiplication of Perspective Matrix by Model View Matrix. */
	perspectiveProjectionMatrix = mat4_perspective * mat4_model_view;
}