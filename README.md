# list-apt-installs - Creates list from terminal logs of apt install command.


## What does this thing do?
It makes a list of the programs from when you type 'apt install' and 'apt-get install'.

## Why would I use it?
It works good for use in a backup scheme. You will have the list of programs that were installed by terminal so that you can restore them easily using, for example, Linux Mint's backup tool restore item.

You can also use the list in a file using the `apt-get dselect-upgrade` method.

The default output format is targeted for either of these methods.

It is also available to conversely format the output as a simple apt install script (you only need `chmod +x <filename>` and to execute the script).

So it can be easy to set up your favorite programs on a fresh install, or restore the programs after returning to a different system restore point.

Or you can share the list or script with a friend, to help them setup a special combination of programs.


## How can I use it?
You have to install it. You can use a package manager, or import it as an external package, or build/install it. Then simply run with command:
```
list-apt-installs
```
( There is also a symlink installed called `lai` so either command name is the same. )

Save output to a file in the same directory:
```
lai --output=initial-mint-favorites.list
```

Or put the list into a file which you will save into your backup scheme for later:
```
list-apt-installs > ~/backups/my-installed-stuff.list
```

Or make an apt install script file that can be used as an installer for another machine:
```
list-apt-installs --script -o /full/existing/absolute-path/my-stuff.installer
```

You can see the few simple command options available here:
```
lai --help
```
or here:
```
man list-apt-installs
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


