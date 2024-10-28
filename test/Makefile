.PHONY: ALL clean
ALL:
	g++ ./crawler.cpp -o crawler -std=c++17 `pkg-config --cflags --libs libcurl libcurlc++ libxml-2.0` -g
	patchelf --set-rpath ../lib64/shared crawler
	@#g++ ./crawler.cpp ../src/_curl_easy.cpp ../src/_curl_multi.cpp ../src/_curl_global.cpp ../src/_curl_http.cpp -o crawler -std=c++17 -lcurl -I ../include -g -DCURL_ERROR_DETAILED
	@#-DCURL_ERROR_ENABLE
clean:
	@rm -f crawler
