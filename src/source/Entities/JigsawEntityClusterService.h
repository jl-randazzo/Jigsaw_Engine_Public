#ifndef _JIGSAW_ENTITY_CLUSTER_SERVICE_H_
#define _JIGSAW_ENTITY_CLUSTER_SERVICE_H_
#include <map>
#include "System/Signature.h"
#include "JigsawEntityCluster.h"

namespace Jigsaw {
	namespace Entities {
		class JigsawEntityClusterService {
		public:
			JigsawEntityClusterService() {};

			JigsawEntityCluster& operator[](const Jigsaw::System::Signature& signature);

			std::map<Jigsaw::System::Signature, JigsawEntityCluster*>::const_iterator begin() const;

			std::map<Jigsaw::System::Signature, JigsawEntityCluster*>::const_iterator end() const;

		private:
			std::map<Jigsaw::System::Signature, JigsawEntityCluster*> signature_cluster_map;

		};
	}
}
#endif