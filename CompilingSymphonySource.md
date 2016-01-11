# Introduction #

Symphony currently uses **Qt 4.7.3** and **C++**.

Symphony is compiled with OpenSSL, boost, as well as with several other configuration flags.

# Details #

The Qt source can be downloaded from:

http://qt.nokia.com/downloads/downloads#qt-lib

Qt _needs_ to be compiled from source. Qt needs to be version **4.7.3**.
Qt needs to be configured pre-compilation with these flags:

`-prefix c:\qt\4.7.3 -qt-gif -qt-libpng -qt-libjpeg -qt-libmng -qt-libtiff -platform win32-msvc2008 -openssl-linked -I C:\OpenSSL-Win32\include -L C:\OpenSSL-Win32\lib`

Please note that _**c:\qt\4.7.3**_ should be changed to the base directory of where your Qt 4.7.3 lies. (the src folder should be in this directory. i.e. c:\qt\4.7.3\src)

You will also need to note that _**C:\OpenSSL-Win32\lib**_ and _**C:\OpenSSL-Win32\include**_ need to be changed to valid directories, pointing at a compiled version of OpenSSL.

## boost ##
boost is one of the libraries that Symphony requires while compiling.

boost can be downloaded at:

http://www.boost.org/users/download

## OpenSSL ##

Symphony relies on OpenSSL to handle most of the SSL-related worked.

More info, including download links, involving OpenSSL can be found at:

http://www.openssl.org

## QtWebkit ##

QtWebkit is Symphony's backend web engine. It is a branch of Apple Webkit designed specially for Qt.

It is also highly recommended that you use the latest version of QtWebkit.

QtWebkit needs to be compiled seperately from Qt.

Currently, Symphony uses the latest - currently beta - version of QtWebkit 2.2. More info about QtWebkit 2.2 here:

http://trac.webkit.org/wiki/QtWebKitRelease22


# Running Symphony after compilation #

Symphony requires these libraries from **OpenSSL**:

  * libeay32
  * libssl32
  * ssleay32

Symphony requires these libraries from **Qt 4.7.3**:

  * phonon4
  * QtCore4
  * QtGui4
  * QtNetwork4
  * QtWebkit4
  * QtXml4

All of the aforementioned libraries need to be compiled and included in the same folder as the Symphony binary file.

## Other Info ##

You will also need to create a new folder inside the folder that contains all of the libraries and the Symphony binary file.

Name this folder _**data**_.

Then, inside the _**data**_ folder, create a folder named _**cache**_.

You will also need to copy the _**imageformats**_ folder from the Qt source (usually found at _**/src/plugins/imageformats**_) in to the folder that contains the Symphony binary file.