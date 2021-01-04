#include "JigsawEntityClusterService.h"

using namespace Jigsaw::System;
using namespace Jigsaw::Entities;

JigsawEntityCluster& JigsawEntityClusterService::operator[](const Signature& signature) {
	auto find = signature_cluster_map.find(signature);
	if (find != signature_cluster_map.end()) {
		return *(find->second);
	} else {
		JigsawEntityCluster* new_service = new JigsawEntityCluster(MakeRef<Signature>(signature));
		signature_cluster_map.insert(std::make_pair(new_service->GetSignature(), new_service));
		return *new_service;
	}
}

std::map<Jigsaw::System::Signature, JigsawEntityCluster*>::const_iterator Jigsaw::Entities::JigsawEntityClusterService::begin() const {
	return signature_cluster_map.begin();
}

std::map<Jigsaw::System::Signature, JigsawEntityCluster*>::const_iterator Jigsaw::Entities::JigsawEntityClusterService::end() const {
	return signature_cluster_map.end();
}
