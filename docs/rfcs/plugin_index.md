# Plugin Index

This is a specification for the creation and format of a central Plugin Index, as discussed in [endless-sky-plugins#21](https://github.com/EndlessSkyCommunity/endless-sky-plugins/pull/21).


## Intended users

Intended first-level users are applications that can query the index in order to display, install or update Plugins. Examples include [ESLauncher2](https://github.com/EndlessSkyCommunity/ESLauncher2/), Endless Sky's website (as discussed in [endless-sky.github.io#21](https://github.com/endless-sky/endless-sky.github.io/pull/21)) and Endless Sky itself (as discussed in [ES-#707](https://github.com/endless-sky/endless-sky/issues/707)).

Second-level users are players that want a convenient way to discover or install Plugins.


## Scope limits

Out of scope for this specification are:
- Behavior, implementation or appearance of Applications using this index.
- Version-incompatibility between Plugins and Endless Sky.


# Structure

- The Plugin Index takes the form of a Git repository.
- The repository contains:
  - A human- and machine-readable file that containing a list of Plugins, hereafter referred to as "index file".
  - Scripts that automatically read from the index file and convert it to other machine-readable formats, for maximum compatibility.
  - A folder containing the files generated from the index files.
  - A script that is able to check Plugins listed in the index file for new versions.

## The Index File

The index file contains a list of Plugins. Each Plugin is a map with the following contents. Unless otherwise stated, the associated values are Strings.

- `name`: A unique name identifying the Plugin
- `authors`: One or more authors of the Plugin, for example `Somename, SomeOtherName & Contributors`
- `homepage`: A valid URL pointing at a page containing more Information about the Plugin.
- `license`: A license identifier in accordance with the [SPDX license list](https://spdx.org/licenses/).
- `version`: The Plugin's version, as currently indexed.
- `url`: A valid URL pointing directly (redirects are allowed) at a Zip-archive containing release `version` of the Plugin.
- `iconUrl`: A valid URL pointing directly (redirects are allowed) at a PNG- or JPEG-image that is used as the Plugin's thumbnail. The image's should preferably be 160x160 pixels large and may have a transparent background.
- `autoupdate`: A map containing information that can be used by aforementioned update script to check for new versions and automatically update the Plugin's key-value pairs accordingly.
  - `type`: A String identifying the method used to check for new versions. Examples may include `git`, which checks the git repository specified in `update_url` for newer tags, or `github-release`, which checks the github repository specified in `update_url` for newer releases.
  - `update_url`: A valid URL complementing `type`. May be omitted, in which case `homepage` shall be used instead`.
  - Any other key-value pairs will be interpreted as update keys:
    - They may containg `$version` as a substitute key
    - Upon detecting a new version, all occurences of `$version` will be replaced with the version detected using `type`.
    - Then, they will replace the Plugin's keys of the same names.
    - For example: A universally-used update key should be `url`, with a value similar to `https://example.com/myPlugin/$version.zip`. Upon detecting a new version (say, `v1.2`), the Plugin's `url` key will be replaced with `https://example.com/myPlugin/v1.2.zip`.
    
## The generated files

The generated files should be updated by a CI service upon every change to the index file. They may take the following forms (or others):
- A JSON-file, containing all Plugins
- A number of JSON-files, each named after a plugin which it contains
- A .txt file in a format readable by Endless Sky, containing all Plugins.

    
