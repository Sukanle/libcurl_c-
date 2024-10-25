#include "_curl.h"
#include <iostream>

inline size_t save_page(void *buffer, size_t size, size_t count, void *user_p);

int main()
{
    web::curl_global::global_init();
    web::curl_easy easy_handle1;
    web::curl_easy easy_handle2;
    web::curl_easy easy_handle3;
    web::curl_multi multi_handle;

    FILE *fp_bili=fopen("bilibili.html", "ab+");
    FILE *fp_baidu=fopen("baidu.html", "ab+");
    FILE *fp_bing=fopen("bing.html", "ab+");

    easy_handle1.setOption(CURLOPT_URL, "https://www.bilibili.com");
    easy_handle1.setOption(CURLOPT_WRITEFUNCTION, &save_page);
    easy_handle1.setOption(CURLOPT_WRITEDATA, fp_bili);

    easy_handle2.setOption(CURLOPT_URL, "https://www.baidu.com");
    easy_handle2.setOption(CURLOPT_WRITEFUNCTION, &save_page);
    easy_handle2.setOption(CURLOPT_WRITEDATA, fp_baidu);

    easy_handle3.setOption(CURLOPT_URL, "https://cn.bing.com");
    easy_handle3.setOption(CURLOPT_WRITEFUNCTION, &save_page);
    easy_handle3.setOption(CURLOPT_WRITEDATA, fp_bing);

    multi_handle.addHandle(easy_handle1);
    multi_handle.addHandle(easy_handle2);
    multi_handle.addHandle(easy_handle3);

    while (multi_handle.perform()==false)
        printf("running_handle_count: %d\n", multi_handle.getEasyExtant());

    while(multi_handle.getEasyExtant()!=0)
    {
        multi_handle.setTimeval(1, 0);
        multi_handle.fdset();
        bool return_code = multi_handle.select();
        if (return_code==false)
        {
            fprintf(stderr, "select error.\n");
            break;
        }
        while (multi_handle.perform()==false)
            printf("running_handle_count: %d\n", multi_handle.getEasyExtant());
    }

    fclose(fp_baidu);
    fclose(fp_bili);
    fclose(fp_bing);

    return 0;
}

size_t save_page(void *buffer, size_t size, size_t count, void *user_p)
{
    return fwrite(buffer, size, count, (FILE *)user_p);
}
