#include <cstring>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <signal.h>

#include "curl.h"

std::ofstream logfile("log.txt", std::ios::app);
std::ofstream webfile("web.html");

char* start_page[] = {
    "https://www.baidu.com",
    // "https://cn.bing.com",
    // "https://www.bilibili.com",
    "https://www.taobao.com",
    // "https://www.jd.com",
};

constexpr int max_con = 200;
constexpr int max_total = 20000;
constexpr int max_requests = 500;
constexpr int max_link_per_page = 100;
constexpr int follow_relative_links = 0;

int pending_interrupt = 0;
void closeIOandsignal(bool bcloseio)
{
    int ii = 0;

    for (; ii < 64; ii++) {
        if (bcloseio == true)
            ::close(ii);

        signal(ii, SIG_IGN);
    }
}
void sighandler(int dummy) { pending_interrupt = 1; }

size_t grow_buffer(void* contents, size_t sz, size_t nmemb, void* ctx)
{
    size_t realsize = sz * nmemb;
    auto* mem = static_cast<std::string*>(ctx);
    mem->append(static_cast<char*>(contents), realsize);
    return realsize;
}

auto make_handle(const std::string& url) -> web::curl_easy*
{
    printf("url: %s\n", url.c_str());
    web::curl_easy* easy = new web::curl_easy();
    /* Important: use HTTP2 over HTTPS */
    easy->setOption(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    easy->setOption(CURLOPT_URL, url.c_str());

    // auto mem = std::make_shared<std::string>();
    std::string* mem = new std::string();

    easy->setOption(CURLOPT_WRITEFUNCTION, grow_buffer);
    easy->setOption(CURLOPT_WRITEDATA, mem);
    easy->setOption(CURLOPT_PRIVATE, mem);

    /* For completeness */
    easy->setOption(CURLOPT_ACCEPT_ENCODING, "");
    easy->setOption(CURLOPT_TIMEOUT, 5L);
    easy->setOption(CURLOPT_FOLLOWLOCATION, 1L);
    /* only allow redirects to HTTP and HTTPS URLs */
    easy->setOption(CURLOPT_REDIR_PROTOCOLS, "http,https");
    easy->setOption(CURLOPT_AUTOREFERER, 1L);
    easy->setOption(CURLOPT_MAXREDIRS, 10L);
    /* each transfer needs to be done within 20 seconds! */
    easy->setOption(CURLOPT_TIMEOUT_MS, 20000L);
    /* connect fast or fail */
    easy->setOption(CURLOPT_CONNECTTIMEOUT_MS, 2000L);
    /* skip files larger than a gigabyte */
    easy->setOption(CURLOPT_MAXFILESIZE_LARGE, static_cast<long>(1024 * 1024 * 1024));
    easy->setOption(CURLOPT_COOKIEFILE, "");
    easy->setOption(CURLOPT_FILETIME, 1L);
    easy->setOption(CURLOPT_USERAGENT, "mini crawler");
    easy->setOption(CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    easy->setOption(CURLOPT_UNRESTRICTED_AUTH, 1L);
    easy->setOption(CURLOPT_PROXYAUTH, CURLAUTH_ANY);
    easy->setOption(CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);
    return easy;
}

/* HREF finder implemented in libxml2 but could be any HTML parser */
size_t follow_links(web::curl_multi& multi, std::string* mem, char* url)
{
    int opts = HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET;
    htmlDocPtr doc = htmlReadMemory(mem->c_str(), mem->length(), url, NULL, opts);
    if (!doc)
        return 0;
    xmlChar* xpath = BAD_CAST("//a/@href");
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if (!result)
        return 0;
    xmlNodeSetPtr nodeset = result->nodesetval;
    if (xmlXPathNodeSetIsEmpty(nodeset)) {
        xmlXPathFreeObject(result);
        return 0;
    }
    size_t count = 0;
    for (int i = 0; i < nodeset->nodeNr; i++) {
        double r = rand();
        int x = r * nodeset->nodeNr / RAND_MAX;
        const xmlNode* node = nodeset->nodeTab[x]->xmlChildrenNode;
        xmlChar* href = xmlNodeListGetString(doc, node, 1);
        if (follow_relative_links) {
            xmlChar* orig = href;
            href = xmlBuildURI(href, (xmlChar*)url);
            xmlFree(orig);
        }
        // char* link = (char*)href;
        char* link = reinterpret_cast<char*>(href);
        if (!link || strlen(link) < 20)
            continue;
        if (!strncmp(link, "http://", 7) || !strncmp(link, "https://", 8)) {
            // curl_multi_add_handle(multi_handle, make_handle(link));
            multi.addHandle(make_handle(link));
            if (count++ == max_link_per_page)
                break;
        }
        xmlFree(link);
    }
    xmlXPathFreeObject(result);
    return count;
}

int is_html(char* ctype)
{
    return ctype != nullptr && strlen(ctype) > 10 && strstr(ctype, "text/html");
}

int main()
{
    // closeIOandsignal(true);

    // if (fork() != 0)
    //     exit(0);

    // if (daemon(1, 1) < 0) {
    //     logfile.write("error daemon...\n", 17);
    //     exit(-1);
    // }
    signal(SIGINT, sighandler);
    LIBXML_TEST_VERSION;
    web::curl_global::global_init();
    auto multi = web::curl_multi();
    multi.setOption(CURLMOPT_MAX_TOTAL_CONNECTIONS, max_con);
    multi.setOption(CURLMOPT_MAX_HOST_CONNECTIONS, 6L);

    /* enables http/2 if available */
#ifdef CURLPIPE_MULTIPLEX
    multi.setOption(CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
#endif

    /* sets html start page */
    for (int i = 0; i < sizeof(start_page) / sizeof(start_page[0]); ++i)
        multi.addHandle(make_handle(start_page[i]));

    int msgs_left = 0;
    int pending = 0;
    int complete = 0;
    while (multi.getEasyExtant() && !pending_interrupt) {
        int numfds;
        multi.wait(nullptr, 0, 1000, numfds);
        printf("numfds: %d\n", numfds);
        multi.perform();

        /* See how the transfers went */
        web::curl_msg m;

        while ((m = web::curl_msg(multi.info_read(&msgs_left), multi)))
        {
            if (m.getMsg()->msg == CURLMSG_DONE) {
                auto& easy = *m.getEasy();
                char* url;
                std::string* mem;
                easy.getinfo(CURLINFO_PRIVATE, mem);
                easy.getinfo(CURLINFO_EFFECTIVE_URL, url);
                if (m.getResult() == CURLE_OK) {
                    long res_status;
                    easy.getinfo(CURLINFO_RESPONSE_CODE, res_status);
                    if (res_status == 200) {
                        char* ctype;
                        easy.getinfo(CURLINFO_CONTENT_TYPE, ctype);
                        webfile << fmt::format("[{}] HTTP 200 ({}): {}\n", complete, ctype, url);
                        if (is_html(ctype) && mem->length() > 100) {
                            if (pending < max_requests && (complete + pending) < max_total) {
                                pending += follow_links(multi, mem, url);
                            }
                        }
                    } else {
                        webfile << fmt::format("[{}] HTTP {}: {}\n", complete, res_status, url);
                    }
                } else {
                    logfile << fmt::format("[{}] Connection failure: {}\n", complete, url);
                }
                multi.rmHandle(easy);
                web::curl_forced_cleanup(easy);
                complete++;
                pending--;
            }
        }
    }
    // web::curl_easy::sub_easy_extant();

    logfile.close();
    webfile.close();

    return 0;
}
