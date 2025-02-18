typedef struct {
	size_t offset; // Offset in bytes from the start of the vertex
	const char *name; // Name of this attribute
	GLuint components; // Number of components of the vector
	GLenum type; // The type of each component
	GLboolean normalise; // Should an integer type be normalised?
} RoOpenGLProgramLayoutFeild;

typedef struct {
	size_t vertex_size; // Size of a full vertex (may be different from sum of
	                    // sizes due to padding and stuff)
	size_t feild_count; // Number of feilds
	const RoOpenGLProgramLayoutFeild *feilds;
} RoOpenGLProgramLayout;

typedef struct RoOpenGLProgram {
	const RoOpenGLProgramLayout *layout;
	GLuint vertex, fragment, program;
} RoOpenGLProgram;

static GLuint RoOpenGLLoadShaderFromSource(GLenum type, const char *source) {
	/**
	 * Load a shader from source.
	 * 
	 * @param source Source of the shader to compile
	 * @return shader handle on success, 0 on failure
	 */
	
	GLuint shader;
	GLint status;
	
	shader = glCreateShader(type);
	
	if (!shader) {
		return 0;
	}
	
	// We like to #define VERTEX or #define FRAGMENT based on the type
	const char *source_real[] = {
		"precision mediump float;\n",
		(type == GL_VERTEX_SHADER) ? "#define VERTEX\n\n" : "#define FRAGMENT\n\n",
		source,
	};
	
	// DgLog(DG_LOG_VERBOSE, "--------\n%s%s%s--------\n", source_real[0], source_real[1], source_real[2]);
	
	glShaderSource(shader, 3, source_real, NULL);
	
	glCompileShader(shader);
	
	// Get the status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
	if (!status) {
		GLint error_length = 0;
		
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);
		
		if (error_length > 0) {
			char *error = DgMemoryAllocate(error_length);
			
			if (!error) {
				DgLog(DG_LOG_ERROR, "Error while displaying shader compilation error message.");
				goto L_DeepError;
			}
			
			glGetShaderInfoLog(shader, error_length, NULL, error);
			
			DgLog(DG_LOG_ERROR, "Failed to compile shader:\n%s", error);
			
			DgMemoryFree(error);
		}
		else {
			DgLog(DG_LOG_ERROR, "Failed to compile shader but no log output was given.");
		}
		
		L_DeepError:
		glDeleteShader(shader);
		
		return 0;
	}
	
	return shader;
}

static DgError RoOpenGLProgramInit(RoOpenGLProgram *this, const char *source) {
	/**
	 * Initialise a new program with it's source code
	 */
	
	GLint status;
	
	this->vertex = RoOpenGLLoadShaderFromSource(GL_VERTEX_SHADER, source);
	this->fragment = RoOpenGLLoadShaderFromSource(GL_FRAGMENT_SHADER, source);
	
	this->program = glCreateProgram();
	
	if (!this->program) {
		return DG_ERROR_FAILED;
	}
	
	glAttachShader(this->program, this->vertex);
	glAttachShader(this->program, this->fragment);
	
	glLinkProgram(this->program);
	
	// Check status
	glGetProgramiv(this->program, GL_LINK_STATUS, &status);
	
	if (!status) {
		GLint error_length = 0;
		
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &error_length);
		
		if (error_length > 0) {
			char *error = DgMemoryAllocate(error_length);
			
			if (!error) {
				DgLog(DG_LOG_ERROR, "Error while displaying program link error message.");
				goto L_DeepError;
			}
			
			glGetProgramInfoLog(this->program, error_length, NULL, error);
			
			DgLog(DG_LOG_ERROR, "Failed to link program:\n%s", error);
			
			DgMemoryFree(error);
		}
		else {
			DgLog(DG_LOG_ERROR, "Failed to link program but no log output was given.");
		}
		
		L_DeepError:
		glDeleteProgram(this->program);
		
		return 0;
	}
	
	return DG_ERROR_SUCCESS;
}

static void RoOpenGLProgramFree(RoOpenGLProgram *this) {
	glDeleteProgram(this->program);
	glDeleteShader(this->vertex);
	glDeleteShader(this->fragment);
}

static DgError RoOpenGLProgramSetGlobalInt(RoOpenGLProgram *this, const char *name, GLint val) {
	// Clear error
	GLint err = glGetError();
	if (err != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "Not setting %s because of previous unhandled OpenGL error: <0x%x>", name, err);
		return DG_ERROR_FAILED;
	}
	
	// Get uniform location
	GLint location = glGetUniformLocation(this->program, name);
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "Failed to get uniform location for %s: <0x%x>", name, err);
		return DG_ERROR_FAILED;
	}
	
	// Set it
	glUseProgram(this->program);
	glUniform1i(location, val);
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "Failed to set global integer %s to %d: <0x%x>", name, val, err);
		return DG_ERROR_FAILED;
	}
	
	return DG_ERROR_SUCCESS;
}

// static void RoOpenGLProgramOn(RoOpenGLProgram *this) {
// 	/**
// 	 * Use a program
// 	 */
// 	
// 	glUseProgram(this->program);
// 	
// 	for (size_t i = 0; i < this->layout->feild_count; i++) {
// 		const RoOpenGLProgramLayoutFeild *f = &this->layout->feilds[i];
// 		
// 		GLint location = glGetAttribLocation(this->program, f->name);
// 		
// 		glVertexAttribPointer(
// 			location,
// 			f->components,
// 			f->type,
// 			f->normalise,
// 			this->layout->vertex_size - f->components,
// 			(void *) f->offset
// 		);
// 		
// 		glEnableVertexAttribArray(location);
// 	}
// }
