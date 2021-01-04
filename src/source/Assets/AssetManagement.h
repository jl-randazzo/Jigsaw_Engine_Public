#ifndef _ASSET_MANAGEMENT_H_
#define _ASSET_MANAGEMENT_H_
#include "windows.h"
#include "RuntimeAssets.h"
#include "DataAssets.h"
#include "AssetDatabase.h"
#include <direct.h>
#include <concurrent_unordered_map.h>
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"

/**
 * Public resource management functions that are friended to the base resources.
 */
namespace Jigsaw {
	namespace Assets {

		/// <summary>
		/// AssetManager provides a thread-safe view into all of the resources loaded in memory. The instance is only visible to
		/// the AssetManagerOwner class
		/// </summary>
		class AssetManager {
		private:
			enum ASSET_STATE {
				ASSET_STATE_LOADING, ASSET_STATE_ACTIVE, ASSET_STATE_INACTIVE
			};

			/// <summary>
			/// Internal struct is used to keep track of the state of a given AssetDescriptor 
			/// </summary>
			struct ASSET_DATA_BLOCK {
				ASSET_STATE state;
				Jigsaw::Weak<Jigsaw::Assets::DataAsset> ref;
			};

			std::mutex lock;

			/// <summary>
			/// Internal function used to fetch internal assets and return information about them in the ref_block
			/// </summary>
			_Requires_lock_held_(this->lock) void FindAsset(const Jigsaw::System::UID& uid, ASSET_DATA_BLOCK& ref_block);

			/// <summary>
			/// Places an indicator that the asset represented by the UID is being loaded into memory and made available
			/// so that concurrent threads do not attempt to load it also. 
			/// </summary>
			_Requires_lock_held_(this->lock) void MarkAssetForLoading(const Jigsaw::System::UID& uid);

			_Guarded_by_(lock) concurrency::concurrent_unordered_map<Jigsaw::System::UID, ASSET_DATA_BLOCK> resource_map;

		public:
			/// <summary>
			/// The calling thread officially claims the Asset associated with the given UID. If the Asset has already been claimed
			/// by another thread, but the runtime import is incomplete, then the calling thread will block until the Asset is made available.
			/// If the Asset has previously been claimed, but has expired, or if the Asset has never been claimed, this function will return false,
			/// and it is the calling thread's responsibility to fetch the Asset and load it into memory.
			/// </summary>
			/// <param name="uid">The uid of the requested asset</param>
			/// <param name="out_ref">An empty Ref that receives an active reference to the requested Asset</param>
			/// <returns>True if the Resource was available and the out_ref was populated; otherwise, returns false</returns>
			bool ClaimAsset(const Jigsaw::System::UID& uid, Jigsaw::Ref<Jigsaw::Assets::DataAsset>* out_ref);

			/// <summary>
			/// If the calling thread previously issued a call to 'ClaimAsset,' and that call returned false, then the calling thread must subsequently
			/// return to deliver a loaded asset, 'claimed_asset.'
			/// </summary>
			/// <param name="claimed_asset"></param>
			/// <returns></returns>
			bool WriteClaimedAsset(Jigsaw::Ref<Jigsaw::Assets::DataAsset>& claimed_asset);

		protected:
			AssetManager() {}

		};

		/// <summary>
		/// AssetManagerOwner provides the only access to the AssetManager as a Singleton that is only visible to the AssetManagement functions such as FetchAssets.
		/// 
		/// This was done so that any functions that are meant to have privileged access to the table of asset references will be specified here.
		///
		/// Generally, there should be no reason that a consuming thread is interacting with the AssetManager except to fetch an asset, a behavior that does not need
		/// class encapsulation. 
		/// </summary>
		class AssetManagerOwner: public AssetManager {
		public:
			friend Jigsaw::Ref<DataAsset> FetchAsset(const char* file_name, ASSET_TYPE type, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);
			friend Jigsaw::Unique<Jigsaw::Ref<DataAsset>[]> FetchAssets(const Jigsaw::Unique<Jigsaw::System::UID[]>& uids, const int count, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

		private:
			/// <summary>
			/// Private function is only available to friend functions. This retrieves the sole instance of the AssetManager. Accessing it is highly privileged. 
			/// </summary>
			/// <returns></returns>
			static AssetManager& GetManager();
		};

		Jigsaw::Ref<DataAsset> FetchAsset(const char* file_name, ASSET_TYPE type, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

		Jigsaw::Unique<Jigsaw::Ref<DataAsset>[]> FetchAssets(const Jigsaw::Unique<Jigsaw::System::UID[]>& descriptors, const int count, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

		template <typename T, typename V>
		Jigsaw::Ref<T> CreateResource(const T* data, size_t count, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {

			AssetDescriptor descriptor;
			descriptor.id = Jigsaw::System::UID::Create();

			RuntimeAsset<T>* resource;
				resource = new RuntimeAsset<T>(descriptor);
			

			resource->Initialize(data, count);

			return resource;
		}

		template <typename T>
		AssetDescriptor UpdateResource(const RuntimeAsset<T>* subresource, const T* newData, size_t count, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd_list) {
			subresource->Update(newData, count);
			return subresource->GetDescriptor();
		}
	}
};

#endif
