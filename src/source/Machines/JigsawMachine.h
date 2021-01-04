#ifndef _JIGSAW_MACHINE_H_
#define _JIGSAW_MACHINE_H_
#include "JigsawMachinePiece.h"
#include "Ref.h"
#include "System/Signature.h"
#include "Machines/FABRICATION_ARGS.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// JigsawMachine is the core scene element in the JigsawEngine
		/// </summary>
		class JigsawMachine {
		public:
			JigsawMachine() : machine_id(Jigsaw::System::UID::Create()) {}
			JigsawMachine(const JigsawMachine& other) : name(other.name) {}

			~JigsawMachine();

			Jigsaw::Entities::JigsawEntity FabricateEntity(Jigsaw::Entities::JigsawEntityCluster& cluster, Jigsaw::Machines::FABRICATION_ARGS& args);

			const Jigsaw::Unique<Jigsaw::System::Signature> GetSignature() const;

			std::string name;

			Jigsaw::Unique<JigsawMachine[]> children;

			std::vector<JigsawMachinePiece*> pieces;

			Jigsaw::System::UID machine_id;

		};

	}
}

#endif