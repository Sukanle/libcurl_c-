#include "_curl.h"
#include <iostream>

size_t writeCallback(char *buf, size_t size, size_t nmemb, void *output) {
    if(output==nullptr)
        return 0;
    std::string& tmp = *static_cast<std::string*>(output);
    size_t realsize = size * nmemb;
    tmp.append(buf, realsize);
    printf("writeCallback: %s\n", tmp.c_str());
    return realsize;
}

int main()
{
    web::curl_easy::global_init();

    // web::curl_easy curl_0;
    std::string htmlData;


    // 使用make_curl_easy创建curl_easy对象
    // web::curl_easy curl_0 = web::make_curl_easy();
    // std::unique_ptr<web::curl_easy> curl_0 = web::make_curl_easy<true>();

    // 使用options一次性设置多个选项
    // std::vector<std::pair<CURLoption,web::OptionValue>> options;
    // options.emplace_back(std::make_pair(CURLOPT_URL, "https://www.bilibili.com"));
    // options.emplace_back(std::make_pair((CURLoption)9999, "https://www.bilibili.com"));
    // options.emplace_back(std::make_pair(CURLOPT_WRITEFUNCTION, writeCallback));
    // options.emplace_back(std::make_pair(CURLOPT_WRITEDATA, &htmlData));

    // 使用make_curl_easy创建curl_easy对象并设置选项
    // web::curl_easy curl_0;
    // try{
        // curl_0 = web::make_curl_easy<false>(options);
        // curl_0.setOption(options);
    // }catch(const std::exception& e){
        // std::cerr << e.what() << std::endl;
        // curl_0.setOption(CURLOPT_URL, "https://www.bilibili.com");
    // }

    // 使用setOption单独设置选项
    // curl_0.setOption(CURLOPT_URL, "https://www.bilibili.com");
    // curl_0.setOption(CURLOPT_WRITEFUNCTION, writeCallback);
    // curl_0.setOption(CURLOPT_WRITEDATA, &htmlData);

    // 使用setOption一次性递归设置多个选项
    // curl_0.setOption(CURLOPT_URL, "https://www.bilibili.com",CURLOPT_WRITEFUNCTION, writeCallback,CURLOPT_WRITEDATA, &htmlData);
    // auto curl_0=web::make_curl_easy(CURLOPT_URL, "https://www.bilibili.com",CURLOPT_WRITEFUNCTION, writeCallback,CURLOPT_WRITEDATA, &htmlData);

    // 使用make_curl_easy创建curl_easy对象并设置选项,一次性递归设置多个选项
#ifdef CURL_ERROR_ON
    try{
        curl_0 = web::make_curl_easy<false>((CURLoption)9999, "https://www.bilibili.com",CURLOPT_WRITEFUNCTION, writeCallback,CURLOPT_WRITEDATA, &htmlData);
    }
    catch(const std::exception& e){
        fprintf(stderr, "%s\n", e.what());
        curl_0.setOption(CURLOPT_URL, "https://www.bilibili.com");

    }
#else
    auto curl_0 = web::make_curl_easy<true>((CURLoption)9999, "https://www.bilibili.com",CURLOPT_WRITEFUNCTION, writeCallback,CURLOPT_WRITEDATA, &htmlData);
    fprintf(stderr, "curl_0.getErrorText(): %s\n", curl_0->getErrorText());
#endif
    curl_0->setOption(CURLOPT_URL, "https://www.bilibili.com",CURLOPT_WRITEFUNCTION, writeCallback,CURLOPT_WRITEDATA, &htmlData);

    // curl_0.perform();
    curl_0->perform();

    // printf("web::OptionValue的大小：%ld\n",sizeof(web::OptionValue));
    // printf("web::curl_easy的大小：%ld\n",sizeof(web::curl_easy));

    return 0;
}
