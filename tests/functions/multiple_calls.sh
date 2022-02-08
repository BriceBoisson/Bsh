foo()
{
    echo "this is a test"
    echo $@
}

foo salut
foo
foo foo foo; foo a
