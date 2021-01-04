#ifndef _RENDER_DATA_H_
#define _RENDER_DATA_H_

#include "GPURuntimeObjects.h"
#include "Physics/Transform.h"

namespace Jigsaw {
	namespace Graphics {
		struct RENDER_DATA {
			Jigsaw::Ref <GPUBuffer> v_buffer;
			Jigsaw::Ref<GPUBuffer> i_buffer;
			Transform t;
			Mat4x4 pvMat;
		};
	}
}
#endif