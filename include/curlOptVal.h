#pragma once
#ifndef CPP_CURL_OPTVAL_H
#define CPP_CURL_OPTVAL_H 1

#include <curl/curl.h>
#include <fstream>
namespace web{
        // 枚举类型，用于标识数据类型
    enum fd_type{
        fd_read=0,
        fd_write,
        fd_except
    };


    using curl_wr_h_callback = size_t(*)(char*,size_t,size_t,void*);// write read header
    using curl_pg_callback_v4l = int(*)(void*,curl_off_t,curl_off_t,curl_off_t,curl_off_t);
    using curl_pg_callback_v4d = int(*)(void*,double,double,double,double);
    using curl_ssl_ctx_callback = CURLcode(*)(CURL*,void*,void*);
    using curl_ssh_keycallback = int(*)(CURL*,const curl_khkey*,const curl_khkey,enum curl_khmatch,void*);
    using curl_host_keycallback = int(*)(void*,int,const char*,size_t);
    using curl_sockopt_callback = int(*)(void*,curl_socket_t,curlsocktype);
    using curl_seek_callback = int(*)(void*,curl_off_t,int);
    using curl_resolver_start_callback = int(*)(void*,void*,void*);
    using curl_prereq_callback = int(*)(void*,char*,char*,int,int);
    using curl_opensocket_callback = curl_socket_t(*)(void*,curlsocktype,struct curl_sockaddr*);
    using curl_ioctl_callback = int(*)(CURL*,int,void*);
    using curl_interleave_callback = size_t(*)(void*,size_t,size_t,void*);
    using curl_hstswrite_callback = size_t(*)(void*,struct curl_hstsentry,struct curl_index,void*);
    using curl_hstsread_callback = size_t(*)(CURL*,struct curl_hstsentry*,void*);
    using curl_fnmatch_callback = int(*)(void*,const char*,const char*);
    using curl_debug_callback = int(*)(CURL*,curl_infotype,char*,size_t,void*);
    using curl_conv_callback = CURLcode(*)(char*,size_t);
    using curl_closesocket_callback = void(*)(void*,curl_socket_t);
    using curl_chunk_end_callback = int(*)(void*);
    using curl_chunk_bgn_callback = int16_t(*)(void*,void*,int);
    using curl_push_callback = int(*)(CURL*,CURL*,size_t,struct curl_pushheaders*,void*);
    using curl_socket_callback = int(*)(CURL*,curl_socket_t,int,void*,void*);
    using curl_timer_callback = int(*)(CURL*,int64_t,void*);

    union OptionValue{
        void *_pointer; // CURL*,CURLSH
        int _integer_; // curl_socket_t
        int *_pinteger_; // curl_socket_t*
        long _integer; // curl_off_t
        long*_pinteger; // curl_off_t*
        char *_token;
        char **servers;
        const char *_text;
        double *_pdouble;

        curl_wr_h_callback _wr_callback;
        curl_pg_callback_v4l _pg_callback_v4l;
        curl_pg_callback_v4d _pg_callback_v4d;
        curl_ssl_ctx_callback _ssl_ctx_callback;
        curl_ssh_keycallback _ssh_keycallback;
        curl_host_keycallback _host_keycallback;
        curl_sockopt_callback _sockopt_callback;
        curl_seek_callback _seek_callback;
        curl_resolver_start_callback _resolver_start_callback;
        curl_prereq_callback _prereq_callback;
        curl_opensocket_callback _opensocket_callback;
        curl_ioctl_callback _ioctl_callback;
        curl_interleave_callback _interleave_callback;
        curl_hstswrite_callback _hstswrite_callback;
        curl_hstsread_callback _hstsread_callback;
        curl_fnmatch_callback _fnmatch_callback;
        curl_debug_callback _debug_callback;
        curl_conv_callback _conv_callback;
        curl_closesocket_callback _closesocket_callback;
        curl_chunk_end_callback _chunk_end_callback;
        curl_chunk_bgn_callback _chunk_bgn_callback;
        curl_push_callback _push_callback;
        curl_socket_callback _socket_callback;
        curl_timer_callback _timer_callback;

