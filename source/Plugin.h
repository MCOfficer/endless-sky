/* Plugin.h
Copyright (c) 2019 by MCOfficer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <string>

class DataNode;

class Plugin
{
	public:
		void Load(const DataNode &node);
		
		const std::string &Url() const;
		const std::string &Checkout() const;
		const std::string &Version() const;
		const std::string &IconUrl() const;
		const std::string &Description() const;

	private:
		std::string url;
		std::string checkout;
		std::string version;
		std::string iconUrl;
		std::string description;
};

#endif
