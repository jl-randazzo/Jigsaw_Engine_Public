#include "CubeSystem.h"
#include "Camera.h"
#include "Graphics/Renderer.h"

namespace Jigsaw {
	namespace Systems {
		void CubeSystem_O::MoveCube(Transform& transform, const Jigsaw::Time::TimeStep& ts, Jigsaw::Assets::CubeComponent& cube) {
			transform.position = transform.position + Vector3(.001, .001f, 0);
		}

		REGISTER_SYSTEM(CubeSystem_O);

		Camera* camera = new Camera(Vector3(0, 0, 0), Vector3(1, 0, 0));
		void CubeSystem::RotateCube(Transform& transform, const Jigsaw::Time::TimeStep& ts, Jigsaw::Assets::CubeComponent& cube) {
			Quaternion rotate(ANGLE, RAD_S * .01f);
			transform.rotation = rotate * transform.rotation;

			Jigsaw::Graphics::RENDER_DATA render_data;

			render_data.pvMat = camera->GetProjectionMatrix(1.77777778) * camera->GetViewMatrix();
			render_data.t = transform;
			render_data.v_buffer = cube.cube_model->GetBuffer();
			size_t size;
			render_data.i_buffer = cube.cube_model->GetIndexBuffer(&size);

			Renderer::DrawIndexed(Jigsaw::Pipeline::JIGSAW_PIPELINE_DRAW, render_data);
		}


		REGISTER_SYSTEM(CubeSystem);
	}
}