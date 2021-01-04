#include "JigsawMachinePiece.h"
#include "Marshalling/JigsawMarshalling.h"

namespace Jigsaw {
	namespace Machines {
		void Jigsaw::Machines::JigsawMachinePiece::ComposeSignature(Jigsaw::System::SignatureBuilder& signature_builder) const {
			for (const JigsawMachinePiece::component_def& comp_def : component_definitions) {
				comp_def.s_augmentor(signature_builder);
			}
		}

		void JigsawMachinePiece::Turn(Jigsaw::Entities::JigsawEntity& entity) const {
			for (const JigsawMachinePiece::component_def& comp_def : component_definitions) {
				comp_def.e_init(entity);
			}
		}

		void JigsawMachinePiece::Hammer(Jigsaw::Entities::JigsawEntity& entity, Jigsaw::Machines::FABRICATION_ARGS& args) const {
			for (const JigsawMachinePiece::component_def& comp_def : component_definitions) {
				comp_def.e_aug(entity, args);
			}
		}

		START_REGISTER_SERIALIZABLE_CLASS(JigsawMachinePiece)
		END_REGISTER_SERIALIZABLE_CLASS(JigsawMachinePiece)
	}
}

