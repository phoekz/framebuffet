#include "debug.hpp"

namespace fb {

LeakTracker::~LeakTracker() {
#if defined(_DEBUG)
    if (debug_device) {
        debug_device->ReportLiveDeviceObjects(
            D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
    }
#endif
}

auto dx_set_name(ID3D12Object* object, std::string_view name) -> void {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void {
    dx_set_name(object.get(), name);
}

} // namespace fb
