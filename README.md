# Clamshell

A helper program for OS X that will execute a given command when your laptop lid is opened and kill it once the lid is closed.

## Getting Started

Clamshell is a self contained executable with no external dependencies beyond what OS X already provides through CoreFoundation and IOKit.

### Prerequisites

You will need a C compiler to build clamshell. Command line tools for development can be installed with:

``
$ xcode-select --install
``

### Installing

Once you have a working compiler installed build the project with make.

``
$ make
``

The application is installed into /usr/local by default, but that can be altered by specifying a different prefix.

``
$ make install prefix=/some/other/dir
``

## Usage

Capture the screen and share it with a remote rtmp server. Stop sharing when the laptop lid is closed and restart sharing when the lid is reopened.

``
$ clamshell ffmpeg -nostats -f avfoundation -i 1 -r 4 -s 1280x800 -f flv rtmp://192.168.0.1/youtube/streaming
``

## Built With

* [Neovim](https://neovim.io/) - literally the future of vim
* [Stackoverflow](https://stackoverflow.com/questions/20070333/obtain-model-identifier-string-on-os-x) - Question that showed me how to interact with the IO registry simply

## Contributing

Pull and feature requests are welcome. If you find this little program useful let me know!

## Authors

* Trey Dempsey** - *Initial work* - [Github](https://github.com/treydempsey)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