        FILE *_stream;
        curl_mime *_mime;
        struct curl_slist *_slist;
        struct curl_slist **_pslist;
        struct curl_blob *_blob;
        struct curl_httppost *_httppost;
        struct curl_tlssessioninfo *_tls;
        struct curl_tlssessioninfo **p_tls;
        struct curl_certinfo *_certinfo;
        struct curl_certinfo **p_certinfo;

        explicit OptionValue(void *p):_pointer(p){}
        explicit OptionValue(int i):_integer_(i){}
        explicit OptionValue(int *i):_pinteger_(i){}
        explicit OptionValue(int64_t i):_integer(i){}
        explicit OptionValue(int64_t *i):_pinteger(i){}
        explicit OptionValue(char *t):_token(t){}
        explicit OptionValue(char **s):servers(s){}
        explicit OptionValue(const char *t):_text(t){}
        explicit OptionValue(double *d):_pdouble(d){}

        explicit OptionValue(curl_wr_h_callback fn):_wr_callback(fn){}
        explicit OptionValue(curl_pg_callback_v4l fn):_pg_callback_v4l(fn){}
        explicit OptionValue(curl_pg_callback_v4d fn):_pg_callback_v4d(fn){}
        explicit OptionValue(curl_ssl_ctx_callback fn):_ssl_ctx_callback(fn){}
        explicit OptionValue(curl_ssh_keycallback fn):_ssh_keycallback(fn){}
        explicit OptionValue(curl_host_keycallback fn):_host_keycallback(fn){}
        explicit OptionValue(curl_sockopt_callback fn):_sockopt_callback(fn){}
        explicit OptionValue(curl_seek_callback fn):_seek_callback(fn){}
        explicit OptionValue(curl_resolver_start_callback fn):_resolver_start_callback(fn){}
        explicit OptionValue(curl_prereq_callback fn):_prereq_callback(fn){}
        explicit OptionValue(curl_opensocket_callback fn):_opensocket_callback(fn){}
        explicit OptionValue(curl_ioctl_callback fn):_ioctl_callback(fn){}
        explicit OptionValue(curl_interleave_callback fn):_interleave_callback(fn){}
        explicit OptionValue(curl_hstswrite_callback fn):_hstswrite_callback(fn){}
        explicit OptionValue(curl_hstsread_callback fn):_hstsread_callback(fn){}
        explicit OptionValue(curl_fnmatch_callback fn):_fnmatch_callback(fn){}
        explicit OptionValue(curl_debug_callback fn):_debug_callback(fn){}
        explicit OptionValue(curl_conv_callback fn):_conv_callback(fn){}
        explicit OptionValue(curl_closesocket_callback fn):_closesocket_callback(fn){}
        explicit OptionValue(curl_chunk_end_callback fn):_chunk_end_callback(fn){}
        explicit OptionValue(curl_chunk_bgn_callback fn):_chunk_bgn_callback(fn){}
        explicit OptionValue(curl_push_callback fn):_push_callback(fn){}
        explicit OptionValue(curl_socket_callback fn):_socket_callback(fn){}
        explicit OptionValue(curl_timer_callback fn):_timer_callback(fn){}

        explicit OptionValue(FILE *f):_stream(f){}
        explicit OptionValue(curl_mime *mime):_mime(mime){}
        explicit OptionValue(struct curl_slist *slist):_slist(slist){}
        explicit OptionValue(struct curl_slist **slist):_pslist(slist){}
        explicit OptionValue(struct curl_blob *blob):_blob(blob){}
        explicit OptionValue(struct curl_httppost *httppost):_httppost(httppost){}
        explicit OptionValue(struct curl_tlssessioninfo *tls):_tls(tls){}
        explicit OptionValue(struct curl_tlssessioninfo **tls):p_tls(tls){}
        explicit OptionValue(struct curl_certinfo *certinfo):_certinfo(certinfo){}
        explicit OptionValue(struct curl_certinfo **certinfo):p_certinfo(certinfo){}
    };
}// namespace web

#endif // _CURL_OPT_VAL_H
