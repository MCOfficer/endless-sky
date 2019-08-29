/* Networking.h
Copyright (c) 2019 by MCOfficer
Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/


#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>

class Networking
{
	public:
		static bool DownloadToFile(const std::string &url, const std::string &destinationFile);
		static bool DownloadToString(const std::string &url, std::string *buffer);

	protected:

	private:
		static bool DownloadData(const std::string &url, void *destPtr, bool toFile);
};

#endif
