#include "_curl.h"

namespace web{
    CURLcode _global_init=CURLE_FAILED_INIT;

    auto getoption(CURLoption id) noexcept
        -> const struct curl_easyoption * {
        return curl_easy_option_by_id(id);
    }

    auto getoption(const char *name) noexcept
        -> const struct curl_easyoption * {
        return curl_easy_option_by_name(name);
    }
}
