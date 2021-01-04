#ifndef _JIGSAW_PIPELINE_H_
#define _JIGSAW_PIPELINE_H_

namespace Jigsaw {
	namespace Pipeline {
		/// /// <summary>
		/// Enum that specifies a specific Jigsaw Graphics Pipeline 
		/// </summary>
		enum JIGSAW_PIPELINE {
			JIGSAW_PIPELINE_DRAW
		};

		/// <summary>
		/// Enum that specifies a specific Jigsaw Engine shader program
		/// </summary>
		enum JIGSAW_SHADER {
			JIGSAW_VERTEX_SHADER_SOLID_COLOR_PRIMITIVE,
			JIGSAW_PIXEL_SHADER_SOLID_COLOR
		};
	}
}
#endif
