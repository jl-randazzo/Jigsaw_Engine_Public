#ifndef _UID_H_
#define _UID_H_

#include <string>

namespace Jigsaw {
	namespace System {

		/// <summary>
		/// UID is a locally-unique identifier.
		/// </summary>
		class UID {
			public:
				/// <summary>
				/// Only for internal use. It is NOT RECOMMENDED to manually set ids.
				/// </summary>
				/// <param name="a"></param>
				/// <param name="b"></param>
				UID(long long a, long long b) : data_a(a), data_b(b) {}

				/// <summary>
				/// Default constructor
				/// </summary>
				UID() : data_a(0), data_b(0) {}

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="other"></param>
				UID(const UID& other) : data_a(other.data_a), data_b(other.data_b) {}

				/// <summary>
				/// Move constructor
				/// </summary>
				/// <param name="other"></param>
				UID(UID&& other) noexcept : data_a(other.data_a), data_b(other.data_b) {}

				/// <summary>
				/// Stack assignment operator.
				/// </summary>
				/// <param name="other"></param>
				/// <returns></returns>
				UID& operator=(const UID& other);

				/// <summary>
				/// Creates a UID from anywhere in the program that can be used freely.
				/// </summary>
				/// <returns></returns>
				static UID Create();

				/// <summary>
				/// Returns a UID with the values reserved for an uninitialized object
				/// </summary>
				/// <returns></returns>
				static UID Empty();

				/// <summary>
				/// Returns the value of the first data element
				/// </summary>
				/// <returns></returns>
				long long GetData_a() const;

				/// <summary>
				/// Returns the value of the second data element
				/// </summary>
				/// <returns></returns>
				long long GetData_b() const;

				/// <summary>
				/// General equality override
				/// </summary>
				/// <param name="other"></param>
				/// <returns></returns>
				bool operator==(const UID& other) const; 

				/// <summary>
				/// General less than override
				/// </summary>
				/// <param name="other"></param>
				/// <returns></returns>
				bool operator<(const UID& other) const;

				long long data_a;
				long long data_b;
		};

		void UIDFromString(Jigsaw::System::UID* node, std::string* string);
		UID UIDFromString(std::string&& str);
		std::string* UIDToString(const Jigsaw::System::UID* uid);
		std::string UIDToString(const Jigsaw::System::UID& uid);
	}
}

namespace std {
	template <> struct hash<Jigsaw::System::UID>
	{
		size_t operator()(const Jigsaw::System::UID& uid) const {
			return (size_t)uid.GetData_a();
		}
	};
}
#endif