/* Plugin.cpp
Copyright (c) 2019 by MCOfficer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Plugin.h"
#include "DataNode.h"

void Plugin::Load(const DataNode &node)
{
	name = node.Token(1);
	for(const DataNode &child : node)
	{
		if(child.Token(0) == "url" && child.Size() >= 2)
			url = child.Token(1);
		else if(child.Token(0) == "checkout" && child.Size() >= 2)
			checkout = child.Token(1);
		else if(child.Token(0) == "version" && child.Size() >= 2)
			version = child.Token(1);
		else if(child.Token(0) == "iconUrl" && child.Size() >= 2)
			iconUrl = child.Token(1);
		else if(child.Token(0) == "description" && child.Size() >= 2)
			description = child.Token(1);
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}
}

const std::string &Plugin::Name() const
{
	return name;
}

const std::string &Plugin::Url() const
{
	return url;
}

const std::string &Plugin::Checkout() const
{
	return checkout;
}

const std::string &Plugin::Version() const
{
	return &version ? version : checkout;
}

const std::string &Plugin::IconUrl() const
{
	return iconUrl;
}

const std::string &Plugin::Description() const
{
	return description;
}

const bool Plugin::isAvailable() const
{
	return &Url() != nullptr;
}
