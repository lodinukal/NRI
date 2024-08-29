// © 2021 NVIDIA Corporation

#include "SharedD3D11.h"

#include "FenceD3D11.h"

using namespace nri;

Result FenceD3D11::Create(uint64_t initialValue) {
    if (m_Device.GetVersion() >= 5) {
        HRESULT hr = m_Device->CreateFence(initialValue, D3D11_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D11Device5::CreateFence()");
    } else {
        D3D11_QUERY_DESC queryDesc = {};
        queryDesc.Query = D3D11_QUERY_EVENT;

        HRESULT hr = m_Device->CreateQuery(&queryDesc, &m_Query);
        RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D11Device::CreateQuery()");
    }

    m_Event = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    m_Value = initialValue;

    return Result::SUCCESS;
}

//================================================================================================================
// NRI
//================================================================================================================

inline uint64_t FenceD3D11::GetFenceValue() const {
    if (m_Fence)
        return m_Fence->GetCompletedValue();

    return m_Value;
}

void FenceD3D11::QueueSignal(uint64_t value) {
    if (m_Fence) {
        HRESULT hr = m_Device.GetImmediateContext()->Signal(m_Fence, value);
        RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "D3D11DeviceContext4::Signal()  failed!");
    } else {
        m_Device.GetImmediateContext()->End(m_Query);
        m_Value = value;
    }
}

void FenceD3D11::QueueWait(uint64_t value) {
    if (m_Fence) {
        HRESULT hr = m_Device.GetImmediateContext()->Wait(m_Fence, value);
        RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "D3D11DeviceContext4::Wait()  failed!");
    }
}

inline void FenceD3D11::Wait(uint64_t value) {
    if (m_Fence) {
        if (m_Event == 0 || m_Event == INVALID_HANDLE_VALUE) {
            while (m_Fence->GetCompletedValue() < value)
                ;
        } else if (m_Fence->GetCompletedValue() < value) {
            HRESULT hr = m_Fence->SetEventOnCompletion(value, m_Event);
            RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "ID3D12Fence::SetEventOnCompletion()  failed!");

            uint32_t result = WaitForSingleObjectEx(m_Event, TIMEOUT_FENCE, TRUE);
            RETURN_ON_FAILURE(&m_Device, result == WAIT_OBJECT_0, ReturnVoid(), "WaitForSingleObjectEx()  failed!");
        }
    } else {
        HRESULT hr = S_FALSE;
        while (hr == S_FALSE)
            hr = m_Device.GetImmediateContext()->GetData(m_Query, nullptr, 0, 0);

        RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "D3D11DeviceContext::GetData()  failed!");
    }
}

#include "FenceD3D11.hpp"
