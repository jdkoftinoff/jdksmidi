
find . \( -name *.cpp -or -name *.h -or -name *.hpp \) -print -exec clang-format -i '{}' ';'

