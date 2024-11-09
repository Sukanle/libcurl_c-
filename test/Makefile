.PHONY: ALL clean CPP C
ALL:CPP C

CPP:
	@# g++ ./crawler.cpp -o crawler -std=c++17 `pkg-config --cflags --libs libcurl libcurlc++ libxml-2.0 fmt` -g -DCURL_ERROR_DETAILED
	@# patchelf --set-rpath ../lib64/shared crawler
	g++ ./crawler.cpp ../src/curl_easy.cpp ../src/curl_multi.cpp ../src/curl_global.cpp ../src/curl_http.cpp ../src/curl_share.cpp -o crawler -std=c++17 -I ../include -g -DCURL_ERROR_DETAILED `pkg-config --cflags --libs libxml-2.0 fmt libcurl`
	@#-DCURL_ERROR_ENABLE
C:
	gcc ./crawler.c -o crawler_c `pkg-config --cflags --libs libcurl libxml-2.0` -g
clean:
	@rm -f crawler_c *.html *.txt
