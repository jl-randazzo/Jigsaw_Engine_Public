#ifndef _TEST_UTILS_B_H
#define _TEST_UTILS_B_H

namespace ProjectTests {
	class t_uid_class;
	class t_context_root {
	public:
		t_context_root() : root_ptr() {}
		t_context_root(t_uid_class* context_root) : root_ptr(static_cast<t_uid_class*>(context_root)) {}

		t_context_root& operator=(const t_context_root& other) {
			root_ptr = other.root_ptr;
			return *this;
		}

		t_context_root& operator=(t_context_root&& other) {
			root_ptr = other.root_ptr;
			return *this;
		}

		const t_uid_class* GetTUIDClass() {
			return root_ptr;
		}

	private:
		const t_uid_class* root_ptr;
	};
}


#endif // !_TEST_UTILS_B_H
#pragma once
