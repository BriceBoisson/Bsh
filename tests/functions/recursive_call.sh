test()
{
    if $@; then
        test false
    else
        echo end
    fi
}

test true
echo "did this work ?"
