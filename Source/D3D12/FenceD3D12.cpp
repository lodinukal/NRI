// © 2021 NVIDIA Corporation

#include "SharedD3D12.h"

#include "CommandQueueD3D12.h"
#include "FenceD3D12.h"

using namespace nri;

Result FenceD3D12::Create(uint64_t initialValue) {
    HRESULT hr = m_Device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
    RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device::CreateFence()");
    m_Event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    return Result::SUCCESS;
}

//================================================================================================================
// NRI
//================================================================================================================

inline uint64_t FenceD3D12::GetFenceValue() const {
    return m_Fence->GetCompletedValue();
}

void FenceD3D12::QueueSignal(CommandQueueD3D12& commandQueue, uint64_t value) {
    HRESULT hr = ((ID3D12CommandQueue*)commandQueue)->Signal(m_Fence, value);
    RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "ID3D12CommandQueue::Signal()  failed!");
}

void FenceD3D12::QueueWait(CommandQueueD3D12& commandQueue, uint64_t value) {
    HRESULT hr = ((ID3D12CommandQueue*)commandQueue)->Wait(m_Fence, value);
    RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "ID3D12CommandQueue::Wait()  failed!");
}

inline void FenceD3D12::Wait(uint64_t value) {
    if (m_Event == 0 || m_Event == INVALID_HANDLE_VALUE) {
        while (m_Fence->GetCompletedValue() < value)
            ;
    } else if (m_Fence->GetCompletedValue() < value) {
        HRESULT hr = m_Fence->SetEventOnCompletion(value, m_Event);
        RETURN_ON_FAILURE(&m_Device, hr == S_OK, ReturnVoid(), "ID3D12Fence::SetEventOnCompletion()  failed!");

        uint32_t result = WaitForSingleObjectEx(m_Event, TIMEOUT_FENCE, TRUE);
        RETURN_ON_FAILURE(&m_Device, result == WAIT_OBJECT_0, ReturnVoid(), "WaitForSingleObjectEx()  failed!");
    }
}

#include "FenceD3D12.hpp"
