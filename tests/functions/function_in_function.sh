foo()
{
    bar()
    {
        echo "bar"
        echo $@
    }
}

foo salut je suis un argument
bar salut