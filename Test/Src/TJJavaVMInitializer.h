#ifndef __TG_JAVAVMINITIALIZER_H__
#define __TG_JAVAVMINITIALIZER_H__

#include "../../TinyJni/Include/TJCommon.h"

class TJJavaVMInitializer
{
public:
	TJJavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options = TJStringArray());
	~TJJavaVMInitializer() noexcept;

private:
	TJStringArray mOptions;
	TJJNIVersion mVersion;

private:
	std::string GetJavaHomePathFromRegistry() const;

	// no implementation
	TJJavaVMInitializer(const TJJavaVMInitializer&) = delete;
	TJJavaVMInitializer& operator=(const TJJavaVMInitializer&) = delete;
};

#endif //__TG_JAVAVMINITIALIZER_H__
