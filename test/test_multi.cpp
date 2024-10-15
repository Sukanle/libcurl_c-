#include "_curl.h"
#include <cstring>
#include <mutex>
#include <condition_variable>

#define MAX_URL_LEN 1024
#define NUM_URLS sizeof(urls)/sizeof(char *)
#define MAX_PARALLEL 10

char *urls[]={
    "https://www.microsoft.com",
    "https://opensource.org",
    "https://apple.com",
    "https://www.bilibili.com",
    "https://www.baidu.com",
};

struct st_res_info {
    int8_t index;
    int8_t consumable;
    char *url[MAX_URL_LEN];
};

struct st_res_info res_list[NUM_URLS];
std::atomic_flag lock = ATOMIC_FLAG_INIT;
std::mutex mtx;
uint tranfer = 0;

static size_t wirte_cb(const char *data, size_t size, size_t nmemb,void *userp)
{
    (void)data;
    (void)userp;
    return size*nmemb;
}

static void add_tranfer(web::curl_multi &multi,int32_t i)
{
    web::curl_easy easy = web::make_curl_easy();
    easy.setOption(CURLOPT_WRITEFUNCTION,wirte_data);
    easy.setOption(CURLOPT_URL,urls[i]);
    easy.setOption(CURLOPT_TIMEOUT,10L);
    easy.setOption(CURLOPT_PRIVATE,urls[i]);
    multi.addHandle(easy);
}

int find_index_of_urls(char *url)
{
    for(int i=0; i<NUM_URLS; i++)
        if(strcmp(urls[i],url)==0)
            return i;
    return -1;
}

void add_response(st_res_info res)
{
    web::lock_guard lock_guard(lock);
    res_list[res.index]=res;
    res_list[res.index].consumable=1;
}

void consume_response()
{
    std::lock_guard<std::mutex> lock_guard(mtx);
    for(int i=0;i<NUM_URLS;++i)
        if(res_list[i].consumable==1){
            printf("Log: t2, consume response, index=%d, url=%s\n",
                    res_list[i].index,res_list[i].url);
            res_list[i].consumable=2;
        }
}
int check_consume_finished()
{
    int finished=1;
    std::lock_guard<std::mutex> lock_guard(mtx);
    for(int i=0;i<NUM_URLS;++i)
        if(res_list[i].consumable!=2){
            finished=0;
            break;
        }
    return finished;
}
void *thd_fn1()
{
    puts("Log: t1 begin...");

    web::curl_multi::global_init();
    web::curl_multi::auto_manage();
    web::curl_multi multi;

    CURLMsg *msg;

    int msgs_left=-1;

    multi.setOption(CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

    for(tranfer=0;tranfer<NUM_URLS;tranfer++)
        add_tranfer(multi,tranfer);
    do{
        multi.perform();
    }while(multi.getEasyExtant()!=0 || (tranfer<NUM_URLS));
}
