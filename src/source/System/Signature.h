#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_
#include <vector>
#include <string>
#include "Ref.h"
#include "Entities/EntityMapping.h"
#include "SignatureMap.h"
#include "Util/etype_info.h"
#include <algorithm>


namespace Jigsaw {
	namespace System {

		/// <summary>
		/// Signature is the common implementation for the SystemSignature and the EntitySignature. Member types will
		/// be collected in the aligned_argument_types.
		/// </summary>
		class Signature {
		friend class SignatureBuilder;
		public:
			Signature(Signature&& other) noexcept : sig_map(std::move(other.sig_map)), aligned_types(std::move(other.aligned_types)) {}

			Signature(const Signature& other) noexcept;

			/// <summary>
			/// Returns an immutable vector of the types in order
			/// </summary>
			const std::vector<Jigsaw::Util::etype_index>& GetAlignedTypes() const;

			/// <summary>
			/// Checks if the signature contains the given type
			/// </summary>
			const bool ContainsType(Jigsaw::Util::etype_index) const;

			/// <summary>
			/// Returns the total size of the aligned_types for this signature
			/// </summary>
			inline size_t GetSize() const {
				return sig_map.GetSize();
			}

			/// <summary>
			/// Returns an immutable map that associates type_infos with their byte offset into the data
			/// or associates indexes of the aligned types into their offset in the data
			/// </summary>
			const SignatureMap& GetMap() const;

			/// <summary>
			/// Builds a new SignatureMap that has indices that are aligned to the types in the convert_to signature.
			///
			/// Example: 
			/// SignatureMap sig_map built with 4 types : Quaternion, Vector2, Vector3, DataStruct
			/// Signature 'convert_to' built with 2 aligned_types: DataStruct, Vector3
			/// 
			/// SignatureMap conversion_map;
			/// if(sig_map.HasConversionMap(convert_to, conversion_map)){
			///		// Do stuff
			/// }
			/// 
			/// conversion_map.GetOffsetOfTypeIndex(0) will no longer return an offset that maps to Quaternion in the data block
			/// that consumes the original signature, it will map to 'DataStruct.' conversion_map.GetOffsetOfTypeIndex(1) will no longer
			/// return an offset that maps to Vector2, it will return an offset that maps to Vector3. 
			///
			/// </summary>
			/// <param name="convert_to">The signature that the out_conversion_map's indices will map to</param>
			/// <param name="out_conversion_map">A pointer to a SignatureMap unique_ptr that will receive the completed conversion SignatureMap</param>
			/// <returns>true <==> there is a valid conversion between the signatures, and the out_conversion_map will be populated.</returns>
			bool HasConversionMap(const Signature& convert_to, Jigsaw::Unique<SignatureMap>* out_conversion_map) const;

			/// <summary>
			/// Equality operator ensures that there is a matching number of types and the exact order matches.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			bool operator==(const Signature& other) const;
			
			/// <summary>
			/// Less than operator for use with map objects
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			bool operator<(const Signature& other) const;

		protected:
			const std::vector<Jigsaw::Util::etype_index> aligned_types;
			SignatureMap sig_map;

		private:
			/// <summary>
			/// The primary constructor for Signature takes a vector of etype_index objects. They are assumed to be sorted at this point. 
			/// It is private to enforce construction of a Signature with an approved interface. The SignatureBuilder is a good candidate.
			/// </summary>
			/// <param name="aligned_types"></param>
			Signature(const std::vector<Jigsaw::Util::etype_index>& aligned_types);
		};

		/// <summary>
		/// Signature is the common implementation for the SystemSignature and the EntitySignature. Member types will
		/// be collected in the aligned_argument_types.
		/// </summary>
		class SignatureBuilder {
		public:
			enum class SORT_MODE {
				SORT_MODE_ON, SORT_MODE_OFF 
			};

			/// <summary>
			/// Constructor
			/// </summary>
			SignatureBuilder() {};

			/// <summary>
			/// Copy constructor is supported with non-const argument
			/// </summary>
			/// <param name="other"></param>
			SignatureBuilder(SignatureBuilder& other) : types(other.types) { }

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="other"></param>
			SignatureBuilder(SignatureBuilder&& other) noexcept : types(other.types) { }

			/// <summary>
			/// Assignment operator is supported for SignatureBuilders because the construction has not been commited. 
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			SignatureBuilder& operator=(SignatureBuilder& other);

			/// <summary>
			/// If it is not already there, adds the type to be registered for the Signature
			/// </summary>
			/// <param name="type"></param>
			/// <returns></returns>
			SignatureBuilder& AddType(const Jigsaw::Util::etype_info& type);

			/// <summary>
			/// Pull in all the types from the signature parameter that are not already present in the SignatureBuilder's types registry
			/// </summary>
			/// <param name="signature"></param>
			/// <returns></returns>
			SignatureBuilder& AddTypes(const Signature& signature);

			/// <summary>
			/// Adds the array of types to the current SignatureBuilder
			/// </summary>
			/// <param name="indices"></param>
			/// <returns></returns>
			SignatureBuilder& AddTypes(const std::vector<Jigsaw::Util::etype_index>& indices);

			/// <summary>
			/// Sets the sort mode for the aligned types on build
			/// </summary>
			/// <param name="sort_mode"></param>
			/// <returns></returns>
			SignatureBuilder& SortMode(SORT_MODE sort_mode);

			bool HasType(Jigsaw::Util::etype_index type) const;

			/// <summary>
			/// Sorts the types and then builds a Signature with them
			/// </summary>
			/// <returns></returns>
			Jigsaw::Unique<Signature> Build();

		private:

			SORT_MODE sort_mode = SORT_MODE::SORT_MODE_ON;
			std::vector<Jigsaw::Util::etype_index> types;
		};
	}
}

namespace std {
	template <> struct hash<Jigsaw::System::Signature>
	{
		size_t operator()(const Jigsaw::System::Signature& sig) const {
			auto aligned_types = sig.GetAlignedTypes();
			size_t hash = 0;
			for (const Jigsaw::Util::etype_info& aligned_type : aligned_types) {
				hash += aligned_type.GetBase().hash_code();
			}
			return hash;
		}
	};
}
#endif
