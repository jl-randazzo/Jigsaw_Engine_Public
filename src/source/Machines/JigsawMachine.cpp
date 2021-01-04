#include "JigsawMachine.h"
#include "Entities/JigsawEntityCluster.h"
#include "Marshalling/JigsawMarshalling.h"


namespace Jigsaw {
	namespace Machines {
		START_REGISTER_SERIALIZABLE_CLASS(JigsawMachine)
		REGISTER_SERIALIZABLE_FIELD(JigsawMachine, std::string, name)
		REGISTER_SERIALIZABLE_POINTER_VECTOR(JigsawMachine, JigsawMachinePiece, pieces)
		REGISTER_SERIALIZABLE_FIELD(JigsawMachine, Jigsaw::System::UID, machine_id)
		END_REGISTER_SERIALIZABLE_CLASS(JigsawMachine)

		const Jigsaw::Unique<Jigsaw::System::Signature> Jigsaw::Machines::JigsawMachine::GetSignature() const {
			Jigsaw::System::SignatureBuilder builder;
			for (const Jigsaw::Machines::JigsawMachinePiece* piece : this->pieces) {
				piece->ComposeSignature(builder);
			}
			return builder.Build();
		}

		JigsawMachine::~JigsawMachine() {
			for (JigsawMachinePiece* piece : pieces) {
				delete piece;
			}
		}

		Jigsaw::Entities::JigsawEntity JigsawMachine::FabricateEntity(Jigsaw::Entities::JigsawEntityCluster& cluster, Jigsaw::Machines::FABRICATION_ARGS& args) {
			Jigsaw::Entities::JigsawEntity entity(cluster.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL));
			for (JigsawMachinePiece* piece : pieces) {
				piece->Turn(entity);
				piece->Hammer(entity, args);
			}
			return entity;
		}
	}
}