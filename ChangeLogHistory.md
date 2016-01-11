## Table of Contents ##


---


# V0.0.006 -> V0.0.007 #
**Backend Stuff:**
  * None

**Frontend Stuff:**
  * The bookmark system has been updated a tad. You are now presented with a small window whenever you add a bookmark, where you will be able to modify the name and url of the bookmark.
  * The download system has been slightly modified. It will now be easier to use and understand. All downloads are currently saved to the **downloads** folder inside the _Symphony Browser_ folder. This will be made configurable soon.

**Fixed Bugs:**
  * Downloading files should now work fine all the time.

# V0.0.005 -> V0.0.006 #
**Backend Stuff:**
  * Cache system added
  * Minor optimizations

**Frontend Stuff:**
  * None

**Fixed Bugs:**
  * Bug that was disabling flash and other plugins has been fixed
  * The tab bar has had several moderate visual bugs patched, and should now be in tip-top condition


# V0.0.004 -> V0.0.005 #
**Backend Stuff:**
  * None

**Frontend Stuff:**
  * Tab Status bar removed
  * Tab preview added

**Fixed Bugs:**
  * Website favicons are now deleted every time you start up the browser. This should help reduce the amount of website icon glitches that the browser has.


# V0.0.003 -> V0.0.004 #
**Backend Stuff:**
  * The browser object is now directly connected to the tab button
  * Minor optimizations

**Frontend Stuff:**
  * One or two minor text changes

**Fixed Bugs:**
  * Attempting to close more than one tab while the window is open causes a crash.
  * The browser should now successfully save your tab session at all times.



# V0.0.002 -> v0.0.003 #
**Backend Stuff:**
  * A few minor bug fixes

**Frontend Stuff:**
  * A few minor UI improvements
  * A completely new, unique, and intuitive system that works very similar to normal browser tabs.

**Fixed Bugs:**
  * The NoScript manager icon sometimes does not show up if it goes from disabled to enabled state.
  * "Invalid Url" sometimes shows up in the status bar even when the URL connects fine.
  * The favicon doesn't load correctly sometimes.
  * The address bar is set to a different tab's URL. This happens more often when lots of tabs are open. This is being worked on.
  * If a site fails to load, AdBlock may ask you if you want to have the site bypass adblock. Doing this may cause a crash.