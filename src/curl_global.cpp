#include "curl.h"

namespace web {
CURLcode curl_global::_global = CURLE_FAILED_INIT;
std::atomic_flag curl_global::_lock = ATOMIC_FLAG_INIT;

std::function<const struct curl_easyoption*(const struct curl_easyoption*)> getEasyOptNext;

auto getEasyOption(CURLoption id) noexcept -> const struct curl_easyoption*
{
    return curl_easy_option_by_id(id);
}

auto getEasyOption(const char* name) noexcept -> const struct curl_easyoption*
{
    return curl_easy_option_by_name(name);
}
void init_easyOptNext()
{
    getEasyOptNext = [](const struct curl_easyoption* opt) noexcept -> const struct curl_easyoption*
    {
        return curl_easy_option_next(opt);
    };
}

bool curl_global::global_init() NOEXCEPT
{
    auto_flag lock(_lock);

    if (_global == CURLE_OK)
        return true;
    _global = curl_global_init(CURL_GLOBAL_DEFAULT);
#ifdef CURL_ERROR_ON
    if (_global != CURLE_OK)
        THROW_CURL_ERROR(_global);
#else
    if (_global != CURLE_OK)
        return false;
#endif
    _lock.clear(std::memory_order_release);
    return true;
}
bool curl_global::global_cleanup() NOEXCEPT
{
    auto_flag lock(_lock);

    if (_global != CURLE_OK)
        return true;
    if (curl_easy::_easy_extant != 0 || curl_multi::_multi_extant != 0) {
#ifdef CURL_ERROR_ON
        throw std::runtime_error(std::string("Error: There are ")
            + std::to_string(curl_easy::_easy_extant + curl_multi::_multi_extant)
            + " curl_easy or curl_multi are still extant.");
#else
        fprintf(stderr, "Error: There are %zu curl_easy and %zu curl_multi are still extant.\n",
            curl_easy::_easy_extant.load(), curl_multi::_multi_extant.load());
#endif
        return false;
    }
    curl_global_cleanup();
    _global = CURLE_FAILED_INIT;
    return true;
}
} // namespace web
