# Introduction #

There are far too many features to list.
On top of the basic features that everyone expects from a modern browser [ex. Firefox or Chrome], Symphony has also implemented NoScript and AdBlock protection.


# Details #

AdBlock is separated in to two pieces. One for network blocking [download of ads](blocks.md) and one for website blocking [off dead/empty elements inside a website](kills.md). It takes all of its data directly from EasyList from AdBlockPlus. It auto-updates every time you run the browser.

NoScript is similar to the Firefox extension NoScript. Firefox's NoScript lets you pick and choose from a list of all javascript scripts on the site you are viewing, seperated by URL that the javascript script is coming from. It then lets you toggle them on or off. They are, by default, off.

Symphony's NoScript is different in the respect that is toggles javascript for the whole site - not per javascript script site. It also lets you choose if the default is on or off.


# Upcoming Features #

  * Download Manager
  * Bookmarks Folders & Manager
  * Cookies Manager
  * AdBlock Custom Rules & Manager
  * More Robust NoScript Manager
  * GreaseMonkey UserScript support
  * Form AutoFiller
  * Customizable UI