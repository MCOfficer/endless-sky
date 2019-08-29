/* Networking.cpp
Copyright (c) 2019 by MCOfficer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Networking.h"
#include "Files.h"
#include "File.h"

#include <curl/curl.h>

#include <string>

namespace {
	// Linux can use a built-in function, but this function appears to be needed in Microsoft Windows environments
	size_t writeToFile(void *buffer, size_t size, size_t nmemb, FILE *ofile)
	{
		Files::LogError((char*)buffer);
		return fwrite(buffer, size, nmemb, ofile);
	}
	size_t writeToString(void *buffer, size_t size, size_t nmemb, std::string *userp)
	{
		Files::LogError((char*)buffer);
		userp->append((char*)buffer, size * nmemb);
		return size * nmemb;
	}
	
}

bool Networking::DownloadToFile(const std::string &url, const std::string &destinationFile)
{
	
	if(Files::Exists(destinationFile))
		return false;
	
	File destFilePtr = File(destinationFile, true);
	if(!destFilePtr)
		return false;
		
	return DownloadData(url, destFilePtr, true) && Files::Exists(destinationFile);
}

bool Networking::DownloadToString(const std::string &url, std::string *buffer)
{
	return DownloadData(url, buffer, false);
}

bool Networking::DownloadData(const std::string &url, void *destPtr, bool toFile)
{
	if(url.empty())
		return false;
	
	CURL *curlDl = curl_easy_init();
	if(!curlDl)
		return false;
	
	curl_easy_setopt(curlDl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlDl, CURLOPT_WRITEDATA, destPtr);
	
	if(toFile)
		curl_easy_setopt(curlDl, CURLOPT_WRITEFUNCTION, writeToFile);
	else
		curl_easy_setopt(curlDl, CURLOPT_WRITEFUNCTION, writeToString);
	
	// Some servers would block requests without user agent, so we gotta set it to something
	curl_easy_setopt(curlDl, CURLOPT_USERAGENT, "ES/libcurl");
	
	// Allow redirects, e.g. for downloads from Github
	curl_easy_setopt(curlDl, CURLOPT_FOLLOWLOCATION, 1L);
	
	char errbuf[CURL_ERROR_SIZE];
	curl_easy_setopt(curlDl, CURLOPT_ERRORBUFFER, errbuf);
	curl_easy_setopt(curlDl, CURLOPT_VERBOSE, 1L);
	errbuf[0] = 0;
	
	CURLcode result = curl_easy_perform(curlDl);
	
	if(result != CURLE_OK) {
		size_t len = strlen(errbuf);
		Files::LogError("libcurl: " + result);
		if(len)
			Files::LogError(errbuf);
		else
			Files::LogError(curl_easy_strerror(result));
	}
	
	curl_easy_cleanup(curlDl);
	return result == CURLE_OK;
}
