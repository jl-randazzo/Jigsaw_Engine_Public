#ifndef _JIGSAW_ENTITY_CLUSTER_H_
#define _JIGSAW_ENTITY_CLUSTER_H_
#include "System/Signature.h"
#include "Debug/j_debug.h"
#include "Util/BYTE.h"
#include "System/UID.h"
#include "Entities/JigsawEntity.h"
#include "Ref.h"
#include <stdlib.h>

#define CLUSTER_SIZE 60

namespace Jigsaw {
	namespace Entities {

		class JigsawEntityCluster;

		/// <summary>
		/// JigsawEntityClusters maintain all of the JigsawEntityClusterNodes for a unique signature. 
		/// </summary>
		class JigsawEntityCluster {
		private:
			/// <summary>
			/// A private class that encapsulates the interface into any of the Entity Data Clusters
			/// </summary>
			class JigsawEntityClusterNode {
			public:
				/// <summary>
				/// Primary constructor takes a signature reference as an argument.
				/// </summary>
				/// <param name="signature"></param>
				JigsawEntityClusterNode(const Jigsaw::Ref<Jigsaw::System::Signature>& signature);

				/// <summary>
				/// Returns an Entity representing the data at the specified index 'x'
				/// </summary>
				/// <param name="x"></param>
				/// <returns></returns>
				JigsawEntity operator[](unsigned int x);

				/// <summary>
				/// Destroys the JigsawEntity at the given index, calling the destructor for each of its components in the Signature
				/// </summary>
				/// <param name="index"></param>
				void Destroy(int index);

				/// <summary>
				/// Fabricates a brand new JigsawEntity with the local Signature
				/// </summary>
				/// <returns></returns>
				JigsawEntity FabricateEntity(JigsawEntity::SCOPE scope);

				/// <summary>
				/// Pops the last JigsawEntity off the end of the ClusterNode
				/// </summary>
				/// <returns></returns>
				JigsawEntity PopLast();

				/// <summary>
				/// Replaces the empty spot at 'index' with an already-existing 'entity' that is being moved from somewhere else,
				/// usually the result of a 'PopLast' call
				/// </summary>
				/// <param name="entity"></param>
				/// <param name="index"></param>
				void ReplaceEmptyIndex(const JigsawEntity& entity, const int index);

				bool Contains(const Jigsaw::System::UID& id, int* index);

				unsigned int GetCount() const;

			private:
				Jigsaw::Ref<BYTE[]> raw_data;
				Jigsaw::System::UID cluster_ids[CLUSTER_SIZE];
				Jigsaw::Entities::JigsawEntity::SCOPE scopes[CLUSTER_SIZE];
				unsigned int count;
				const Jigsaw::Ref<Jigsaw::System::Signature>& signature;
			};

		public:

			/// <summary>
			/// This public class allows external classes to iterate through the JigsawEntities of a given node without having
			/// direct access to the Node definition itself
			/// </summary>
			class JIGSAW_ENTITY_ITERATOR {
			public:
				JIGSAW_ENTITY_ITERATOR(JigsawEntityClusterNode& cluster);

				JIGSAW_ENTITY_ITERATOR operator++(int);

				JigsawEntity operator*();

				operator bool();

			private:
				JigsawEntityClusterNode& node;

				int i = 0;

			};

			/// <summary>
			/// The primary constructor needs only to know about the Signature for all of the JigsawEntities managed within. 
			/// This Signature will never change for the entire lifetime of the JigsawEntityClusterRoot.
			/// </summary>
			/// <param name="signature"></param>
			JigsawEntityCluster(const Jigsaw::Ref<Jigsaw::System::Signature>& signature) : signature(signature) {}

			/// <summary>
			/// Returns the number of ClusterNodes in the Cluster
			/// </summary>
			/// <returns></returns>
			size_t GetClusterCount() const;

			/// <summary>
			/// Returns the total number of Entities present in this cluster, across all of its nodes
			/// </summary>
			/// <returns></returns>
			size_t GetTotalCount() const;

			/// <summary>
			/// Returns the JigsawEntity associated with the UID. It is assumed that the Entity exists in the Cluster
			/// </summary>
			/// <param name="entity_id"></param>
			/// <returns></returns>
			JigsawEntity FetchEntity(const Jigsaw::System::UID& entity_id);

			/// <summary>
			/// Creates a new entity associated with this JigsawEntityCluster and its signature
			/// </summary>
			/// <returns></returns>
			JigsawEntity FabricateEntity(JigsawEntity::SCOPE scope);

			/// <summary>
			/// Remove the entity with the specified UID from the JigsawEntityCluster
			/// </summary>
			/// <param name="entity_id"></param>
			void RemoveEntity(const Jigsaw::System::UID& entity_id);

			/// <summary>
			/// Returns a constant reference to the signature housed in the Cluster
			/// </summary>
			/// <returns></returns>
			const Jigsaw::System::Signature& GetSignature() const;

			JIGSAW_ENTITY_ITERATOR GetNodeIterator(unsigned int i);

		private:
			std::unordered_map<Jigsaw::System::UID, Jigsaw::Ref<JigsawEntityClusterNode>> entity_node_map;
			std::vector<Jigsaw::Ref<JigsawEntityClusterNode>> cluster_nodes;
			const Jigsaw::Ref<Jigsaw::System::Signature> signature;

		};
	}
}

#endif
