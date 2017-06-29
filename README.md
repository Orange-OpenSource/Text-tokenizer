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

the build process will download http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt, which is needed to define the nature of unicode characters.

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release [-DCMAKE_INSTALL_PREFIX=/usr/local]  ..
(you can also use `Debug`, especially for `test_valgrind`)

    make [-j 4]
    make mini_test
    make valgrind_test
	
## Testing

    build/example/textSegmenter [options] example/seg.data example/text.txt
      options
	 --out json|col|long|short   define output format
	 --select                    output only segments which are not marked 'ignore'

## Install library
    cd build
    make install

## API

create an instance of the segmenter:

    Segmenter *seg = new Segmenter(string data);

segment a text (read from UTF8 into a variable of the type wstring)

    string inputtext; // input text in UTF-8
    Text result; // instance which contains the segmented text
    boolean output_ignored_segments = false; // if true, do not ouput segment types which are marked as "ignored" in the rules file

    wchar_t *unicode = Unicode::fromUTF8(inputtext.c_str());
    seg->segment(wstring(unicode), result, boolean output_ignored_segments);

    result.json(cout); // output the result in json to `stdout`
    res.longOutput(cerr); // output the result in a verbose format



See [example/textSegmenter.cc](example/textSegmenter.cc) for more information. 
Do not forget to add `-I/usr/local/include/segmenter` and `-llibsegment ` to your c++ compiler


