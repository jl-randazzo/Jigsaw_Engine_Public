#include "JigsawEntityCluster.h"
#include "Debug/j_debug.h"

using namespace Jigsaw::Entities;

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::operator[](unsigned int x) {
	unsigned int offset = x * signature->GetSize();
	return JigsawEntity(raw_data.get() + offset, signature->GetMap(), cluster_ids[x], scopes[x]);
}

JigsawEntityCluster::JigsawEntityClusterNode::JigsawEntityClusterNode(const Jigsaw::Ref<Jigsaw::System::Signature>& signature) : signature(signature),
raw_data(Jigsaw::Ref<BYTE[]>((BYTE*)_aligned_malloc(signature->GetSize()* CLUSTER_SIZE, sizeof(max_align_t)), [](BYTE* byte) { _aligned_free(byte); })), count(0) {}

void Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::Destroy(int index) {
	BYTE* entity_address = &raw_data[index * signature->GetSize()];
	JigsawEntity entity = this->operator[](index);
	const Jigsaw::System::SignatureMap& map = signature->GetMap();

	int i = 0;
	for (const Jigsaw::Util::etype_info& t_info : signature->GetAlignedTypes()) {
		BYTE* type_address = map[i++] + entity_address;
		bool was_destructed = t_info.GetUnsafeFunctions().DestructInPlace(type_address);
		J_D_ASSERT_LOG_ERROR((was_destructed), JigsawEntity, "The Component type {0} of the entity cannot be instantiated--it is either not move assignable or not default destructible. It cannot be a component for a JigsawEntity", t_info.GetQualifiedName().c_str());
	}
}

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::FabricateEntity(JigsawEntity::SCOPE scope) {
	BYTE* entity_address = &raw_data[count * signature->GetSize()];
	const Jigsaw::System::SignatureMap& map = signature->GetMap();

	int i = 0;
	for (const Jigsaw::Util::etype_info& t_info : signature->GetAlignedTypes()) {
		BYTE* type_address = map[i++] + entity_address;
		bool was_constructed = t_info.GetUnsafeFunctions().ConstructInPlace(type_address);
		J_D_ASSERT_LOG_ERROR((was_constructed), JigsawEntity, "The Component type {0} of the entity cannot be instantiated--it is either not move assignable or not default constructible", t_info.GetQualifiedName().c_str());
	}

	cluster_ids[count] = Jigsaw::System::UID::Create();
	scopes[count] = scope;
	return JigsawEntity(entity_address, signature->GetMap(), cluster_ids[count++], scope);
}

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::PopLast() {
	return (*this)[--count];
}

void Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::ReplaceEmptyIndex(const JigsawEntity& entity, const int index) {
	J_D_ASSERT_LOG_ERROR((index < count), JigsawEntityClusterNode, "Insert was called with an index beyond the JigsawEntityClusterNode's capacity");
	const BYTE* const data = entity.raw_data;
	memcpy(&raw_data[index * signature->GetSize()], data, signature->GetSize());
	cluster_ids[index] = entity.GetUID();
}

bool Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::Contains(const Jigsaw::System::UID& id, int* index) {
	int i = 0;
	for (int i = 0; i < count; i++) {
		if (cluster_ids[i] == id) {
			*index = i;
			return true;
		}
	}
	return false;
}

unsigned int Jigsaw::Entities::JigsawEntityCluster::JigsawEntityClusterNode::GetCount() const {
	return count;
}

size_t Jigsaw::Entities::JigsawEntityCluster::GetClusterCount() const {
	return cluster_nodes.size();
}

size_t Jigsaw::Entities::JigsawEntityCluster::GetTotalCount() const {
	return entity_node_map.size();
}

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::FetchEntity(const Jigsaw::System::UID& entity_id) {
	int index = 0;
	Ref<JigsawEntityClusterNode>& node = entity_node_map[entity_id];
	node->Contains(entity_id, &index);
	return (*node)[index];
}

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::FabricateEntity(JigsawEntity::SCOPE scope) {
	if (cluster_nodes.size() == 0 || cluster_nodes.back()->GetCount() >= CLUSTER_SIZE) {
		Ref<JigsawEntityClusterNode> node = MakeRef<JigsawEntityClusterNode>(signature);
		cluster_nodes.push_back(node);
	}
	JigsawEntity e = cluster_nodes.back()->FabricateEntity(scope);
	entity_node_map.insert(std::make_pair(e.GetUID(), cluster_nodes.back()));
	return e;
}

void Jigsaw::Entities::JigsawEntityCluster::RemoveEntity(const Jigsaw::System::UID& entity_id) {
	Ref<JigsawEntityClusterNode>& last_node = cluster_nodes.back();
	auto iter = entity_node_map.find(entity_id);
	if (iter != entity_node_map.end()) {
		Ref<JigsawEntityClusterNode>& node = iter->second;
		int index = 0;
		if (node->Contains(entity_id, &index)) {
			node->Destroy(index);
			if (node == last_node && index == node->GetCount() - 1) {
				node->PopLast();
			}
			else {
				JigsawEntity entity = last_node->PopLast();
				node->ReplaceEmptyIndex(entity, index);
				entity_node_map[entity.GetUID()] = node;
			}

			if (last_node->GetCount() == 0) {
				cluster_nodes.pop_back();
			}
			entity_node_map.erase(entity_id);
		}
	}
}

const Jigsaw::System::Signature& Jigsaw::Entities::JigsawEntityCluster::GetSignature() const {
	return *signature;
}

Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR Jigsaw::Entities::JigsawEntityCluster::GetNodeIterator(unsigned int i) {
	return JIGSAW_ENTITY_ITERATOR(*cluster_nodes[i].get());
}

Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR::JIGSAW_ENTITY_ITERATOR(JigsawEntityClusterNode& node) : node(node) { }

Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR::operator++(int x) {
	i++;
	return *this;
}

JigsawEntity Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR::operator*() {
	return node[i];
}

Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR::operator bool() {
	return i < node.GetCount();
}
