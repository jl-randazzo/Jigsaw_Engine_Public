#ifndef _JIGSAW_MACHINE_PIECE_H_
#define _JIGSAW_MACHINE_PIECE_H_

#include <functional>
#include "System/Signature.h"
#include "Entities/JigsawEntity.h"
#include "Machines/FABRICATION_ARGS.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// Interface for tracking a dynamic type object. Not intended for Serialization or any other purposes
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T>
		class DTO {
		protected:
			typedef T _THIS_TYPE;
		};

		/// <summary>
		/// JigsawMachinePiece is the root class for all inheriting pieces that apply component data to JigsawEntities during fabrication
		/// </summary>
		class JigsawMachinePiece {
		public:
			JigsawMachinePiece() {}

			virtual ~JigsawMachinePiece() {}

			/// <summary>
			/// Modifies the 'signature_builder' to include the component data native to this JigsawMachinePiece
			/// </summary>
			/// <param name="signature_builder"></param>
			void ComposeSignature(Jigsaw::System::SignatureBuilder& signature_builder) const;

			/// <summary>
			/// 'Turns' the fabricated Entity, assigning the default ComponentData from this MachinePiece to the JigsawEntity
			/// </summary>
			/// <param name="signature_builder"></param>
			void Turn(Jigsaw::Entities::JigsawEntity& entity) const;

			/// <summary>
			/// 'Hammers' the fabricated Entity, assigning values corresponding to from the FABRICATION_ARGS to the newly-minted entity
			/// </summary>
			/// <param name="entity"></param>
			/// <param name="args"></param>
			void Hammer(Jigsaw::Entities::JigsawEntity& entity, Jigsaw::Machines::FABRICATION_ARGS& args) const;

		protected:
			struct component_def {
				std::function<void(Jigsaw::System::SignatureBuilder&)> s_augmentor;
				std::function<void(Jigsaw::Entities::JigsawEntity&)> e_init;
				std::function<void(Jigsaw::Entities::JigsawEntity&, Jigsaw::Machines::FABRICATION_ARGS&)> e_aug;
			};
			std::vector<component_def> component_definitions;

		};
	}
}


#define EXPANDER(fieldname) fieldname
#define D_EXPANDER(classname, field_name) EXPANDER(classname)EXPANDER(_)EXPANDER(field_name)

#define JGSW_MACHINE_PIECE(piece_type)\
class piece_type : public Jigsaw::Machines::JigsawMachinePiece, protected Jigsaw::Machines::DTO<piece_type>

#define JGSW_COMPONENT_DATA(type, field_name) \
type field_name; \
private: struct EXPANDER(field_name)_component_def  {\
	EXPANDER(field_name)_component_def(_THIS_TYPE& piece) {\
		Jigsaw::Machines::JigsawMachinePiece::component_def def;\
		const Jigsaw::Util::etype_info& t_info = Jigsaw::Util::etype_info::Id<type>(); \
		def.s_augmentor = [&t_info](Jigsaw::System::SignatureBuilder& builder) -> void { builder.AddType(t_info); };\
		def.e_init = [&piece](Jigsaw::Entities::JigsawEntity& e) { e.SetMemberData<type>(piece.field_name); }; \
		def.e_aug = [&t_info](Jigsaw::Entities::JigsawEntity& e, Jigsaw::Machines::FABRICATION_ARGS& args) { \
			type& t = e.GetMemberDataRef<type>(); \
			args.GetData(t_info, (BYTE*)(void*)&t);\
		}; \
		piece.component_definitions.push_back(def); \
	}; \
}; \
EXPANDER(field_name)_component_def EXPANDER(field_name)_component_def_register = EXPANDER(field_name)_component_def(*this); \
public:

#endif

