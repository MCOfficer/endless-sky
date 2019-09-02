/* Git.h
Copyright (c) 2019 by MCOfficer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef GIT_H
#define GIT_H

#include <git2/repository.h>

#include <string>

class Git
{
	public:
		static git_repository* Open(std::string directory);
		static bool Clone(std::string &url, std::string &directory);
		static bool HardReset(std::string &directory, std::string &revision);
	protected:

	private:
};

#endif
