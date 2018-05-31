# list-apt-installs - Creates list from terminal logs of apt install command.


## What does this thing do?
It makes a list.

## Why would I use it?
It works good for use in a backup scheme.


## How can I use it?
You have to install it. You can use a package manager, or import it as and external package, or build/install it. Then simply run with command:
```
list-apt-installs
```

Or put the list into a file which you will save into your backup scheme for later:
```
list-apt-installs > my-installed-stuff.list
```


### Package manager installation:

Use some software manager to install, or some command like this:
```
apt install list-apt-installs
```
same as:
```
sudo apt-get install list-apt-installs
```

Remove with:
```
apt purge list-apt-installs
```
or:
```
sudo apt-get purge list-apt-installs
```


### Import:

Find the .deb file somewhere, then you can simply double-click it and follow the prompts from your package manager.

Or issue some terminal command such as:
```
sudo dpkg -i list-apt-installs-0.3-0ubuntu1.deb
```


### Build:

You will need certain packages in place to have a successful build: `build-essential cmake`

so download this source and extract the zip into a directory:
```
wget https://github.com/zcot/archive/list-apt-installs.zip
```
or use git clone method if you have git installed:
```
git clone https://github.com/zcot/list-apt-installs.git
```
or download and untar to some directory:
download, then:
```
tar -xzvf list-apt-installs.tar.gz
```

After you have obtained files then proceed in that directory:
to build it:
```
cmake .
make
sudo make install
```
and to uninstall it:
```
sudo make uninstall
```


