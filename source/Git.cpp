/* Git.cpp
Copyright (c) 2019 by MCOfficer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include <git2.h>
#include <git2/checkout.h>
#include <git2/remote.h>
#include <git2/repository.h>

#include "Git.h"

static git_repository* Open(std::string &directory)
{
	git_repository* repo;
	if(git_repository_open(&repo, directory.c_str()) != 0)
		return nullptr;
	return repo;
}

static bool Clone(std::string &url, std::string &directory)
{
	git_repository* out;
	return git_clone(&out, url.c_str(), directory.c_str(), NULL) == 0;
}

static bool HardReset(std::string &directory, std::string &revision)
{
	git_repository* repo = Open(directory);
	if(!repo)
		return false;
	
	git_strarray* remotes;
	if(git_remote_list(remotes, repo) != 0)
		return false;
	
	for(int i=0; i < remotes->count; i++)
	{
		git_remote* remote;
		if(git_remote_lookup(&remote, repo, remotes->strings[i]) != 0)
			return false;
		if(git_remote_fetch(remote, NULL, NULL, NULL) != 0)
			return false;
	}
	git_strarray_free(remotes);
	
	git_object* revisionObject;
	if (git_revparse_single(&revisionObject, repo, revision.c_str()) != 0)
		return false;
	
	git_checkout_options* checkoutOptions;
	if(git_checkout_init_options(checkoutOptions, GIT_CHECKOUT_OPTIONS_VERSION) != 0)
		return false;
	
	return git_reset(repo, revisionObject, GIT_RESET_HARD, checkoutOptions) == 0;
}
