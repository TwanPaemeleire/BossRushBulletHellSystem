#pragma once
#include <coroutine>

namespace Bloodforge
{
    struct Coroutine
    {
        struct promise_type
        {
            Coroutine get_return_object()
            {
                return Coroutine
                {
                    std::coroutine_handle<promise_type>::from_promise(*this)
                };
            }

            std::suspend_always initial_suspend() noexcept
            {
                return {};
            }

            std::suspend_always final_suspend() noexcept
            {
                return {};
            }

            void return_void() {}
            void unhandled_exception() {}
        };

        std::coroutine_handle<promise_type> handle;
    };

	int StartCoroutine(Coroutine coroutine);
	void StopCoroutine(int id);
	// void WaitForSeconds(float seconds);

    struct WaitNextFrameAwaiter
    {
        bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> handle) const noexcept;

        void await_resume() const noexcept {}
    };

    struct WaitForSecondsAwaiter
    {
        bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> handle) const noexcept;

        void await_resume() const noexcept {}

        float SecondsToWait = 0.0f;
    };

    inline WaitNextFrameAwaiter WaitUntilNextFrame()
    {
        return {};
    }

    inline WaitForSecondsAwaiter WaitForSeconds(float seconds)
    {
        return
        {
            seconds
        };
    }
}