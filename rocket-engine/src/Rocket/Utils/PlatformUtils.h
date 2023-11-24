#pragma once

#include <string>

namespace Rocket {

	class FileDialogs {
	public:
		//these methods are returning empty strings if dialog window was closed-operation canceled
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};


} // namespace Rocket