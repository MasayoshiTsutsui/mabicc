#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./mabicc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 10 "- -10;"
assert 10 "- - +10;"

assert 0 "0==1;"
assert 1 "42==42;"
assert 1 "0!=1;"
assert 0 "42!=42;"

assert 1 "0<1;"
assert 0 "1<1;"
assert 0 "2<1;"
assert 1 "0<=1;"
assert 1 "1<=1;"
assert 0 "2<=1;"

assert 1 "1>0;"
assert 0 "1>1;"
assert 0 "1>2;"
assert 1 "1>=0;"
assert 1 "1>=1;"
assert 0 "1>=2;"

assert 3 "a=3;"
assert 8 "a=3;b=5;a+b;"

assert 3 "foo=3;"
assert 8 "foo=3;bar=5;foo+bar;"

assert 3 "return 3;"
assert 3 "foo=3;return foo;"
assert 8 "foo=3;bar=5;return foo+bar;"

assert 2 "a = 0; a = 2;"

assert 3 "if (1) return 1 + 2; return 4 - 2;"
assert 1 "if (1) return 1; else return 2;"
assert 5 "a = 0; if (a) a = 9; else a = 5; return a;"
assert 9 "a = 1; if (a) a = 9; else a = 5; return a;"

assert 3 "a = 0; while (a < 3) a = a + 1; return a;"
assert 3 "a = 0; for (i = 0; i < 3; i = i + 1) a = a + 1; return a;"
assert 1 "a = 0; for (;;) return 1; return a;"
assert 3 "a = 0; for (;;) { a = a + 1; if (a == 3) return a; } return 5;"
assert 6 "a = 0; while (a < 6) { a = a + 1; if (a == 6) return a; } return 5;"
assert 4 "a = 0; for (i = 0; i < 4; i = i + 1) { a = a + 1; if (a == 4) return a; } return 5;"


echo OK
