# Text Segmenter Library

Library to segment raw text (UTF-8) into typed segments using a set of regular expression.
An example of segmentation rule is in example/seg.data

## License

This library is under the [3-Clause BSD Licence](https://opensource.org/licenses/BSD-3-Clause). See [LICENSE.md](LICENSE.md)


## Author

Johannes Heinecke

## Requirements

needs `boost_regex` to run and `boost_regex_dev`, `curl` and  `python` to compile


## Build

the build process will download http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release [-DCMAKE_INSTALL_PREFIX=/usr/local]  ..
(you can also use `Debug`, especially vor test_valgrind)

    make
    make mini_test
    make valgrind_test
	
## Testing

    build/example/textSegmenter [options] example/seg.data example/text.txt
      options
	 --out json|col|long|short   define output format
	 --select                    output only segments which are not marled 'ignore'

## Install library
    cd build
    make install

## API

create instance of the segmenter:
    Segmenter *seg = new Segmenter(string data);

segment a text (read from UTF8 into a variable of the type wstring)
    string inputtext;
    Text result;
    boolean output_ignored_segments = false; // if true, segment types marked as ignored in the rules files if output.
    
    wchar_t *unicode = Unicode::fromUTF8(inputtext.c_str());
    seg->segment(wstring(unicode), result, boolean output_ignored_segments);




See [example/textSegmenter.cc](example/textSegmenter.cc) for more information. 
Do not forget to add `-I/usr/local/include/segmenter` to your c++ compiler

