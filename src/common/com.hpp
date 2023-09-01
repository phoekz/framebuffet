#pragma once

#include "pch.hpp"

namespace fb {

// Adapted from: https://github.com/microsoft/wil

template<typename T>
class ComPtr {
    static_assert(std::is_base_of_v<IUnknown, T>);

public:
    // Constructors.
    ComPtr()
        : _ptr(nullptr) {}
    ComPtr(std::nullptr_t)
        : _ptr(nullptr) {}
    ComPtr(T* ptr) {
        _ptr = ptr;
        if (_ptr) {
            _ptr->AddRef();
        }
    }
    ComPtr(const ComPtr& other)
        : ComPtr(other.get()) {}
    template<typename U>
        requires std::is_convertible_v<U*, T*>
    ComPtr(const ComPtr<U>& other)
        : ComPtr(other.get()) {}
    ComPtr(ComPtr&& other)
        : _ptr(other.detach()) {}
    template<typename U>
        requires std::is_convertible_v<U*, T*>
    ComPtr(ComPtr<U>&& other)
        : _ptr(other.detach()) {}

    // Destructor.
    ~ComPtr() {
        if (_ptr) {
            _ptr->Release();
        }
    }

    // Assignment operators.
    ComPtr& operator=(std::nullptr_t) {
        reset();
        return *this;
    }
    ComPtr& operator=(T* other) {
        auto ptr = _ptr;
        _ptr = other;
        if (_ptr) {
            _ptr->AddRef();
        }
        if (ptr) {
            ptr->Release();
        }
        return *this;
    }
    ComPtr& operator=(const ComPtr& other) { return operator=(other.get()); }
    template<typename U>
        requires std::is_convertible_v<U*, T*>
    ComPtr& operator=(const ComPtr<U>& other) {
        return operator=(static_cast<T*>(other.get()));
    }
    ComPtr& operator=(ComPtr&& other) {
        attach(other.detach());
        return *this;
    }
    template<typename U>
        requires std::is_convertible_v<U*, T*>
    ComPtr& operator=(ComPtr<U>&& other) {
        attach(other.detach());
        return *this;
    }

    // Modifiers.
    auto reset() {
        auto ptr = _ptr;
        _ptr = nullptr;
        if (ptr) {
            ptr->Release();
        }
    }
    void attach(T* other) {
        auto ptr = _ptr;
        _ptr = other;
        if (ptr) {
            ptr->Release();
        }
    }
    auto detach() -> T* {
        auto ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    // Getters.
    auto get() -> T* { return _ptr; }
    auto get() const -> T* { return _ptr; }
    auto addressof() -> T** { return &_ptr; }
    auto addressof() const -> T* const* { return &_ptr; }
    auto operator->() -> T* { return _ptr; }
    auto operator->() const -> T* { return _ptr; }
    auto operator&() -> T** {
        reset();
        return &_ptr;
    }

    // Implicit conversion.
    explicit operator bool() const { return _ptr != nullptr; }

    // Query interface.
    template<class U>
    auto query_to(_COM_Outptr_ U** result) -> HRESULT {
        return _ptr->QueryInterface(IID_PPV_ARGS(result));
    }

private:
    T* _ptr;
};

} // namespace fb
