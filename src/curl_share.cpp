#include "curl.h"
#include "curl_share.h"

namespace web {
curl_share::curl_share() noexcept
{
    _share = curl_share_init();
    if (_share == nullptr) {
        _error = CURLSHE_BAD_OPTION;
        _error_vec.push_back(_error);
    }
}
curl_share::curl_share(curl_share&& obj) noexcept
{
    if (this == &obj)
        return;
    _share = std::exchange(obj._share, nullptr);
    _error = std::exchange(obj._error, CURLSHE_OK);
    _error_vec = std::exchange(obj._error_vec, {});
}
curl_share::operator bool() const noexcept { return _share == nullptr; }
curl_share::operator CURLSH*() const noexcept { return _share; }
curl_share::~curl_share() noexcept
{
    if (_share == nullptr)
        return;
    curl_share_cleanup(_share);
}

curl_share& curl_share::operator=(curl_share&& obj) noexcept
{
    if (this == &obj)
        return *this;
    _share = std::exchange(obj._share, nullptr);
    _error = std::exchange(obj._error, CURLSHE_OK);
    _error_vec = std::exchange(obj._error_vec, {});
    return *this;
}
bool curl_share::setOption(curlsh_option& options) NOEXCEPT
{
    if (_share == nullptr)
        return false;
    for (const auto& opt : options) {
        if ((_error = curl_share_setopt(_share, opt.first, opt.second)) != CURLSHE_OK) {
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#endif
            return false;
        }
    }
    return true;
}

CURL* curl_share::getHandle() const noexcept { return _share; }
CURLSHcode curl_share::getError() const noexcept { return _error; }
const char* curl_share::getErrorText() const noexcept { return curl_share_strerror(_error); }
const std::vector<CURLSHcode>& curl_share::getErrorVec() const noexcept { return _error_vec; }
}; // namespace web
