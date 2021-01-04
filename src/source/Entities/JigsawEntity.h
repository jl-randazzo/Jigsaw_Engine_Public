#ifndef _JIGSAW_ENTITY_H_
#define _JIGSAW_ENTITY_H_
#include "System/Signature.h"
#include <memory>
#include "Util/etype_info.h"
#include "Util/BYTE.h"
#include "System/UID.h"

namespace Jigsaw {
	namespace Entities {

		/// <summary>
		/// The JigsawEntityCluster is friended so that the raw, underlying data stream can be retrieved from the JigsawEntity
		/// </summary>
		class JigsawEntityCluster;

		/// <summary>
		/// A JigsawEntity is an instantiation of an EntitySignature's type layout. JigsawEntities are packed tightly into an
		/// EntityCluster, which are managed by the EntityService. 
		/// </summary>
		class JigsawEntity {
			friend JigsawEntityCluster;
		public:
			/// <summary>
			/// Defines the scope of a given entity
			/// </summary>
			enum SCOPE {
				SCOPE_EPHEMERAL,
				SCOPE_STATIC,
				SCOPE_PERSISTENT
			};

			/// <summary>
			/// Copy constructor just takes data directly from the other JigsawEntity; it does not copy it because the JigsawEntity object
			/// does not own the underlying object
			/// </summary>
			/// <param name="other"></param>
			JigsawEntity(const JigsawEntity& other) : raw_data(other.raw_data), id(other.id), map(other.map), scope(other.scope) { }

			/// <summary>
			/// Rvalue move constructor just takes data directly from the other JigsawEntity. The reason is that the JigsawEntity
			/// does not own the underlying data, so it can't transfer it. 
			/// </summary>
			/// <param name="other"></param>
			JigsawEntity(JigsawEntity&& other) noexcept : raw_data(other.raw_data), id(other.id), map(other.map), scope(other.scope) {}

			/// <summary>
			/// Returns a non-reference copy of the member data of type 'T' in this particular entity.
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <returns>The components current data, not by reference</returns>
			template <typename T>
			T GetMemberData() const {
				BYTE offset = map.GetTypeOffset(Jigsaw::Util::etype_info::Id<T>());
				BYTE* loc = raw_data + offset;
				return *Jigsaw::Util::CastToTypeObject<T>(loc);
			}

			template <typename T>
			T& GetMemberDataRef() {
				BYTE offset = map.GetTypeOffset(Jigsaw::Util::etype_info::Id<T>());
				BYTE* loc = raw_data + offset;
				return *Jigsaw::Util::CastToTypeObject<T>(loc);
			}

			/// <summary>
			/// Assigns the value of 'data' to the the Entity's component of the same type 'T'
			/// </summary>
			/// <typeparam name="T"></typeparam>
			template <typename T>
			void SetMemberData(const T& data) {
				const Jigsaw::Util::etype_info& type = Jigsaw::Util::etype_info::Id<T>();
				BYTE offset = map.GetTypeOffset(type);
				BYTE* loc = raw_data + offset;
				*static_cast<T*>((void*)loc) = data;
			}

			/// <summary>
			/// Assigns the value of 'data' to the the Entity's component of the same type 'T'
			/// </summary>
			/// <typeparam name="T"></typeparam>
			template <typename T>
			void SetMemberData(T&& data) {
				const Jigsaw::Util::etype_info& type = Jigsaw::Util::etype_info::Id<T>();
				BYTE offset = map.GetTypeOffset(type);
				BYTE* loc = raw_data + offset;
				*static_cast<T*>((void*)loc) = std::move(data);
			}

			/// <summary>
			/// Returns a reference to this Entity's unique identifier
			/// </summary>
			/// <returns></returns>
			const Jigsaw::System::UID& GetUID() const {
				return id;
			}

			/// <summary>
			/// Fetches the direct raw_data located in the cluster
			/// </summary>
			/// <returns></returns>
			BYTE* GetRawData() {
				return this->raw_data;
			}

			/// <summary>
			/// Returns the scope of the current entity
			/// </summary>
			/// <returns></returns>
			SCOPE GetScope() const {
				return scope;
			}

		private:

			JigsawEntity(BYTE* raw_data, const Jigsaw::System::SignatureMap& map, Jigsaw::System::UID& uid, SCOPE scope) 
				: raw_data(raw_data), map(map), id(uid), scope(scope) { }

			SCOPE scope;
			const Jigsaw::System::SignatureMap& map;
			Jigsaw::System::UID id;

			/// <summary>
			/// The JigsawEntity itself has no responsibility for the lifetime of the underlying raw_data. 
			/// The deletion of the raw_data is the responsibility of the JigsawEntityCluster which is interfaced with
			/// by the JigsawMachineObserver
			/// </summary>
			BYTE* raw_data;
		};
	}
}
#endif
